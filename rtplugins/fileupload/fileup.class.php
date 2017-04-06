<?php

class FILEUP extends FLM {
	
	private $datafile;
	private $services = array();
	private $fields = array('login', 'password');
		
	public function __construct() {
		global $topDirectory;
		eval(getPluginConf('fileupload'));
		
		$this->services = $services;

		$this->datafile = addslash(getSettingsPath()).'fileuploads.dat';
		$this->check_post($this->postlist);
		$this->load();
		$this->userdir = addslash($topDirectory);
		$this->shout = false;
		$this->xmlrpc = new rxmlrpcfix();
		$this->settings = array('tempdir' => '/tmp');
	}
	
	
	public function has_login($service) {
		
		foreach ($this->fields as $req) {
			if(isset($this->services[$service]['account']) && isset($this->services[$service][$req]) && empty($this->services[$service][$req])) {return false;}
		}
		
		return true;
	}
	
	public function login_isset($service) {
		
		foreach ($this->fields as $req) {
			if(empty($this->services[$service][$req])) {return false;}
		}
		
		return true;
	}
	
	public function do_upload() {

		$file = '/'.$this->postlist['file'];
		if(	!$this->postlist['target'] 
			|| !$this->postlist['file'] 
			|| !$this->remote_test($file, 'f')
			|| !isset($this->services[$this->postlist['target']])
			|| !$this->has_login($this->postlist['target'])
			|| (count(explode($this->userdir, $file)) < 2)) 
		{
				$this->sdie('Invalid parameters');
		}

		$this->make_temp();
		$this->shout = true;	
		$this->upload();
		
	}
		
	protected function load() {
		if(!is_file($this->datafile) && !$this->write()) {die('Failed to write file');}
		
		if(($rf = file_get_contents($this->datafile)) === FALSE) {die('Failed to load data');}
		$this->services = unserialize($rf);
	}
	
	public function get_settings($encode = true) {
		
		eval(getPluginConf('fileupload'));
		
		$set = array();
		foreach($services as $n => $ser) {
			if($ser['enabled']) {$set[$n] = $this->services[$n]; unset($set[$n]['enabled']);}
		}
		return $encode ? json_encode($set ) : $set;
		
	}
	
	public function save_settings() {

		if(!$this->postlist['fls'] || (($settings = json_decode($this->postlist['fls'], true)) === FALSE)) {$this->sdie('Invalid settings');}

		eval(getPluginConf('fileupload'));
		foreach($services as $s => $o) {
			
			if($o['enabled']) {
				if(!isset($settings[$s])) {$this->sdie('Invalid settings');}
				if($s == 'megaupload') {$services[$s]['email'] = $settings[$s]['email'];}
				foreach($this->fields as $v) {
					if(!isset($o[$v])) {continue 2;}
					if(array_key_exists($v,$o) && !array_key_exists($v,$settings[$s])) {$this->sdie('Invalid settings');}
					$services[$s][$v] = $settings[$s][$v];
				}
			}

		}
		
		$this->services = $services;
		$this->write();
		$this->shout = true;
		
	}
	
	public function stop_upload() {
		
		$this->shout = true;
		$this->kill($this->postlist['target']);
	}


	public function upload() {
		
		$script = dirname(__FILE__).'/scripts/upload';
		$arguments = escapeshellarg($script).' '.escapeshellarg($this->temp['dir']).' '.
						escapeshellarg($this->postlist['target']).' ';
								
		if($this->login_isset($this->postlist['target'])) {
			$arguments .= escapeshellarg('--auth='.$this->services[$this->postlist['target']]['login'].':'.$this->services[$this->postlist['target']]['password']);
		}
		

		if($this->postlist['mode'] !== FALSE) {
					$arguments .= ' '.escapeshellarg('--description='.$this->postlist['mode']);
		}
		if($this->postlist['to'] !== FALSE) {
					$arguments .= ' '.escapeshellarg('--email-to='.$this->postlist['to']);
		}
		
		$arguments .= ' '.escapeshellarg('/'.$this->postlist['file']);

		$this->batch_exec(array("sh", "-c", " ".$arguments));
	
	}


	public function upload_status() {
		
		$this->shout = true;
		$this->output['done'] = false;

		if(!$this->postlist['target']) {$this->sdie('Invalid session');}

		$log['file'] = addslash($this->settings['tempdir']).'.rutorrent/.fman/'.$this->postlist['target'].'/log';
		if(!is_file($log['file'])) {$this->output['errcode'] = 21; return false;}

		$log['contents'] = file_get_contents($log['file']);
		$this->output['status'] = $log['contents'];
		
		if(preg_match('/website error:/', $log['contents'])) {
				 $this->output['errcode'] = 1; 
				 return false;
		}elseif(preg_match('/(https?\S+)(\s\((\S+)\))?[\s]+?$/', $log['contents'], $matches)) {
			$this->output['status'] = preg_replace('/(https?\S+)(\s\((\S+)\))?[\s]+?$/', '', $log['contents']);
			$this->output['done'] = true;	
			if(isset($matches[3])) {
				$this->output['dellink'] = $matches[3];
			}
			$this->output['dllink'] = $matches[1];

		} 
	
	}

	private function write() {

		return (file_put_contents($this->datafile, serialize($this->services)) !== FALSE);

	}
		
}

?>
	