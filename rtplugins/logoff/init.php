<?php
eval(getPluginConf("logoff"));

$me = getUser();
$users = array();

$dirs = scandir($rootPath . "/share/users/");
if ($dirs && count($dirs) > 0)
    foreach ($dirs as $dir)
        if ($dir[0] != "." && $dir != $me)
            $users[] = $dir;

$allowed = 0;
foreach (explode(",", $allowSwitch) as $as)
    if (trim($as) == $me)
        $allowed = 1;

$jResult .= "plugin.logoffURL = '" . $logoffURL . "';";
$jResult .= "plugin.me = '" . $me . "';";
$jResult .= "plugin.users = " . json_encode($users) . ";";
$jResult .= "plugin.allowSwitch = " . $allowed . ";";
$jResult .= "plugin.abortMs = " . $abortMs . ";";

$theSettings->registerPlugin("logoff");
?>
