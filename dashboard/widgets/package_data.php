<?php

$packages = array(
        'btsync',
        'csf',
        'couchpotato',
        'deluge',
        'emby',
        'headphones',
        'jackett',
		'lidarr',
        'medusa',
        'nextcloud',
        'nzbget',
        'nzbhydra',
        'ombi',
        'plex',
        'Tautulli',
        'pyload',
        'quassel',
        'quota',
        'radarr',
        'rapidleech',
        'sabnzbd',
        'sickgear',
        'sickrage',
        'sonarr',
        'subsonic',
        'syncthing',
        'transmission',
        'x2go',
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
