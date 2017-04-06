<?php
require_once( dirname(__FILE__)."/../../php/xmlrpc.php" );
require_once( dirname(__FILE__)."/../filemanager/flm.class.php" );
require_once( dirname(__FILE__)."/../filemanager/xmlfix.php" );
require_once( dirname(__FILE__)."/fileup.class.php" );


	$fup = new FILEUP();
	$jResult.= 'theWebUI.fileup = {};';
	$jResult.= 'theWebUI.fileup.services = '.$fup->get_settings().';';

	$theSettings->registerPlugin("fileupload");	
?>