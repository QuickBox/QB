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

// Returns all logged autodl-irssi lines

require_once 'getConf.php';

$command = Array("command" => "getlines");
if (isset($_GET["cid"]) && preg_match('/^\d+$/', $_GET["cid"]))
	$command["cid"] = $_GET["cid"];
$response = sendAutodlCommand($command);

$res = Array('error' => $response->error);
if (isset($response->lines)) {
	$res['lines'] = $response->lines;
	$res['cid'] = $response->cid;
}
$jsonData = json_encode($res);

header('Content-Type: application/json; charset=UTF-8');
echo $jsonData;

?>
