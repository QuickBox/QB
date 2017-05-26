#!/bin/bash
#
# [Quick Box :: Install Radarr package]
#
# GITHUB REPOS
# GitHub _ packages  :   https://github.com/QuickBox/QB
# LOCAL REPOS
# Local _ packages   :   /etc/QuickBox/packages
# Author             :   PastaGringo | KarmaPoliceT2
# URL                :   https://quickbox.io
#
# QuickBox Copyright (C) 2017 QuickBox.io
# Licensed under GNU General Public License v3.0 GPL-3 (in short)
#
#   You may copy, distribute and modify the software as long as you track
#   changes/dates in source files. Any modifications to our software
#   including (via compiler) GPL-licensed code must also be made available
#   under the GPL along with build & install instructions.
#################################################################################
function _string() { perl -le 'print map {(a..z,A..Z,0..9)[rand 62] } 0..pop' 15 ; }
#################################################################################
#
# Specify the package name and the relevant port for use with the template files and sed commands.
package="radarr"
port="7878"
# Define the startup command for this app.
startcommand="/usr/bin/mono /opt/${package^}/${package^}.exe -nobrowser"
#
function _installRadarrIntro() {
  echo "Radarr will now be installed." >>"${OUTTO}" 2>&1;
  echo "This process may take up to 2 minutes." >>"${OUTTO}" 2>&1;
  echo "Please wait until install is completed." >>"${OUTTO}" 2>&1;
  # output to box
  echo "Radarr will now be installed."
  echo "This process may take up to 2 minutes."
  echo "Please wait until install is completed."
  echo
  sleep 5
}

function _installRadarrDepdendencies() {
  # output to box
  echo "Installing dependencies ... "
	apt update >/dev/null 2>&1
	# incase of an error exprienced where the apt-get install -y libmono-cil-dev curl mediainfo command failed. Cause unknown.
	dpkg --configure -a >/dev/null 2>&1
	apt-get install -y libmono-cil-dev curl mediainfo >/dev/null 2>&1
}

function _installRadarrCode() {
  # output to box
  echo "Installing Radarr ... "
  cd /opt
  wget $( curl -s https://api.github.com/repos/Radarr/Radarr/releases | grep linux.tar.gz | grep browser_download_url | head -1 | cut -d \" -f 4 ) > /dev/null 2>&1
  tar -xvzf Radarr.develop.*.linux.tar.gz >/dev/null 2>&1
  rm -rf /opt/Radarr.develop.*.linux.tar.gz
  touch /install/.radarr.lock
}

function _installRadarrConfigure() {
  # output to box
  echo "Configuring Radarr ... "
  #
  # ${package^} makes the variable start with a Capital or ${package} matches the case of the variable defined above in package=""
  #
  mkdir -p /home/${username}/.config/${package^}
  chown -R ${username}:${username} /home/${username}/.config
  chmod 775 /home/${username}/.config
  chown -R ${username}:${username} /opt/${package^}/
  #
  cp ${local_setup}configs/${package^}/config.xml /home/${username}/.config/${package^}/config.xml
  chown ${username}:${username} /home/${username}/.config/${package^}/config.xml
  #
  # Create the Apache2 configuration from a template file.
  cp ${local_setup}configs/Apache2/package.conf /etc/apache2/sites-enabled/$package.conf
  # Edit the configuration file.
  sed -i 's|PACKAGE|'"${package}"'|g' /etc/apache2/sites-enabled/$package.conf
  sed -i 's|PORT|'"${port}"'|g' /etc/apache2/sites-enabled/$package.conf
  sed -i 's|USERNAME|'"${username}"'|g' /etc/apache2/sites-enabled/$package.conf
  chown www-data:www-data /etc/apache2/sites-enabled/${package}.conf
  service apache2 reload
  #
  # Create service using template file. 
  cp ${local_setup}configs/systemd/package.service /etc/systemd/system/${package}.service
  # Edit the file.
  sed -i 's|PACKAGE|'"${package^}"'|g' /etc/systemd/system/${package}.service
  sed -i 's|USERNAME|'"${username}"'|g' /etc/systemd/system/${package}.service
  sed -i 's|COMMAND|'"${startcommand}"'|g' /etc/systemd/system/${package}.service
  #
  # Start the service
  systemctl daemon-reload
  systemctl enable ${package}.service > /dev/null 2>&1
  #
}

function _installRadarrStart() {
  # output to box
  echo "Starting Radarr ... "
  systemctl start radarr.service
}

function _installRadarrFinish() {
  # output to dashboard
  echo "Radarr Install Complete!" >>"${OUTTO}" 2>&1;
  echo "You can access it at  : http://$ip/radarr" >>"${OUTTO}" 2>&1;
  echo >>"${OUTTO}" 2>&1;
  echo >>"${OUTTO}" 2>&1;
  echo "Close this dialog box to refresh your browser" >>"${OUTTO}" 2>&1;
  # output to box
  echo "Radarr Install Complete!"
  echo "You can access it at  : http://$ip/radarr"
  echo
  echo "Close this dialog box to refresh your browser"
}

function _installRadarrExit() {
	exit 0
}

OUTTO=/srv/rutorrent/home/db/output.log
local_setup=/etc/QuickBox/setup/
local_packages=/etc/QuickBox/packages/
username=$(cat /srv/rutorrent/home/db/master.txt)
distribution=$(lsb_release -is)
ip=$(curl -s http://whatismyip.akamai.com)

_installRadarrIntro
echo "Installing dependencies ... " >>"${OUTTO}" 2>&1;_installRadarrDependencies
echo "Installing Radarr ... " >>"${OUTTO}" 2>&1;_installRadarrCode
echo "Configuring Radarr ... " >>"${OUTTO}" 2>&1;_installRadarrConfigure
echo "Starting Radarr ... " >>"${OUTTO}" 2>&1;_installRadarrStart
_installRadarrFinish
_installRadarrExit
