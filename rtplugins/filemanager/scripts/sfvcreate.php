<?php

if(!is_dir($argv[1])) {

	if(!mkdir($argv[1], 0777, TRUE)) {die('Could not create temp dir');}
}


file_put_contents($argv[1].'/pid', getmypid());

$log  = fopen($argv[1].'/log', "a");

$files = array_slice($argv, 3);

$t = count($files);
$c = 1;

if (($sfv  = fopen($argv[2], "abt")) === FALSE) {
		fwrite($log, '1: SFV HASHING FAILED. File '.$argv[2]." not writable\n"); 
		fclose($log);

		sleep(20);
		exec('rm -rf '.escapeshellarg($argv[1]));

		die();
}

fwrite($sfv, "; ruTorrent File Manager Plugin by HWK\n;\n");


foreach($files as $file) {
	
	fwrite($log, '0: '.$c.'/'.$t.' hashing '.$file."\n");
	if(!is_file($file)) {fwrite($log, '0: '.$c.'/'.$t.' FAILED: no such file '.$file."\n"); continue;}

	$hash = hash_file('crc32b', $file);
	
	fwrite($sfv, end(explode('/', $file)).' '.$hash."\n");
	fwrite($log, '0: '.$c.'/'.$t.' OK -  Hash: '.$hash."\n");

	$c++;
}

fclose($sfv);
fwrite($log, "1: Done\n");
fclose($log);


sleep(20);
exec('rm -rf '.escapeshellarg($argv[1]));


?>