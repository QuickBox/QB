<?php

include '/srv/rutorrent/php/util.php';
$username = getUser();

function processExists($processName, $username) {
  $exists= false;
  exec("ps axo user:20,pid,pcpu,pmem,vsz,rss,tty,stat,start,time,comm,cmd|grep $username | grep -iE $processName | grep -v grep", $pids);
  if (count($pids) > 0) {
    $exists = true;
  }
  return $exists;
}

$sonarr = processExists("nzbdrone",$username);

if ($sonarr == "1") { $sval = "<span class=\"badge badge-service-running-dot\"></span><span class=\"badge badge-service-running-pulse\"></span>";
} else { $sval = "<span class=\"badge badge-service-disabled-dot\"></span><span class=\"badge badge-service-disabled-pulse\"></span>";
}

echo "$sval";

?>