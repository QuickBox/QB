<?php
session_destroy();
include '/srv/rutorrent/php/util.php';
include ($_SERVER['DOCUMENT_ROOT'].'/widgets/class.php');
$version = "v2.5.2";
error_reporting(E_ERROR);
$master = file_get_contents('/srv/rutorrent/home/db/master.txt');
$master=preg_replace('/\s+/', '', $master);
$username = getUser();

require_once ($_SERVER['DOCUMENT_ROOT'].'/inc/localize.php');

// Network Interface
$interface = INETFACE;
$iface_list = array('INETFACE');
$iface_title['INETFACE'] = 'External';
$vnstat_bin = '/usr/bin/vnstat';
$data_dir = './dumps';
$byte_notation = null;

$dconf = '/home/'.$username.'/.config/deluge/web.conf';
if (file_exists($dconf)) {
    $dconf_data = file_get_contents($dconf);
    $dwport = search($dconf_data, '"port": ', ',');
    $dwssl = search($dconf_data, '"https": ', ',');
}

$szconf = '/home/'.$username.'/.sabnzbd/sabnzbd.ini';
if (file_exists($szconf)) {
    $szconf_data = file_get_contents($szconf);
    $szport = search($szconf_data, 'port = ', "\n");
    $szssl = search($szconf_data, 'https_port = ', "\n");
}

$zconf = '/srv/rutorrent/home/db/znc.txt';
if (file_exists($zconf)) {
    $zconf_data = file_get_contents($zconf);
    $zport = search($zconf_data, 'Port = ', "\n");
    $zssl = search($zconf_data, 'SSL = ', "\n");
}


function search($data, $find, $end) {
    $pos1 = strpos($data, $find) + strlen($find);
    $pos2 = strpos($data, $end, $pos1);
    return substr($data, $pos1, $pos2 - $pos1);
}

define('HTTP_HOST', preg_replace('~^www\.~i', '', $_SERVER['HTTP_HOST']));

$panel = array(
    'name'              => 'QuickBox Seedbox',
    'author'            => 'Everyone that contributes to the open QuickBox project!',
    'robots'            => 'noindex, nofollow',
    'title'             => 'Quickbox Dashboard',
    'description'       => 'QuickBox is an open-source seedbox project that is developed and maintained by anyone who so choses to provide time and energy. For more QuickBox, checkout https://plaza.quickbox.io',
    'active_page'       => basename($_SERVER['PHP_SELF']),
);

$time_start = microtime_float();

// Timing
function microtime_float() {
  $mtime = microtime();
  $mtime = explode(' ', $mtime);
  return $mtime[1] + $mtime[0];
}

//Unit Conversion
function formatsize($size) {
  $danwei=array(' B ',' KB ',' MB ',' GB ',' TB ');
  $allsize=array();
  $i=0;
  for($i = 0; $i <5; $i++) {
    if(floor($size/pow(1024,$i))==0){break;}
  }
  for($l = $i-1; $l >=0; $l--) {
    $allsize1[$l]=floor($size/pow(1024,$l));
    $allsize[$l]=$allsize1[$l]-$allsize1[$l+1]*1024;
  }
  $len=count($allsize);
  for($j = $len-1; $j >=0; $j--) {
    $fsize=$fsize.$allsize[$j].$danwei[$j];
  }
  return $fsize;
}

function GetCoreInformation() {$data = file('/proc/stat');$cores = array();foreach( $data as $line ) {if( preg_match('/^cpu[0-9]/', $line) ){$info = explode(' ', $line);$cores[]=array('user'=>$info[1],'nice'=>$info[2],'sys' => $info[3],'idle'=>$info[4],'iowait'=>$info[5],'irq' => $info[6],'softirq' => $info[7]);}}return $cores;}
function GetCpuPercentages($stat1, $stat2) {if(count($stat1)!==count($stat2)){return;}$cpus=array();for( $i = 0, $l = count($stat1); $i < $l; $i++) { $dif = array(); $dif['user'] = $stat2[$i]['user'] - $stat1[$i]['user'];$dif['nice'] = $stat2[$i]['nice'] - $stat1[$i]['nice'];  $dif['sys'] = $stat2[$i]['sys'] - $stat1[$i]['sys'];$dif['idle'] = $stat2[$i]['idle'] - $stat1[$i]['idle'];$dif['iowait'] = $stat2[$i]['iowait'] - $stat1[$i]['iowait'];$dif['irq'] = $stat2[$i]['irq'] - $stat1[$i]['irq'];$dif['softirq'] = $stat2[$i]['softirq'] - $stat1[$i]['softirq'];$total = array_sum($dif);$cpu = array();foreach($dif as $x=>$y) $cpu[$x] = round($y / $total * 100, 2);$cpus['cpu' . $i] = $cpu;}return $cpus;}
$stat1 = GetCoreInformation();sleep(1);$stat2 = GetCoreInformation();$data = GetCpuPercentages($stat1, $stat2);
$cpu_show = $data['cpu0']['user']."%us,  ".$data['cpu0']['idle']."%id,  ";

