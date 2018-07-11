#!/bin/bash
# Update installed apps to new reverse proxy configurations

function _couchpotato() {
  if [[ ! -f /etc/apache2/sites-enabled/couchpotato.conf ]]; then
    service couchpotato@${MASTER} stop
    sed -i "/host = .*/d" /home/"${MASTER}"/.couchpotato/settings.conf
    sed -i "s/url_base.*/url_base = couchpotato\nhost = localhost/g" /home/"${MASTER}"/.couchpotato/settings.conf

    cat > /etc/apache2/sites-enabled/couchpotato.conf <<EOF
<Location /couchpotato>
ProxyPass http://localhost:5050/couchpotato
ProxyPassReverse http://localhost:5050/couchpotato
AuthType Digest
AuthName "rutorrent"
AuthUserFile '/etc/htpasswd'
Require user ${MASTER}
</Location>
EOF
    chown www-data: /etc/apache2/sites-enabled/couchpotato.conf
    service apache2 reload
  fi
}

function _emby() {
  if [[ ! -f /etc/apache2/sites-enabled/emby.conf ]]; then
    service emby-server stop

    cat > /etc/apache2/sites-enabled/emby.conf <<EOF
<Location /emby>
Satisfy Any
order deny,allow
deny from all
allow from all
ProxyPass http://localhost:8096/emby
ProxyPassReverse http://localhost:8096/emby
</Location>

<Location /embyws>
Satisfy Any
order deny,allow
deny from all
allow from all
ProxyPass ws://localhost:8096/embyws
ProxyPassReverse ws://localhost:8096/embyws
</Location>
EOF
    chown www-data: /etc/apache2/sites-enabled/emby.conf
    service apache2 reload
  fi
}

function _jackett() {
  if [[ ! -f /etc/apache2/sites-enabled/jackett.conf ]]; then
    systemctl stop jackett@${MASTER}
    sed -i "s/\"AllowExternal.*/\"AllowExternal\": false,/g" /home/"${MASTER}"/.config/Jackett/ServerConfig.json
    sed -i "s/\"BasePathOverride.*/\"BasePathOverride\": \"\/jackett\"/g" /home/"${MASTER}"/.config/Jackett/ServerConfig.json

cat > /etc/apache2/sites-enabled/jackett.conf <<EOF
<Location /jackett>
ProxyPass http://localhost:9117/jackett
ProxyPassReverse http://localhost:9117/jackett
AuthType Digest
AuthName "rutorrent"
AuthUserFile '/etc/htpasswd'
Require user ${MASTER}
</Location>
EOF
    chown www-data: /etc/apache2/sites-enabled/jackett.conf
    service apache2 reload
    service jackett@${MASTER} start
  fi
}

function _Tautulli() {
  if [[ ! -f /etc/apache2/sites-enabled/Tautulli.conf ]]; then
  service Tautulli stop
  sed -i "s/http_root.*/http_root = \"Tautulli\"/g" /opt/Tautulli/config.ini
  sed -i "s/http_host.*/http_host = localhost/g" /opt/Tautulli/config.ini

  cat > /etc/apache2/sites-enabled/Tautulli.conf <<EOF
  <Location /Tautulli>
  ProxyPass http://localhost:8181/tautulli
  ProxyPassReverse http://localhost:8181/autulli
  AuthType Digest
  AuthName "rutorrent"
  AuthUserFile '/etc/htpasswd'
  Require user ${MASTER}
  </Location>
EOF
  chown www-data: /etc/apache2/sites-enabled/Tautulli.conf
  service apache2 reload
  service Tautulli restart
  fi
}

function _ombi() {
  if [[ ! -f /etc/apache2/sites-enabled/ombi.conf ]]; then
  cat > /etc/apache2/sites-enabled/ombi.conf <<EOF
<Location /ombi>
ProxyPass http://localhost:3000/ombi
ProxyPassReverse http://localhost:3000/ombi
Require all granted
</Location>
EOF
  chown www-data: /etc/apache2/sites-enabled/ombi.conf
  service apache2 reload
  fi
}

