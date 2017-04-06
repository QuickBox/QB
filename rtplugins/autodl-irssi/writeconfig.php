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

// Writes to autodl.cfg

require_once 'getConf.php';

$error = "";
if (!isset($_FILES['file']))
	$error = "No file uploaded!";
else {
	$file = $_FILES['file'];
	if ($file['error'] !== UPLOAD_ERR_OK)
		$error = "Error uploading the file, code: " . $file['error'];
}

if (!$error) {
	$data = file_get_contents($file['tmp_name']);
	if ($data === false)
		$error = "Could not read uploaded file";
}

if (!$error) {
	$command = Array("command" => "writeconfig", "data" => $data);
	$response = sendAutodlCommand($command);
	$error = $response->error;
}
if (is_null($error))
	$error = "";

if (isset($_POST['restoring']) && $_POST['restoring'] === "1") {
	header('Content-Type: text/html; charset=UTF-8');
}
else {
	$result = Array("error" => $error);
	$jsonData = json_encode($result);
	header('Content-Type: application/json; charset=UTF-8');
	echo $jsonData;
}

?>