// Information obtained depending on the system CPU
switch(PHP_OS)
{
  case "Linux":
    $sysReShow = (false !== ($sysInfo = sys_linux()))?"show":"none";
  break;

  case "FreeBSD":
    $sysReShow = (false !== ($sysInfo = sys_freebsd()))?"show":"none";
  break;

  default:
  break;
}

//linux system detects
function sys_linux()
{
    // CPU
    if (false === ($str = @file("/proc/cpuinfo"))) return false;
    $str = implode("", $str);
    @preg_match_all("/model\s+name\s{0,}\:+\s{0,}([^\:]+)([\r\n]+)/s", $str, $model);
    @preg_match_all("/cpu\s+MHz\s{0,}\:+\s{0,}([\d\.]+)[\r\n]+/", $str, $mhz);
    @preg_match_all("/cache\s+size\s{0,}\:+\s{0,}([\d\.]+\s{0,}[A-Z]+[\r\n]+)/", $str, $cache);
    if (false !== is_array($model[1]))
  {
        $res['cpu']['num'] = sizeof($model[1]);

    if($res['cpu']['num']==1)
      $x1 = '';
    else
      $x1 = ' ×'.$res['cpu']['num'];
    $mhz[1][0] = ' <span style="color:#999;font-weight:600">Frequency:</span> '.$mhz[1][0];
    $cache[1][0] = ' <br /> <span style="color:#999;font-weight:600">Secondary cache:</span> '.$cache[1][0];
    $res['cpu']['model'][] = '<h4>'.$model[1][0].'</h4>'.$mhz[1][0].$cache[1][0].$x1;
        if (false !== is_array($res['cpu']['model'])) $res['cpu']['model'] = implode("<br />", $res['cpu']['model']);
        if (false !== is_array($res['cpu']['mhz'])) $res['cpu']['mhz'] = implode("<br />", $res['cpu']['mhz']);
        if (false !== is_array($res['cpu']['cache'])) $res['cpu']['cache'] = implode("<br />", $res['cpu']['cache']);
  }

    return $res;
}

//FreeBSD system detects
function sys_freebsd()
{
  //CPU
  if (false === ($res['cpu']['num'] = get_key("hw.ncpu"))) return false;
  $res['cpu']['model'] = get_key("hw.model");
  return $res;
}

//Obtain the parameter values FreeBSD
function get_key($keyName)
{
  return do_command('sysctl', "-n $keyName");
}

//Determining the location of the executable file FreeBSD
function find_command($commandName)
{
  $path = array('/bin', '/sbin', '/usr/bin', '/usr/sbin', '/usr/local/bin', '/usr/local/sbin');
  foreach($path as $p)
  {
    if (@is_executable("$p/$commandName")) return "$p/$commandName";
  }
  return false;
}

//Order Execution System FreeBSD
function do_command($commandName, $args)
{
  $buffer = "";
  if (false === ($command = find_command($commandName))) return false;
  if ($fp = @popen("$command $args", 'r'))
  {
    while (!@feof($fp))
    {
      $buffer .= @fgets($fp, 4096);
    }
    return trim($buffer);
  }
  return false;
}


function GetWMI($wmi,$strClass, $strValue = array()) {
  $arrData = array();

  $objWEBM = $wmi->Get($strClass);
  $arrProp = $objWEBM->Properties_;
  $arrWEBMCol = $objWEBM->Instances_();
  foreach($arrWEBMCol as $objItem) {
    @reset($arrProp);
    $arrInstance = array();
    foreach($arrProp as $propItem) {
      eval("\$value = \$objItem->" . $propItem->Name . ";");
      if (empty($strValue)) {
        $arrInstance[$propItem->Name] = trim($value);
      } else {
        if (in_array($propItem->Name, $strValue)) {
          $arrInstance[$propItem->Name] = trim($value);
        }
      }
    }
    $arrData[] = $arrInstance;
  }
  return $arrData;
}

