<?php

// limits
// 0 = unlimited
$limits['duration'] = 1; 	// maximum duration hours
$limits['links'] = 0; 	//maximum sharing links per user

// path on domain where a symlink to share.php can be found
// example: http://mydomain.com/share.php
$downloadpath = $_SERVER['HTTP_HOST'] . '/fileshare/share.php';

?>
