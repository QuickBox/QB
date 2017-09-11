<?php

$language = array(
        'lang_de',
        'lang_dk',
        'lang_en',
        'lang_fr'
);

foreach ($language as $lang) {
if (isset($_GET['langSelect-'.$lang.''])) {
        header('Location: /');
        shell_exec("sudo /usr/local/bin/quickbox/system/lang/langSelect-$lang");
}}

?>