//NIC flow
$strs = @file("/proc/net/dev");

for ($i = 2; $i < count($strs); $i++ ) {
  preg_match_all( "/([^\s]+):[\s]{0,}(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)/", $strs[$i], $info );
  $NetOutSpeed[$i] = $info[10][0];
  $NetInputSpeed[$i] = $info[2][0];
  $NetInput[$i] = formatsize($info[2][0]);
  $NetOut[$i]  = formatsize($info[10][0]);
}

//Real-time refresh ajax calls
if ($_GET['act'] == "rt") {
  $arr=array('NetOut2'=>"$NetOut[2]",'NetOut3'=>"$NetOut[3]",'NetOut4'=>"$NetOut[4]",'NetOut5'=>"$NetOut[5]",'NetOut6'=>"$NetOut[6]",'NetOut7'=>"$NetOut[7]",'NetOut8'=>"$NetOut[8]",'NetOut9'=>"$NetOut[9]",'NetOut10'=>"$NetOut[10]",'NetInput2'=>"$NetInput[2]",'NetInput3'=>"$NetInput[3]",'NetInput4'=>"$NetInput[4]",'NetInput5'=>"$NetInput[5]",'NetInput6'=>"$NetInput[6]",'NetInput7'=>"$NetInput[7]",'NetInput8'=>"$NetInput[8]",'NetInput9'=>"$NetInput[9]",'NetInput10'=>"$NetInput[10]",'NetOutSpeed2'=>"$NetOutSpeed[2]",'NetOutSpeed3'=>"$NetOutSpeed[3]",'NetOutSpeed4'=>"$NetOutSpeed[4]",'NetOutSpeed5'=>"$NetOutSpeed[5]",'NetInputSpeed2'=>"$NetInputSpeed[2]",'NetInputSpeed3'=>"$NetInputSpeed[3]",'NetInputSpeed4'=>"$NetInputSpeed[4]",'NetInputSpeed5'=>"$NetInputSpeed[5]");
  $jarr=json_encode($arr);
  $_GET['callback'] = htmlspecialchars($_GET['callback']);
  echo $_GET['callback'],'(',$jarr,')';
  exit;
}

function session_start_timeout($timeout=5, $probability=100, $cookie_domain='/') {
  ini_set("session.gc_maxlifetime", $timeout);
  ini_set("session.cookie_lifetime", $timeout);
  $seperator = strstr(strtoupper(substr(PHP_OS, 0, 3)), "WIN") ? "\\" : "/";
  $path = ini_get("session.save_path") . $seperator . "session_" . $timeout . "sec";
  if(!file_exists($path)) {
    if(!mkdir($path, 600)) {
      trigger_error("Failed to create session save path directory '$path'. Check permissions.", E_USER_ERROR);
    }
  }
  ini_set("session.save_path", $path);
  ini_set("session.gc_probability", $probability);
  ini_set("session.gc_divisor", 100);
  session_start();
  if(isset($_COOKIE[session_name()])) {
    setcookie(session_name(), $_COOKIE[session_name()], time() + $timeout, $cookie_domain);
  }
}

session_start_timeout(5);
$MSGFILE = session_id();

function processExists($processName, $username) {
  $exists= false;
  exec("ps axo user:20,pid,pcpu,pmem,vsz,rss,tty,stat,start,time,comm,cmd|grep $username | grep -iE $processName | grep -v grep", $pids);
  if (count($pids) > 0) {
    $exists = true;
  }
  return $exists;
}

