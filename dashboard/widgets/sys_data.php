<?php
if (isset($_GET['clean_mem'])) {
        header('Location: //');
        shell_exec("sudo /usr/local/bin/quickbox/system/clean_mem");
}

if (isset($_GET['clean_log'])) {
        header('Location: /');
        shell_exec("sudo /usr/local/bin/quickbox/system/clean_log");
}

if (isset($_GET['updateQuickBox'])) {
        header('Location: //');
        shell_exec("sudo /usr/local/bin/quickbox/system/box upgrade");
}

?>
