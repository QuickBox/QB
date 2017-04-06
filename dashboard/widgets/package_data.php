<?php

$packages = array(
        'btsync',
        'csf',
        'couchpotato',
        'deluge',
        'emby',
        'jackett',
        'nextcloud',
        'nzbhydra',
        'plex',
        'plexpy',
        'ombi',
        'pyload',
        'rapidleech',
        'sabnzbd',
        'sickrage',
        'subsonic',
        'sonarr',
        'syncthing',
        'x2go',
        'quota',
        'quassel',
        'znc'
);

foreach ($packages as $ipackage) {
if (isset($_GET['installpackage-'.$ipackage.''])) {
        header('Location: //');
        shell_exec("sudo /usr/local/bin/quickbox/package/install/installpackage-$ipackage");
}}

foreach ($packages as $rpackage) {
if (isset($_GET['removepackage-'.$rpackage.''])) {
        header('Location: /');
        shell_exec("sudo /usr/local/bin/quickbox/package/remove/removepackage-$rpackage");
}}

?>
