<?php

class FSHARE extends FLM {


	protected $datafile;
	public $data = array();
	private $limits;


	public function __construct() {
		global $topDirectory;

		$this->datafile = addslash(getSettingsPath()).'fileshare.dat';
		$this->check_post($this->postlist);
		$this->load();
		$this->userdir = addslash($topDirectory);
	}

	public function islimited($max, $cur) {
		global $limits;

		return ($limits[$max]) ? (($cur <= $limits[$max]) ? false : true): false;
	}

	public function add($file, $duration, $password) {
		global $limits;

		$file = '/'.$file;
		$pch = explode($this->userdir, $file);

		
		if((count($pch) < 2) || (($stat = LFS::stat('/'.$file)) === FALSE)) {die('Invalid file'); }
		if(($duration < 1) || $this->islimited('duration', $duration)) {die('Invalid duration!');}

		if($this->islimited('links', count($this->data))) {die('Link limit reached');}

		do {$token = $this->random_chars();} while (isset($this->data[$token]));


		$this->data[$token] = array(
					 'file' => $file,
					 'size' => $stat['size'],
					 'expire' => time()+(3600*$duration),
					 'password' => $password);
		$this->write();
	}


	public function del($input) {
		$items = json_decode($input);
		if(!$items) { die('Invalid link id');}
		foreach($items as $id) {
			if(isset($this->data[$id])) {unset($this->data[$id]);}
		}
		$this->write();
	}

	public function edit($id, $duration, $password) {
		if(!isset($this->data[$id])) {die('Invalid link');}
		if($duration !== FALSE) {
			if(($duration < 1)|| $this->islimited('duration', $duration)) {die('Invalid duration!');}			
			$this->data[$id]['expire'] = time()+(3600*$duration);
		} 
		$this->data[$id]['password'] = $password;
		$this->write();
	}

	public function show() {
		$out = array( 'uh' => base64_encode(getUser()), 
				'list' => array());

		$out['list'] = $this->data;

		cachedEcho(json_encode($out));
	}


	public function random_chars($length = "32") {

		$rnd = '';

		for ($i=0; $i<$length; $i++) {
    			$lists[1] = rand(48,57);
    			$lists[2] = rand(65,90);
    			$lists[3] = rand(97,122);

			$randchar = $lists[rand(1,3)];

    			$rnd .= chr($randchar);
		} 

 		return $rnd;
	}


	protected function load() {
		if(!is_file($this->datafile) && !$this->write()) {die('Failed to write file');}
		
		if(($rf = file_get_contents($this->datafile)) === FALSE) {die('Failed to load data');}
		$this->data = unserialize($rf);
	}


	private function write() {

		return (file_put_contents($this->datafile, serialize($this->data)) !== FALSE);

	}

  	public function __destruct() {
   	}

}


?>