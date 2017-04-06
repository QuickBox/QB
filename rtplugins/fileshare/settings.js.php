<?php
include('conf.php');

echo 'theWebUI.FS.maxlinks = "',$limits['links'], "\";\n";
echo 'theWebUI.FS.downlink = "',$downloadpath, "\";\n";
echo 'theWebUI.FS.maxdur = "',$limits['duration'], "\";\n";

?>