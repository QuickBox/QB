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

// The JS code calls this file to read the autodl.cfg file and all *.tracker files.
// Only those files can be returned.

require_once 'getConf.php';

function notFound() {
	header("HTTP/1.1 404 Not Found");
	echo "<h1>Not Found</h1><p>404</p>";
	exit;
}

if (!isset($_GET["file"]))
	notFound();
$filename = trim($_GET["file"]);
$command = Array("command" => "getfile", "name" => $filename);
$response = sendAutodlCommand($command);

if ($response->error)
	notFound();

$mtime = $response->mtime;
$etag = '"' . md5($mtime . '-' . $filename) . '"';
checkSameFile($etag, $mtime);

if ($filename === 'autodl.cfg')
	$contentType = 'text/plain';
else
	$contentType = 'application/xml';

header("Content-Type: $contentType; charset=UTF-8");
header("Content-Disposition: attachment; filename=$filename");
echo $response->data;

?>