$btsync = processExists("resilio-sync",rslsync);
$deluged = processExists("deluged",$username);
$delugedweb = processExists("deluge-web",$username);
$emby = processExists("emby-server",$username);
$headphones = processExists("headphones",$username);
$irssi = processExists("irssi",$username);
$nzbget = processExists("nzbget",$username);
$nzbhydra = processExists("nzbhydra",$username);
$ombi = processExists("ombi",$username);
$plex = processExists("Plex",plex);
$plexpy = processExists("plexpy",plexpy);
$pyload = processExists("pyload",$username);
$radarr = processExists("radarr",$username);
$rtorrent = processExists("rtorrent",$username);
$sabnzbd = processExists("sabnzbd",$username);
$sickrage = processExists("sickrage",$username);
$medusa = processExists("medusa",$username);
$sonarr = processExists("nzbdrone",$username);
$subsonic = processExists("subsonic",$username);
$syncthing = processExists("syncthing",$username);
$jackett = processExists("jackett",$username);
$couchpotato = processExists("couchpotato",$username);
$quassel = processExists("quassel",$username);
$shellinabox = processExists("shellinabox",shellinabox);
$csf = processExists("lfd",root);
$sickgear = processExists("sickgear",8088);
$nzbget = processExists("nzbget",$username);
$znc = processExists("znc",$username);

function isEnabled($process, $username){
  $service = $process;
  if(file_exists('/etc/systemd/system/multi-user.target.wants/'.$process.'@'.$username.'.service') || file_exists('/etc/systemd/system/multi-user.target.wants/'.$process.'.service')){
    return " <div class=\"toggle-wrapper text-center\"> <div class=\"toggle-en toggle-light primary\" onclick=\"location.href='?id=77&servicedisable=$service'\"></div></div>";
  } else {
    return " <div class=\"toggle-wrapper text-center\"> <div class=\"toggle-dis toggle-light primary\" onclick=\"location.href='?id=66&serviceenable=$service'\"></div></div>";
  }
}

if(file_exists('/srv/rutorrent/home/custom/url.override.php')){
  // BEGIN CUSTOM URL OVERRIDES //
  include ($_SERVER['DOCUMENT_ROOT'].'/custom/url.override.php');
  // END CUSTOM URL OVERRIDES ////
} else {
  $btsyncURL = "http://" . $_SERVER['HTTP_HOST'] . ":8888/gui/";
  $cpURL = "https://" . $_SERVER['HTTP_HOST'] . "/couchpotato";
  $csfURL = "https://" . $_SERVER['HTTP_HOST'] . ":3443";
  if ($dwssl == "true") { $dwURL = "https://" . $_SERVER['HTTP_HOST'] . ":$dwport"; }
  if ($dwssl == "false") { $dwURL = "http://" . $_SERVER['HTTP_HOST'] . ":$dwport"; }
  $embyURL = "https://" . $_SERVER['HTTP_HOST'] . "/emby";
  $headphonesURL = "https://" . $_SERVER['HTTP_HOST'] . "/headphones/home";
  $jackettURL = "https://" . $_SERVER['HTTP_HOST'] . "/jackett/UI/Dashboard";
  $nextcloudURL = "https://" . $_SERVER['HTTP_HOST'] . "/nextcloud";
  $nzbgetURL = "https://" . $_SERVER['HTTP_HOST'] . "/nzbget";
  $nzbhydraURL = "https://" . $_SERVER['HTTP_HOST'] . "/nzbhydra";
  $plexURL = "http://" . $_SERVER['HTTP_HOST'] . ":31400/web/";
  $plexpyURL = "https://" . $_SERVER['HTTP_HOST'] . "/plexpy";
  $ombiURL = "https://" . $_SERVER['HTTP_HOST'] . "/ombi";
  $pyloadURL = "https://" . $_SERVER['HTTP_HOST'] . "/pyload/login";
  $radarrURL = "https://" . $_SERVER['HTTP_HOST'] . "/radarr";
  $rapidleechURL = "https://" . $_SERVER['HTTP_HOST'] . "/rapidleech";
  $sabnzbdURL = "https://" . $_SERVER['HTTP_HOST'] . "/sabnzbd";
  $sickgearURL = "https://" . $_SERVER['HTTP_HOST'] . "/sickgear";
  $sickrageURL = "https://" . $_SERVER['HTTP_HOST'] . "/sickrage";
  $medusaURL = "https://" . $_SERVER['HTTP_HOST'] . "/medusa";
  $sonarrURL = "https://" . $_SERVER['HTTP_HOST'] . "/sonarr";
  $subsonicURL = "https://" . $_SERVER['HTTP_HOST'] . "/subsonic";
  $syncthingURL = "https://" . $_SERVER['HTTP_HOST'] . "/syncthing/";
  if ($zssl == "true") { $zncURL = "https://" . $_SERVER['HTTP_HOST'] . ":$zport"; }
  if ($zssl == "false") { $zncURL = "http://" . $_SERVER['HTTP_HOST'] . ":$zport"; }
 }
 
