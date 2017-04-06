<?php



if(!is_dir($argv[1])) {

	if(!mkdir($argv[1], 0777, TRUE)) {die('Could not create temp dir');}
}

file_put_contents($argv[1].'/pid', getmypid());

$path = explode('/', $argv[2]);
array_pop($path);
$path = implode('/', $path);


$hi = array();

$fr = file($argv[2]);

$filelines = array();
foreach($fr as $fl) {if (substr(trim($fl), 0, 1) == ';') {continue;} $filelines[] = $fl;}

$t = count($filelines);
$c = 1;


$log  = fopen($argv[1].'/log', "a");

foreach($filelines as $line) {



	$hi['file'] = explode(' ', trim($line));
	$hi['crc32'] = array_pop($hi['file']);
	$hi['file'] = implode(' ', $hi['file']);

	$filepath= $path.'/'.$hi['file'];
	
	fwrite($log, '0: '.$c.'/'.$t.' checking '.$hi['file']."\n");
	if(!is_file($filepath)) {fwrite($log, '0: FAILED: no such file '.$filepath."\n"); continue;}
	if(hash_file('crc32b', $filepath) != $hi['crc32']) { fwrite($log, '0: FAILED: files dont match - '.$filepath."\n"); continue;}

	fwrite($log, "0: OK: files match\n");

	$c++;
}


fwrite($log, "1: Done\n");
fclose($log);

sleep(20);


exec('rm -rf '.escapeshellarg($argv[1]));


?>