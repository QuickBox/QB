<?php
require_once( dirname(__FILE__)."/../../php/xmlrpc.php" );
require_once( dirname(__FILE__)."/../filemanager/flm.class.php" );
require_once( dirname(__FILE__)."/../filemanager/xmlfix.php" );
require_once( dirname(__FILE__)."/fileup.class.php" );



$fup = new FILEUP();


switch($fup->postlist['action']) {

	case 'save':
		$fup->save_settings();
		break;
	case 'upload':
		$fup->do_upload();
		break;
	case 'status':
		$fup->upload_status();
		break;
	case 'stop':
		$fup->stop_upload();
		break;
	default: $fup->sdie('Invalid action');

}


?>