include ($_SERVER['DOCUMENT_ROOT'].'/widgets/lang_select.php');
include ($_SERVER['DOCUMENT_ROOT'].'/widgets/plugin_data.php');
include ($_SERVER['DOCUMENT_ROOT'].'/widgets/package_data.php');
include ($_SERVER['DOCUMENT_ROOT'].'/widgets/sys_data.php');
include ($_SERVER['DOCUMENT_ROOT'].'/widgets/theme_select.php');
$base = 1024;
$location = "/home";

/* check for services */
switch (intval($_GET['id'])) {
case 0:
  $rtorrent = isEnabled("rtorrent", $username);
    $cbodyr .= $rtorrent;
  $irssi = isEnabled("irssi", $username);
    $cbodyi .= $irssi;
  $deluged = isEnabled("deluged", $username);
    $cbodyd .= $deluged;
  $delugedweb = isEnabled("deluge-web", $username);
    $cbodydw .= $delugedweb;
  $shellinabox = isEnabled("shellinabox",shellinabox);
    $wcbodyb .= $shellinabox;
  $btsync = isEnabled("resilio-sync",rslsync);
    $cbodyb .= $btsync;
  $couchpotato = isEnabled("couchpotato", $username);
    $cbodycp .= $couchpotato;
  $emby = isEnabled("emby-server", $username);
    $cbodye .= $emby;
  $headphones = isEnabled("headphones", $username);
    $cbodyhp .= $headphones;
  $jackett = isEnabled("jackett", $username);
    $cbodyj .= $jackett;
  $nzbget = isEnabled("nzbget", $username);
    $cbodynzg .= $nzbget;
  $nzbhydra = isEnabled("nzbhydra", $username);
    $cbodynzb .= $nzbhydra;
  $ombi = isEnabled("ombi", $username);
    $cbodypr .= $ombi;
  $plex = isEnabled("plexmediaserver",plex);
    $cbodyp .= $plex;
  $plexpy = isEnabled("plexpy",plexpy);
    $cbodypp .= $plexpy;
  $pyload = isEnabled("pyload", $username);
    $cbodypl .= $pyload;
  $quassel = isEnabled("quassel", $username);
    $cbodyq .= $quassel;
  $radarr = isEnabled("radarr", $username);
    $cbodyrad .= $radarr;
  $rapidleech = isEnabled("rapidleech", $username);
    $cbodyrl .= $rapidleech;
  $sabnzbd = isEnabled("sabnzbd", $username);
    $cbodysz .= $sabnzbd;
  $sickgear = isEnabled("sickgear", 8088);
    $cbodysg .= $sickgear;
  $sickrage = isEnabled("sickrage", $username);
    $cbodysr .= $sickrage;
  $medusa = isEnabled("medusa", $username);
    $cbodym .= $medusa;
  $sonarr = isEnabled("sonarr", $username);
    $cbodys .= $sonarr;
  $subsonic = isEnabled("subsonic", root);
    $cbodyss .= $subsonic;
  $syncthing = isEnabled("syncthing", $username);
    $cbodyst .= $syncthing;
  $x2go = isEnabled("x2go", $username);
    $cbodyx .= $x2go;
  $znc = isEnabled("znc", $username);
    $cbodyz .= $znc;

break;

/* enable & start services */
case 66:
  $process = $_GET['serviceenable'];
    if ($process == "resilio-sync"){
      shell_exec("sudo systemctl enable $process");
      shell_exec("sudo systemctl start $process");
    } elseif ($process == "shellinabox"){
      shell_exec("sudo systemctl enable $process");
      shell_exec("sudo systemctl start $process");
    } elseif ($process == "emby-server"){
      shell_exec("sudo systemctl enable $process");
      shell_exec("sudo systemctl start $process");
    } elseif ($process == "headphones"){
      shell_exec("sudo systemctl enable $process");
      shell_exec("sudo systemctl start $process");
    } elseif ($process == "nzbget"){
      shell_exec("sudo systemctl enable $process");
      shell_exec("sudo systemctl start $process");
    } elseif ($process == "plexmediaserver"){
      shell_exec("sudo systemctl enable $process");
      shell_exec("sudo systemctl start $process");
    } elseif ($process == "plexpy"){
      shell_exec("sudo systemctl enable $process");
      shell_exec("sudo systemctl start $process");
    } elseif ($process == "ombi"){
      shell_exec("sudo systemctl enable $process");
      shell_exec("sudo systemctl start $process");
    } elseif ($process == "radarr"){
      shell_exec("sudo systemctl enable $process");
      shell_exec("sudo systemctl start $process");
    } elseif ($process == "subsonic"){
      shell_exec("sudo systemctl enable $process");
      shell_exec("sudo systemctl start $process");
    } else {
      shell_exec("sudo systemctl enable $process@$username");
      shell_exec("sudo systemctl start $process@$username");
    }
  header('Location: https://' . $_SERVER['HTTP_HOST'] . '/');
break;

/* disable & stop services */
case 77:
  $process = $_GET['servicedisable'];
    if ($process == "resilio-sync"){
      shell_exec("sudo systemctl stop $process");
      shell_exec("sudo systemctl disable $process");
    } elseif ($process == "shellinabox"){
      shell_exec("sudo systemctl stop $process");
      shell_exec("sudo systemctl disable $process");
    } elseif ($process == "emby-server"){
      shell_exec("sudo systemctl stop $process");
      shell_exec("sudo systemctl disable $process");
    } elseif ($process == "headphones"){
      shell_exec("sudo systemctl stop $process");
      shell_exec("sudo systemctl disable $process");
    } elseif ($process == "nzbget"){
      shell_exec("sudo systemctl stop $process");
      shell_exec("sudo systemctl disable $process");
    } elseif ($process == "plexmediaserver"){
      shell_exec("sudo systemctl stop $process");
      shell_exec("sudo systemctl disable $process");
    } elseif ($process == "plexpy"){
      shell_exec("sudo systemctl stop $process");
      shell_exec("sudo systemctl disable $process");
    } elseif ($process == "ombi"){
      shell_exec("sudo systemctl stop $process");
      shell_exec("sudo systemctl disable $process");
    } elseif ($process == "radarr"){
      shell_exec("sudo systemctl stop $process");
      shell_exec("sudo systemctl disable $process");
    } elseif ($process == "subsonic"){
      shell_exec("sudo systemctl stop $process");
      shell_exec("sudo systemctl disable $process");
    } else {
      shell_exec("sudo systemctl stop $process@$username");
      shell_exec("sudo systemctl disable $process@$username");
    }
  header('Location: https://' . $_SERVER['HTTP_HOST'] . '/');
break;

/* restart services */
case 88:
  $process = $_GET['servicestart'];
    if ($process == "resilio-sync"){
      shell_exec("sudo systemctl enable $process");
      shell_exec("sudo systemctl restart $process");
    } elseif ($process == "shellinabox"){
      shell_exec("sudo systemctl enable $process");
      shell_exec("sudo systemctl restart $process");
    } elseif ($process == "emby-server"){
      shell_exec("sudo systemctl enable $process");
      shell_exec("sudo systemctl restart $process");
    } elseif ($process == "headphones"){
      shell_exec("sudo systemctl enable $process");
      shell_exec("sudo systemctl restart $process");
    } elseif ($process == "nzbget"){
      shell_exec("sudo systemctl enable $process");
      shell_exec("sudo systemctl restart $process");
    } elseif ($process == "plexmediaserver"){
      shell_exec("sudo systemctl enable $process");
      shell_exec("sudo systemctl restart $process");
    } elseif ($process == "plexpy"){
      shell_exec("sudo systemctl enable $process");
      shell_exec("sudo systemctl restart $process");
    } elseif ($process == "ombi"){
      shell_exec("sudo systemctl enable $process");
      shell_exec("sudo systemctl restart $process");
    } elseif ($process == "radarr"){
      shell_exec("sudo systemctl enable $process");
      shell_exec("sudo systemctl restart $process");
    } elseif ($process == "subsonic"){
      shell_exec("sudo systemctl enable $process");
      shell_exec("sudo systemctl restart $process");
    } else {
      shell_exec("sudo systemctl restart $process@$username");
    }
  header('Location: https://' . $_SERVER['HTTP_HOST'] . '/');
break;

}

?>
