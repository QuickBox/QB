<?php
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is IRC Auto Downloader.
 *
 * The Initial Developer of the Original Code is
 * David Nilsson.
 * Portions created by the Initial Developer are Copyright (C) 2010, 2011
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * ***** END LICENSE BLOCK ***** */

require_once '../../php/util.php';
eval(getPluginConf('autodl-irssi'));

// Checks if there are missing PHP modules, and if so returns JSON data with an
// error message saying exactly which PHP modules are missing.
function checkPhpModules() {
	$missing = "";
	foreach (Array('xml', 'sockets', 'json') as $module) {
		if (!extension_loaded($module))
			$missing = "$missing $module";
	}
	if ($missing === "")
		return;

	$jsonData = "{\"error\":\"Missing PHP modules:$missing\"}";
	header('Content-Type: application/json; charset=UTF-8');
	echo $jsonData;
	exit(0);
}

// Sends a 304 if it's the same file (and exits) or returns if the file has changed
// with added ETag HTTP header.
function checkSameFile($etag, $mtime) {
	header("ETag: $etag");
	if (isset($_SERVER['HTTP_IF_NONE_MATCH']) &&
		trim($_SERVER['HTTP_IF_NONE_MATCH']) === $etag) {
		header("HTTP/1.1 304 Not Modified");
		exit;
	}
}

function getSocketError($socket) {
	$code = socket_last_error($socket);
	return "($code) " . socket_strerror($code);
}

function socketWriteAllData($socket, $data) {
	while (strlen($data)) {
		$lenWritten = socket_write($socket, $data, strlen($data));
		if ($lenWritten === false || $lenWritten <= 0)
			throw new Exception("Could not write to socket: " . getSocketError($socket));
		$data = substr($data, $lenWritten);
	}
}

function socketReadAllData($socket) {
	$data = "";
	while (1) {
		$val = socket_read($socket, 4096);
		if ($val === false)
			throw new Exception("Could not read from socket: " . getSocketError($socket));
		if ($val === "")
			break;
		$data .= $val;
	}
	return $data;
}

function sendAutodlCommand($data) {
	global $autodlPort, $autodlPassword;
	try {
		if ($autodlPort <= 0 || $autodlPort > 65535)
			throw new Exception("Invalid port ($autodlPort)! Initialize port in conf.php");

		$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
		if ($socket === false)
			throw new Exception("Could not create socket: " . getSocketError());

		if (!socket_connect($socket, "127.0.0.1", $autodlPort))
			throw new Exception("Could not connect: " . getSocketError($socket));

		$data['password'] = $autodlPassword;
		socketWriteAllData($socket, json_encode($data));
		$response = utf8_decode(socketReadAllData($socket));
		socket_close($socket);
		$jsonData = json_decode($response);
		if (is_null($jsonData))
			throw new Exception("Could not decode json string");
		if (!isset($jsonData->error))
			$jsonData->error = "";
		return $jsonData;
	}
	catch (Exception $ex) {
		$obj = (object)0;
		$obj->error = 'Error: ' . $ex->getMessage();
		return $obj;
	}
}

?>
