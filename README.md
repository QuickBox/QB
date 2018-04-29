[![QuickBox](https://cdn.quickbox.io/file/2018/04/qb_logo_original.svg "QuickBox")](https://quickbox.io)

## To make use of the GitHub repo (all future updates posted here!)
>This step is only required for users who are coming off of our self-hosted GitLab repositories. If you are installing from fresh, please see [How to install](#how-to-install)

### [1]:
login to your server via ssh. Gain root access with `sudo su` followed up with `cd` to place you in your /root directory. Once in your /root directory, type the following to remove your current local QuickBox repositories.
```
rm -rf ~/QuickBox
```

---

### If you are already located on the GitHub repo as of v2.4.9, run the following to easily update to the new v2.5.+ QuickBox/QB repo:
```
rm -rf /etc/QuickBox; \
git clone https://github.com/amefs/QB /etc/QuickBox; \
wget -O /usr/local/bin/quickbox/system/updateQuickBox https://raw.githubusercontent.com/amefs/QB/master/packages/system/updateQuickBox; \
dos2unix $(find /usr/local/bin/quickbox -type f) && chmod +x $(find /usr/local/bin/quickbox -type f); \
updateQuickBox
```

### If you have received the following error:
>`find: ‘/home/*/.config/deluge’: No such file or directory`
This simply means you do not have Deluge installed, there is no further action for you take.

---

> #### NOTE:<br>The continuation below explains the above all-in-one code, minus the removal of the recent locations at `/root/QuickBox`

### [2]:
grab the current/future QuickBox repository with the following:
```
git clone https://github.com/amefs/QB /etc/QuickBox
```

>If you receive an error `/etc/QuickBox already exists` please perform an `rm -rf /etc/QuickBox`. You can then run the `git clone` command above to proceed.

### [3]:
update your current update function by doing the following:
```
local_packages=/etc/QuickBox/packages/
rm -rf /usr/local/bin/quickbox
cp -r ${local_packages}/. /usr/local/bin/quickbox
dos2unix $(find /usr/local/bin/quickbox -type f)
chmod +x $(find /usr/local/bin/quickbox -type f)
cp -f /usr/local/bin/quickbox/system/reload /usr/bin/reload
```

### [4]:
run the upgrade script to match any recent changes and to get you on your way:
```
updateQuickBox
```


---

## Script status

[![Version 2.5.4-production](https://img.shields.io/badge/version-2.5.4-674172.svg?style=flat-square)](https://quickbox.io/readme-md/) [![GNU v3.0 License](https://img.shields.io/badge/license-GNU%20v3.0%20License-blue.svg?style=flat-square)](https://quickbox.io/readme-md/)

#### Ubuntu Builds
 [![Ubuntu 15.10 Passing](https://img.shields.io/badge/Ubuntu%2015.10-passing-brightgreen.svg?style=flat-square)](https://quickbox.io/readme-md/) [![Ubuntu 16.04 Passing](https://img.shields.io/badge/Ubuntu%2016.04-passing-brightgreen.svg?style=flat-square)](https://quickbox.io/readme-md/) [![Ubuntu 16.10 Failing](https://img.shields.io/badge/Ubuntu%2016.10-failing-brightred.svg?style=flat-square)](https://quickbox.io/readme-md/)

#### Debian Builds
[![Debian 8 Null](https://img.shields.io/badge/Debain%208-passing-brightgreen.svg?style=flat-square)](https://quickbox.io/readme-md/)

---

### Quick Advisory Notice(s) on QuickBox

>### :rotating_light: OVH DEFAULT KERNEL NOTICE!<br>
grsec is built into OVH's custom kernel and it absolutely wrecks havoc when using these panels where we depend on the ability for one user (www-data) to see the processes of another running user ($username).
<br>This can be seen clearly by using a task manager such as h/top.
<br>With grsec enabled you can only see the processes owned by your user unless you run htop as root. As such, it is highly recommended to use the stock kernel for your distribution or at the very least installing an OVH kernel that is not compiled with grsec
<br>If you are using So You Start (SYS) as a host, you should opt to use the distribution kernel. You will see this as a check box option when installing your server. Otherwise, QuickBox will handle this for you on install.



---
> Please understand that we do not want to push this project as a means to supply a commercially used product, ie; seedbox provider - so keep this in mind - only if you wish to sale slots on your server. Though it is very multi-user friendly... it is also not free of it's faults due to it's high amount of capabilities and being publicly available. This is a community based project that is a measure of goodwill to be given to such an outspoken and freethinking community.

### A Preview of Quick Box
![QuickBox Preview - Smoked Theme](https://github.com/QuickBox/quickbox_github_assets/blob/master/img/quickbox_preview-theme_smoked.gif "QuickBox Preview - Smoked Theme")

---

This script has the following features

* A multi-user environment, complete with scripts to add and delete users.
* Linux Quota, to control how much space every user can use in the box. This can be controlled via the '__setdisk__' command.
* Customized Seedbox Dashboard located at https://SERVER_IP/
* HTTPs Downloads directory (https://SERVER_IP/${username}.downloads)
* Obscures ports for ssh and ftp. __SSH = 4747__ | __FTP = 5757__ (note, this is not for security reasons... it's simply a means to reduce bad bot hits from all over the web)
* Creates a limited shell access environment. This gives your additional created users the ability to interact with their seedbox via ssh on port 4747 w/o having access to other users shells and/or root/sudo commands and functions.

## Installed software
* IRSSI
* Linux Quota
* LShell - (LimitedShell for additional users to ssh)
* mktorrent
* pureftp - vsftp (CuteFTP multi-segmented download friendly)
* __ruTorrent 3.7__ + official plugins
* __rTorrent 0.9.6 + libTorrrent 0.13.6__
* SSH Server (for SSH terminal and sFTP connections)
* HTTPS - Web Console

## Optional software
* BTSync
* CouchPotato
* ConfigServer Firewall
* __Deluge (Web-client and thin-client)__
* Emby
* Jackett
* NextCloud
* OpenVPN
* Plex
* PlexPy
* Plex Requests (.NET)
* pyLoad
* qBittorrent
* Quassel
* Quotas
* Radarr
* Rapidleech
* RTorrent (ruTorrent)
* SABnzbd
* SickGear
* SickRage
* Medusa
* Sonarr
* Subsonic
* Syncthing
* Transmission (single-user)
* X2Go - Remote Desktop
* ZNC
* .. more to come (or how about that feature request?)

## Main ruTorrent plugins
autotools, cpuload, quotaspace, erasedata, extratio, extsearch, feeds, filedrop, filemanager, geoip, history, logoff, mediainfo, mediastream, ratiocolor, rss, scheduler, screenshots, theme, trafic and unpack

## Additional ruTorrent plugins
* Autodl-IRSSI (with an updated list of trackers)
* A modified version of Diskpace to support quota (by Notos)
* Filemanager (modified to handle rar, zip, unzip, tar and bzip)
* Fileshare Plugin (http://forums.rutorrent.org/index.php?topic=705.0)
* Logoff
* Theme: QuickBox ``Dark rutorrent skin``
* Colorful Ratios: Customized to match QuickBox Theme
* __rutorrentMobile__: Mobile version of ruTorrent - seriously - toss TransDroid and the pain that it is... this is a new essential plugin (IMO)

## Before installation
You need to have a Fresh "blank" server installation.
After that access your box using a SSH client, like PuTTY.

---

## How to install

#### You must be logged in as root to run this installation.


---

### Ubuntu 15.10, 16.04 & ~~16.10~~ || Debian 8

**Run the following command to grab our latest stable release ...**
```
apt-get -yqq update; apt-get -yqq upgrade; apt-get -yqq install git lsb-release; \
git clone https://github.com/amefs/QB /etc/QuickBox; \
bash /etc/QuickBox/setup/quickbox-setup
```


## Want to run in development mode?(not support in this mod version)

**Run the following command to grab current development repos ...**
```
mkdir /install/ && touch /install/.developer.lock; \
apt-get -yqq update; apt-get -yqq upgrade; apt-get -yqq install git lsb-release; \
git clone --branch "development" https://github.com/QuickBox/QB /etc/QuickBox; \
bash /etc/QuickBox/setup/quickbox-setup
```

## Already have QuickBox installed and want to switch over to development?

**EASY! Run the following command to grab current development repos ...**
```
mkdir /install/ && touch /install/.developer.lock \
sudo box upgrade
```


---


## Commands
After installing you will have access to the following commands to be used directly in terminal (as root)

* __changeUserpass__ - change users SSH/FTP/deluge/ruTorrent password
* __clean_mem__ - flushes servers physical memory cache (helps avoid swap overflow)
* __createSeedboxUser__ - creates a shelled seedbox user
* __deleteSeedboxUser__ - deletes a created seedbox user and their directories
<sup>**This is permanent, current data will be deleted - you can create them again at any time**</sup>
* __reload__ - restarts your seedbox services, i.e; rtorrent & irssi
* __removepackage-cron__ - upgrades your system to make use of systemd
* __setdisk__ - set your disk quota for any given user
* __showspace__ - shows amount of space used by each user
* __upgradeBTSync__ - upgrades btsync when new version is available
* __upgradeJackett__ - upgrades Jackett when new version is available
* __upgradeOmbi__ - upgrades Ombi when new version is available
* __upgradePlex__ - upgrades Plex when new version is available
* __upgradepyLoad__ - upgrades pyLoad when new version is available
* __box install letsencrypt__ used to install letsencrypt
* More commands detailed here: [QuickBox Commands](https://quickbox.io/wiki/quickbox-commands/)

If your disk space widget is not showing the correct amount of space, run one of the following commands based on the mount you use:
* If you're using a /home mounted partition then run: __fix-disk_widget_home__
* If you're using a /(root) mounted partition then run: __fix-disk_widget_root__
<br/>