function _sabnzbd() {
  if [[ ! -f /etc/apache2/sites-enabled/sickrage.conf ]]; then
    systemctl stop sabnzbd@${MASTER}
    cat >/etc/systemd/system/sabnzbd@.service<<EOF
[Unit]
Description=sabnzbd
After=network.target

[Service]
Type=forking
KillMode=process
User=%I
ExecStart=/usr/bin/screen -f -a -d -m -S sabnzbd python SABnzbd/SABnzbd.py --browser 0 --server 127.0.0.1:65080 --https 65443
ExecStop=/bin/kill -HUP $MAINPID
WorkingDirectory=/home/%I/

[Install]
WantedBy=multi-user.target

EOF

  cat > /etc/apache2/sites-enabled/sabnzbd.conf <<EOF
<Location /sabnzbd>
ProxyPass http://localhost:65080/sabnzbd
ProxyPassReverse http://localhost:65080/sabnzbd
AuthType Digest
AuthName "rutorrent"
AuthUserFile '/etc/htpasswd'
Require user ${MASTER}
</Location>
EOF
    chown www-data: /etc/apache2/sites-enabled/sabnzbd.conf
    service apache2 reload
    systemctl daemon-reload
    systemctl start sabnzbd@${MASTER}
  fi
}

function _sickrage() {
  if [[ ! -f /etc/apache2/sites-enabled/sickrage.conf ]]; then
    service sickrage@${MASTER} stop
    sed -i "s/web_root.*/web_root = \"sickrage\"/g" /home/"${MASTER}"/.sickrage/config.ini
    sed -i "s/web_host.*/web_host = localhost/g" /home/"${MASTER}"/.sickrage/config.ini
    cat > /etc/apache2/sites-enabled/sickrage.conf <<EOF
<Location /sickrage>
  ProxyPass http://localhost:8081/sickrage
  ProxyPassReverse http://localhost:8081/sickrage
  AuthType Digest
  AuthName "rutorrent"
  AuthUserFile '/etc/htpasswd'
  Require user ${MASTER}
</Location>
EOF
    chown www-data: /etc/apache2/sites-enabled/sickrage.conf
    service apache2 reload
    service sickrage@${MASTER} start
  fi
}

function _medusa() {
  if [[ ! -f /etc/apache2/sites-enabled/medusa.conf ]]; then
    # The config below is according to Medusa Wiki:
    # https://github.com/pymedusa/Medusa/wiki/FAQ%27s-and-Fixes#reverse-proxy-is-not-working
    LAN_IP_ADDR=$(ip route get 8.8.8.8 | awk 'NR==1 {print $NF}')
    service medusa@${MASTER} stop
    sed -i "s/web_root.*/web_root = \"medusa\"/g" /home/"${MASTER}"/.medusa/config.ini
    sed -i "s/web_host.*/web_host = 0.0.0.0/g" /home/"${MASTER}"/.medusa/config.ini
    sed -i "s/localhost_ip.*/localhost_ip = ${LAN_IP_ADDR}/g" /home/"${MASTER}"/.medusa/config.ini
    sed -i "s/web_port.*/web_port = 8082/g" /home/"${MASTER}"/.medusa/config.ini
    sed -i "s/ssl_verify.*/ssl_verify = 0/g" /home/"${MASTER}"/.medusa/config.ini
    sed -i "s/enable_https.*/enable_https = 1/g" /home/"${MASTER}"/.medusa/config.ini
    sed -i "s/https_cert.*/https_cert = ssl-cert-snakeoil.pem/g" /home/"${MASTER}"/.medusa/config.ini
    sed -i "s/https_key.*/https_key = ssl-cert-snakeoil.key/g" /home/"${MASTER}"/.medusa/config.ini
    sed -i "s/handle_reverse_proxy.*handle_reverse_proxy = 1/g" /home/"${MASTER}"/.medusa/config.ini

    cat > /etc/apache2/sites-enabled/medusa.conf <<EOF
LoadModule proxy_module /usr/lib/apache2/modules/mod_proxy.so
LoadModule proxy_http_module /usr/lib/apache2/modules/mod_proxy_http.so
LoadModule proxy_wstunnel_module /usr/lib/apache2/modules/mod_proxy_wstunnel.so
LoadModule ssl_module modules/mod_ssl.so
SSLProxyEngine on
SSLProxyVerify none
SSLProxyCheckPeerCN off
SSLProxyCheckPeerName off
SSLProxyCheckPeerExpire off
LogLevel debug
ProxyRequests Off
<Proxy *>
        AddDefaultCharset Off
        Order deny,allow
        Allow from all
        AuthType Digest
        AuthName "rutorrent"
        AuthUserFile '/etc/htpasswd'
        Require user ${MASTER}
</Proxy>
ProxyPass /medusa/ws wss://127.0.0.1:8082/medusa/ws keepalive=On timeout=600 retry=1 acquire=3000
ProxyPassReverse /medusa/ws wss://127.0.0.1:8082/medusa/ws

ProxyPass /medusa https://127.0.0.1:8082/medusa keepalive=On timeout=600 retry=1 acquire=3000
ProxyPassReverse /medusa https://127.0.0.1:8082/medusa
ProxyPassReverseCookieDomain 127.0.0.1 %{HTTP:Host}
EOF

    chown www-data: /etc/apache2/sites-enabled/medusa.conf
    service apache2 reload
    service medusa@${MASTER} start
  fi
}

