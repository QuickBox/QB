<?php

require_once( dirname(__FILE__)."/../../php/cache.php" );
require_once( dirname(__FILE__)."/../filemanager/flm.class.php" );
include( dirname(__FILE__).'/share.class.php');
include( dirname(__FILE__).'/conf.php');


if(!isset($_POST['action'])) {die('Invalid action');}

$f = new FSHARE();

switch($f->postlist['action']) {

	case 'list':
		$f->show();
		break;
	case 'del':
		$f->del($f->postlist['target']);
		$f->show();
		break;
	case 'add':
		$f->add($f->postlist['file'], $f->postlist['target'], $f->postlist['to']);
		$f->show();
		break;
	case 'edit':
		$f->edit($f->postlist['file'], $f->postlist['target'], $f->postlist['to']);
		$f->show();
		break;
	default: die('Invalid action');

}




?>