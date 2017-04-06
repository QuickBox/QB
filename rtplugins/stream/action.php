<?php
require_once('./config.php');
require_once( '../../php/xmlrpc.php' );

if(!empty($auth))
	$auth .= '@';

if(isset($_REQUEST['result']))
	cachedEcho('log(theUILang.cantAccessData);',"text/html");

if(isset($_REQUEST['hash']) && isset($_REQUEST['no']))
{
	$req = new rXMLRPCRequest( 
		new rXMLRPCCommand( "f.get_frozen_path", array($_REQUEST['hash'],intval($_REQUEST['no']))) );
	if($req->success())
	{
		$filename = $req->val[0];
		if($filename=='')
		{
			$req = new rXMLRPCRequest( array(
				new rXMLRPCCommand( "d.open", $_REQUEST['hash'] ),
				new rXMLRPCCommand( "f.get_frozen_path", array($_REQUEST['hash'],intval($_REQUEST['no'])) ),
				new rXMLRPCCommand( "d.close", $_REQUEST['hash'] ) ) );
			if($req->success())
				$filename = $req->val[1];
		}

		$out = '<?xml version="1.0" encoding="UTF-8"?>
<playlist version="1" xmlns="http://xspf.org/ns/0/">
	<trackList>
	<track>
		<title>' . basename($filename) . '</title>
		<location>' . SCHEME . '://' . $auth . $_SERVER['HTTP_HOST'] . dirname($_SERVER['DOCUMENT_URI']) . '/plugins/stream/stream.php?f=' . urlencode($filename) .'</location>
	</track>
	</trackList>
</playlist>';

		header("Content-type: application/vlc");
		header('Content-Disposition: attachment; filename="' . basename($filename) . '.xspf"');
		header("Content-Length: ".strlen($out));
		echo $out;
		exit;
	}
}

if(isset($_REQUEST['readable']))
	cachedEcho("Cant retrieve such large file, sorry","text/html");
else
{
	header("HTTP/1.0 302 Moved Temporarily");
	header("Location: ".$_SERVER['PHP_SELF'].'?result=0');
}
?>