function _sonarr() {
  if [[ ! -f /etc/apache2/sites-enabled/sonarr.conf ]]; then
    systemctl stop sonarr@${MASTER}
    sed -i "s/<UrlBase>.*/<UrlBase>sonarr<\/UrlBase>/g" /home/${MASTER}/.config/NzbDrone/config.xml
    sed -i "s/<BindAddress>.*/<BindAddress>127.0.0.1<\/BindAddress>/g" /home/${MASTER}/.config/NzbDrone/config.xml
    cat > /etc/apache2/sites-enabled/sonarr.conf <<EOF
<Location /sonarr>
ProxyPass http://localhost:8989/sonarr
ProxyPassReverse http://localhost:8989/sonarr
AuthType Digest
AuthName "rutorrent"
AuthUserFile '/etc/htpasswd'
Require user ${MASTER}
</Location>
EOF
    chown www-data: /etc/apache2/sites-enabled/sonarr.conf
    service apache2 reload
    systemctl start sonarr@${MASTER}
  fi
}

function _subsonic() {
  baseurl=$(cat /usr/share/subsonic/subsonic.sh | grep -i -m1 context | cut -d= -f 2)
  if [[ $baseurl == \/ ]]; then rm -f /etc/apache2/sites-enabled/subsonic.conf; fi
  if [[ ! -f /etc/apache2/sites-enabled/subsonic.conf ]]; then
    cp -f ${local_setup}templates/subsonic/subsonic.sh.template /usr/share/subsonic/subsonic.sh
    cp -f ${local_setup}templates/sysd/subsonic.template /etc/systemd/system/subsonic.service
    sed -i "s/MASTER/${MASTER}/g" /etc/systemd/system/subsonic.service
    mkdir /srv/subsonic
    chown ${MASTER}: /srv/subsonic
    systemctl enable subsonic.service >/dev/null 2>&1
    systemctl start subsonic.service >/dev/null 2>&1

    cat > /etc/apache2/sites-enabled/subsonic.conf <<EOF
<Location /subsonic>
ProxyPass http://localhost:4040/subsonic
ProxyPassReverse http://localhost:4040/subsonic
AuthType Digest
AuthName "rutorrent"
AuthUserFile '/etc/htpasswd'
Require user ${MASTER}
</Location>
EOF
    chown www-data: /etc/apache2/sites-enabled/subsonic.conf
    service apache2 reload
    service subsonic restart
  fi
}

function _syncthing() {
  if [[ ! -f /etc/apache2/sites-enabled/syncthing.conf ]]; then
    systemctl stop syncthing@${MASTER}
    sed -i 's/<address>0.0.0.0/<address>127.0.0.1/g' /home/${MASTER}/.config/syncthing/config.xml
    sed -i 's/tls="true"/tls="false"/g' /home/${MASTER}/.config/syncthing/config.xml

    cat > /etc/apache2/sites-enabled/syncthing.conf <<EOF
<Location /syncthing>
ProxyPass http://localhost:8384
ProxyPassReverse http://localhost:8384
AuthType Digest
AuthName "rutorrent"
AuthUserFile '/etc/htpasswd'
Require user ${MASTER}
</Location>
EOF
    chown www-data: /etc/apache2/sites-enabled/syncthing.conf
    service apache2 reload

    systemctl start syncthing@${MASTER}
  fi
}

local_setup=/etc/QuickBox/setup/
MASTER=$(cat /srv/rutorrent/home/db/master.txt)

if [[ -f /install/.couchpotato.lock ]]; then _couchpotato; fi
if [[ -f /install/.emby.lock ]]; then _emby; fi
if [[ -f /install/.jackett.lock ]]; then _jackett; fi
if [[ -f /install/.Tautulli.lock ]]; then _Tautulli; fi
#if [[ -f /install/.ombi.lock ]]; then _ombi; fi
if [[ -f /install/.sabnzbd.lock ]]; then _sabnzbd; fi
if [[ -f /install/.sickrage.lock ]]; then _sickrage; fi
if [[ -f /install/.medusa.lock ]]; then _medusa; fi
if [[ -f /install/.sonarr.lock ]]; then _sonarr; fi
if [[ -f /install/.subsonic.lock ]]; then _subsonic; fi
if [[ -f /install/.syncthing.lock ]]; then _syncthing; fi
