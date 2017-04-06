<?php


class rxmlrpcfix extends rXMLRPCRequest {
	public function run()
	{
	        $ret = false;
		$this->i8s = array();
		$this->strings = array();
		$this->val = array();
		if($this->makeCall())
		{
			$answer = self::send($this->content);

			
			if(!empty($answer))
			{
				if($this->parseByTypes)
				{
					if((preg_match_all("|<value><string>(.*)</string></value>|Us",$answer,$this->strings)!==false) &&
						count($this->strings)>1 &&
						(preg_match_all("|<value><i.>(.*)</i.></value>|Us",$answer,$this->i8s)!==false) &&
						count($this->i8s)>1) {
						$this->strings = str_replace("\\","\\\\",$this->strings[1]);
						$this->strings = str_replace("\"","\\\"",$this->strings);
						foreach($this->strings as &$string) 
							$string = html_entity_decode($string,ENT_COMPAT,"UTF-8");
						$this->i8s = $this->i8s[1];
						$ret = true;
	
					}
				} else {

					if((preg_match_all("/<value>(<string>|<i.>)(.*)(<\/string>|<\/i.>)<\/value>/s",$answer,$this->val)!==false) &&
						count($this->val)>2)
					{

						$this->val = str_replace("\\","\\\\",$this->val[2]);
						$this->val = str_replace("\"","\\\"",$this->val);

						foreach($this->val as &$string) 
							$string = html_entity_decode($string,ENT_COMPAT,"UTF-8");
						$ret = true;
					}
				}



				if($ret) {
					if(strstr($answer,"faultCode")!==false)
					{
						
						$this->fault = true;	
						if(LOG_RPC_FAULTS && $this->important)
						{
							toLog($this->content);
							toLog($answer);
						}
					}
				}
			}
		}
		$this->content = "";
		$this->commands = array();
		return($ret);
	}
}


?>