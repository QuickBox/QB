<?php
require_once('./config.php');
require_once( '../../php/xmlrpc.php' );
define('USE_NGINX', false);
define('SCHEME', 'https');
if ( !isset($_GET['f']) || empty($_GET['f']) || !file_exists($_GET['f']) || !preg_match('/.(avi|mkv|ts|mp4|flv|wmv|mpe?g|ogg|wma|mp3|flac)$/i', $_GET['f']) ) {
	header('HTTP/1.0 404 Not Found'); exit(); }

if ( USE_NGINX ) {
	header("Content-Type: application/octet-stream");
	header("X-Accel-Redirect: /stream{$_GET['f']}");
} else {
	sendFile($_GET['f']);
}
