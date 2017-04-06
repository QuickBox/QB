
##CHANGELOG v2.4.8:
_Changelog updated Monday, October 24, 2016_

---

### General adjustments & additions
* Version Bump set to **2.4.8**
* Emby - much requested Plex alternative and now officially supported!
* Custom Menu - thou shalt not overwrite my custom links
* Custom URL Override - using your own custom proxy configs for your favorite application? No problem, we got you covered now!
* Numerous fixes have been introduced
 + QuickBox now proxies around 90% of it's supported applications for sexxy urls.
 + Added fix to some package installers to ensure proper install
 + There were some typos :blush:
* Several enhancements to make your QuickBox experience every bit of stellar (see below)

### Commit Histories by Submodule v2.4.8
---

#### QuickBox/quickbox_dashboard version 2.4.8

* Compare changes from v2.4.7 to v2.4.8 <sup>[COMPARE +25 commits](https://github.com/QuickBox/quickbox_dashboard/compare/65a33b195b43ea50138cb498bd4aaa069c6e9f1f...67b3a76eaf609dde7cdc87638898bec4ac7bfc17)</sup>


#### QuickBox/quickbox-packages version 2.4.8

* Compare changes from v2.4.7 to v2.4.8 <sup>[COMPARE +52 commits](https://github.com/QuickBox/quickbox_packages/compare/c6028fa228e862e75f9db9c705dd97227e689b27...2a1f8f11b7952c921c993ff02f945c654e21b715)</sup>

#### QuickBox/quickbox-rutorrent version 2.4.8

> Note: RUTorrent changes are sourced from their original GitHub repo provided by it's maintainer
https://github.com/Novik/ruTorrent

* Compare changes from v2.4.7 to v2.4.8 <sup>[COMPARE +0 commits]</sup>


#### QuickBox/quickbox_rutorrent-plugins version 2.4.8

* Compare changes from v2.4.7 to v2.4.8 <sup>[COMPARE +1 commits](https://github.com/QuickBox/quickbox_rutorrent-plugins/compare/43f6d5d28c8adfd63c214db7d06a058af41b3dc7...7c6b49b872886a26267a47dbb9d6e487ac6e5f76)</sup>


#### QuickBox/quickbox-setup version 2.4.8

* Compare changes from v2.4.7 to v2.4.8 <sup>[COMPARE +23 commits](https://github.com/QuickBox/quickbox_setup/compare/8926132144f103a7ba54e41045c64ea7cc850ed0...7237652e33f33a2c47629116db0fb083d6b195d2)</sup>

---

##CHANGELOG v2.4.7:
_Changelog updated Sunday, August 28, 2016_

---

### General adjustments & additions
* Version Bump set to **2.4.7**
* SABnzbd - much requested and now officially supported!
* pyLoad - it has been lurking in the shadows and waiting for the include, well... here it is!
* Subsonic - music fans rejoice, Subsonic is now supported in the QuickBox Eco-System
* Numerous fixes have been introduced
 + Default OVH Kernel's are now automagically updated on QuickBox install (option presents)
 + Deluge package installer hiccups have been burped
 + There were some typos :blush:
 + PlexRequests.NET resets are no more (mono dependencies addressed)
 + `upgradePlex` will now update bot Plex Free and PlexPass users
* Several enhancements to make your QuickBox experience every bit of stellar (see below)

### Commit Histories by Submodule v2.4.7
---

#### QuickBox/quickbox_dashboard version 2.4.7

* Added translations for pyLoad, SABnzbd & Subsonic + Minor changes (*lang_fr*) [96eb4862](https://github.com/QuickBox/quickbox_dashboard/commit/96eb4862c30c3b57055c6f94de9107772014ca3f)
* core config for service toggles and processing updated [7031cc4a](https://github.com/QuickBox/quickbox_dashboard/commit/7031cc4a38c3b67bb9c7809c764d126656fd199f)
* create dashboard toggles and service status for new applications [71a0b2a9](https://github.com/QuickBox/quickbox_dashboard/commit/71a0b2a96a38fabea2ba7508dafc86e3327069b1)
* include new packages within the data array [f2dee90e](https://github.com/QuickBox/quickbox_dashboard/commit/f2dee90e594ff83e9d4870babc9b8118ed823d22)
* add modal calls to new packages and applications to panel.scripts [45980fd6](https://github.com/QuickBox/quickbox_dashboard/commit/45980fd63aed300acfa81106956a0b1feacbbc7c)
* [ux] reorganize and add new menu items [6aa0ba45](https://github.com/QuickBox/quickbox_dashboard/commit/6aa0ba45b9ea52528f25a006a4f7facdacf7d6ca)
 + <sup>pyLoad, SABnzbd and Subsonic are not included in the menu</sup>
* reorganize ajax for cpu and bandwidth charts (tidyness) [91ad9d6c](https://github.com/QuickBox/quickbox_dashboard/commit/91ad9d6c3291c1a18b506d95975296d63d719e7a)
* [fix] we now utilize `box upgrade` to update from the dashboard as well [2107321c](https://github.com/QuickBox/quickbox_dashboard/commit/2107321cb8cfd42f92f386bf7d700c3c202dc6f5)
* language files updated with new package variables [2b7f43f7](https://github.com/QuickBox/quickbox_dashboard/commit/2b7f43f7601897f8be8928ded6f160b68069e767)
* sabnzbd brand icon added [d73949d6](https://github.com/QuickBox/quickbox_dashboard/commit/d73949d675d2c594f49c0fc401c9956b30960fa7)
* pyload brand icon added [25a8acaa](https://github.com/QuickBox/quickbox_dashboard/commit/25a8acaae6d787e1a693af0e6dc2cef54db3838a)
* removing useless files in preparation for v2.4.7 update [b8de5709](https://github.com/QuickBox/quickbox_dashboard/commit/b8de5709d780018b4c0f9b75b113bdd546039aa7)
* merged updated lang_de from @cinory [5b8b9ded](https://github.com/QuickBox/quickbox_dashboard/commit/5b8b9ded72d394074f77c1530aaff07b9b007f32)
* adding subsonic brand icon ;) [7543add8](https://github.com/QuickBox/quickbox_dashboard/commit/7543add8a1ba8fecbe10668f0da9616bacf39bdf)
* [fix] merged minor corrections on lang_fr from @tomcdj71 [c96fd892](https://github.com/QuickBox/quickbox_dashboard/commit/c96fd8924ceec75d7c5a78c3aaf4423e08b741ae)
* merged updated lang_fr from @tomcdj71 [aa9005b1](https://github.com/QuickBox/quickbox_dashboard/commit/aa9005b1ca3047e7453eeb8848214d6091613ffe)
* Update lang_dk with new strings :) [1e26b672](https://github.com/QuickBox/quickbox_dashboard/commit/1e26b672416cd95e199db39b9baca4c7d791d125)
* language files updated with new values for translation [17a366f7](https://github.com/QuickBox/quickbox_dashboard/commit/17a366f73230ac18725a5b41c17874b365749b6c)
* additional language strings added on alert modals/popups [5e81e904](https://github.com/QuickBox/quickbox_dashboard/commit/5e81e904a8fcc05eb2ac536a12d1ae8324ae9e8b)


#### QuickBox/quickbox-packages version 2.4.7

* copy local repo bash profile to system and source on update [149288eb](https://github.com/QuickBox/quickbox_packages/commit/149288eb2cae54de85d0fbb6d1caa08b0b60f771)
* setup subsonic uninstaller package [4374c223](https://github.com/QuickBox/quickbox_packages/commit/4374c223ce3048d188bdb33251f0ec7eeeec8915)
* setup sabnzbd uninstaller package [82297ea4](https://github.com/QuickBox/quickbox_packages/commit/82297ea45e8ff60151f821303b6604bde3f0e09b)
* create pyload package uninstaller [5ebabd4e](https://github.com/QuickBox/quickbox_packages/commit/5ebabd4edcd7b13f07b2861b345979a91913291c)
* setup subsonic install package [bcc1ffb3](https://github.com/QuickBox/quickbox_packages/commit/bcc1ffb3ef64fe18a44b9d6782db885048cfda65)
* Sabnzbd scripts [7e11fe52](https://github.com/QuickBox/quickbox_packages/commit/7e11fe5286f9481e39e2e975d503353495c01d4f)
 + <sup>Special thanks to @kc1 for supplying these</sup>
* create pyload package installer [a7d3bcab](https://github.com/QuickBox/quickbox_packages/commit/a7d3bcab8cbfb2d1c0618b9fd415684b2e45e370)
* [fix] addressed issues with deluge package installer [dd97f5f0](https://github.com/QuickBox/quickbox_packages/commit/dd97f5f07ea5331905b2422ae6b7eade7800ab52)
* updated $master user file location & repo location [d79ab410](https://github.com/QuickBox/quickbox_packages/commit/d79ab4106afe56f22c69bd3cc5da0072dcf25072)
* [fix] use github.com in upgrade script [529789b9](https://github.com/QuickBox/quickbox_packages/commit/529789b97e1a6a2979ee68ff95189a5b895fdb7d)
 + <sup>thanks for pointing that out @eclipse</sup>
* [fix] typo in word "decimals". [254688fb](https://github.com/QuickBox/quickbox_packages/commit/254688fb07ab958cb746089c22d5b4118d2cca6c)
* [fix] address mono depends for plexrequests.net [9999a769](https://github.com/QuickBox/quickbox_packages/commit/9999a769c02627776acfe41ca31c415a2e80da33)
* Minor packages readme update [e8aa71cc](https://github.com/QuickBox/quickbox_packages/commit/e8aa71cca7af92583ceef3a5e3d0c4d5b8de7fb6)


#### QuickBox/quickbox-rutorrent version 2.4.7

> Note: RUTorrent changes are sourced from their original GitHub repo provided by it's maintainer
https://github.com/Novik/ruTorrent

* minor typo [f3f0785d](https://github.com/QuickBox/quickbox_rutorrent/commit/f3f0785db5a0954bf5ac132f86bc6742c619456c)


#### QuickBox/quickbox_rutorrent-plugins version 2.4.7

* update `_task` plugin to match Novik fixes [5ff0c6e2](https://github.com/QuickBox/quickbox_rutorrent-plugins/commit/5ff0c6e2da15e7c391a1f07c914ede688bd77ec7)
* remove duplicate out-dated theme [53c7b585](https://github.com/QuickBox/quickbox_rutorrent-plugins/commit/53c7b5850813fdd3ecb5237ad161e56ffcf5eb76)
 + <sup>QuickBox-Dark is the parent to club-Swizards, which is the current and developed/maintained theme</sup>


#### QuickBox/quickbox-setup version 2.4.7

* added pyload, sabnzbd and subsonic to sudoers allowed list [4e4519b1](https://github.com/QuickBox/quickbox_setup/commit/4e4519b1d664589f22c544f1acdc4dec890ec3fb)
* setup subsonic sysd template [cd149da4](https://github.com/QuickBox/quickbox_setup/commit/cd149da45807f967b4a06785d6efb44f9693315c)
* use subsonic.sh template and import to installer [3dc27726](https://github.com/QuickBox/quickbox_setup/commit/3dc27726b825e05bff717c94ddb12efcb55c79f5)
* [enhancement] add bash profile template as some providers may not have them implemented on OS build [bfbc1eff](https://github.com/QuickBox/quickbox_setup/commit/bfbc1eff67ab2cb2c78a2d2005dd268dd890cf98)
* created pyload functions for setup and upgrade [c7964d43](https://github.com/QuickBox/quickbox_setup/commit/c7964d431586b9b89c98933cc97831ad50a059d9)
* improved the `fixhome` alias function [d8bb1e84](https://github.com/QuickBox/quickbox_setup/commit/d8bb1e84d472a04260f6453e14e1a6e5b62b9925)
 + <sup>`fixhome` now wants to know specific user of interest</sup>
* [fix] `prompt_OFF` was not resetting to default system prompt [c958f6bb](https://github.com/QuickBox/quickbox_setup/commit/c958f6bb57c12c6c0a91e9c1ddb200097e2a5e56)
* [enhancement] better aesthetic on QuickBox bash greeting [49e05238](https://github.com/QuickBox/quickbox_setup/commit/49e0523864bced042b8b1ea67bb8b485f93d7e64)
* removed deprecated .startup functions [6b150850](https://github.com/QuickBox/quickbox_setup/commit/6b1508500056f794e22200e76f62c0d5ae2580e4)
* enable proxy_fci module and php7.0-fpm conf on initial install [7a580fa3](https://github.com/QuickBox/quickbox_setup/commit/7a580fa3b240baf21c7a98b10ad275fd7b274ac8)
* [enhancement] setup custom bash profile for sourcing on install [dc665f9e](https://github.com/QuickBox/quickbox_setup/commit/dc665f9e3ea1140c109d0e7b4301397c68cbe163)
* [enhancement] compile x265 libraries with ffmpeg on initial install [89614f97](https://github.com/QuickBox/quickbox_setup/commit/89614f970b4b89739556bea530defb797fb30009)
* [fix] remove club-Swizards github clone - clone from github.com [1f591d45](https://github.com/QuickBox/quickbox_setup/commit/1f591d451d7869fa1da0544607d73e3d6f8ecb41)
* Converted to latest .rtorrent.rc syntax. Added "network.tos.set = throughput". [7b4dd4bb](https://github.com/QuickBox/quickbox_setup/commit/7b4dd4bb8d45161fdc345530ff2d2f99e22aa940)
* added utf-8 enconding to rtorrent.rc template [d08a280b](https://github.com/QuickBox/quickbox_setup/commit/d08a280b62d32747e6083149b8a9a4edcaa7c744)
* added utf-8 encoding to vsftpd config [a051465b](https://github.com/QuickBox/quickbox_setup/commit/a051465babef845dd8813d295b309d2cbe422ac5)
* Small fixes to checkkernel + run function [677826f8](https://github.com/QuickBox/quickbox_setup/commit/677826f8279959d8622838cf22529f6bac0b6c34)
* Update grsec function for correctness [4b29077b](https://github.com/QuickBox/quickbox_setup/commit/4b29077bd03581525815f73c7583996a2130929a)
* tidy up unused functions and lines [f9481562](https://github.com/QuickBox/quickbox_setup/commit/f94815621c3ca27ebed1827d8018156f38e491ad)
* [enhancement] smarter `upgradePlex` command - now updates PlexPass users [8e234bf8](https://github.com/QuickBox/quickbox_setup/commit/8e234bf84dbc25db7d93fc79fdd1767bf6a764b7)



---

##CHANGELOG v2.4.6:
_Changelog updated Saturday, August 21, 2016_

---

### General adjustments & additions
* Version Bump set to **2.4.6**

### Commit Histories by Submodule v2.4.6
---

#### QuickBox/quickbox_dashboard version 2.4.6

* [fix] deluge & rutorrent panel links [633446c2](https://github.com/QuickBox/quickbox_dashboard/commit/633446c2c7508d5946380f39bc4a11150c1eae3a)
* [enhancement] nextcloud is here! [13c75b70](https://github.com/QuickBox/quickbox_dashboard/commit/13c75b7084b783bc3b9e0c67c3df00127a905b47)
* [fix] set a minimum of '0' on bandwidth chart to avoid negative values [66e4f0aa](https://github.com/QuickBox/quickbox_dashboard/commit/66e4f0aac1455455d9427636e53ce34fa73aa10d)
* danish language file updated with new values [346d1f81](https://github.com/QuickBox/quickbox_dashboard/commit/346d1f8120a4e7d1ee9f823efe9bb78509ae5e16)
* [ux] a more lean and accurate cpu widget [c6d50168](https://github.com/QuickBox/quickbox_dashboard/commit/c6d50168e38bd04e22fccb7b5b0338e02679c423)
* update on language files [152c268b](https://github.com/QuickBox/quickbox_dashboard/commit/152c268b012ab464f6eb7d9edad205a98c06be80)
* [fix] removed characters causing breakage on language files [66264062](https://github.com/QuickBox/quickbox_dashboard/commit/66264062123399345ef68d3dfba20118fb0b4600)


#### QuickBox/quickbox-packages version 2.4.6

* Further refine IFACE detection X2 [f607391b](https://github.com/QuickBox/quickbox_packages/commit/f607391b92cf0f3239b4a1d87c64887dd81080ef) & [6e42136f](https://github.com/QuickBox/quickbox_packages/commit/6e42136fc2dbcffe569683dce62287140fd5c9bd)
* [security] Remove php execution for existing users on update [1d74f999](https://github.com/QuickBox/quickbox_packages/commit/1d74f99974231955cac6b00d2715fa2e151737e0)
* [fix] Spinner causes breakage on interactive scripts [888be134](https://github.com/QuickBox/quickbox_packages/commit/888be1349f989c6e9af03da10700f11ca2c3a914)
* cleanup on system installer packages [a0f12fc1](https://github.com/QuickBox/quickbox_packages/commit/a0f12fc15b2b0a7d21adb2da2a1a5660857996e8)
* [enhancement] nextcloud is here! [7c93e13c](https://github.com/QuickBox/quickbox_packages/commit/7c93e13c6e8890301846782c3d8d43b9a2d8c12f)
* [fix] update the csf archive download link to their new release location [e9074ce6](https://github.com/QuickBox/quickbox_packages/commit/e9074ce6700b500ed0c1440c1d822a678ae09be8)
* Upgrade box to newest version [520b1893](https://github.com/QuickBox/quickbox_packages/commit/520b1893465a621b4d2cc4699e62490d24daa443)
* remove old .startup remnants on sonarr package [35e44e5d](https://github.com/QuickBox/quickbox_packages/commit/35e44e5db4a4b4260f3198b4b7ba7e3de9ff8cb2)
* restart apache after openvpn installation [a498feca](https://github.com/QuickBox/quickbox_packages/commit/a498feca5e26ef74a3072c25a1bece145f1435a1)
* remove organize user home directory function [c84b2186](https://github.com/QuickBox/quickbox_packages/commit/c84b2186777c9906defdf934bb9bb2ba7a3c18a1)
* [security] require a valid user to access vpn configs [a421c585](https://github.com/QuickBox/quickbox_packages/commit/a421c585b7e5bda1c3f7a55c18c3d62f070cee57)
* [enhancement] vpn management now included - type: `quickVPN` [c5dbf75a](https://github.com/QuickBox/quickbox_packages/commit/c5dbf75a1ffc9eed4878240eecc7189b7dcfdc5d)


#### QuickBox/quickbox-rutorrent version 2.4.6

* updated gitignore - overlook conf and share directories [69aa9c50](https://github.com/QuickBox/quickbox_rutorrent/commit/69aa9c5079fc7bfe04ed4a195ded682ceed0c0d9)
* update rutorrent to latest novik/master [14d8951a](https://github.com/QuickBox/quickbox_rutorrent/commit/14d8951ac669fe92fbdfb221e39d6fe24f66d22b)


#### QuickBox/quickbox_rutorrent-plugins version 2.4.6

* No Changes


#### QuickBox/quickbox-setup version 2.4.6

* Further refine IFACE detection [96553245](https://github.com/QuickBox/quickbox_setup/commit/96553245c90e089b7f4720db9aa170556cf36f39)
* minor typo fix [d634ec68](https://github.com/QuickBox/quickbox_setup/commit/d634ec682d13f8d49912af39c7cb4079a1ff1065)
* Version bump deluge libtorrent to 1.1.1 [5a16e32b](https://github.com/QuickBox/quickbox_setup/commit/5a16e32bd5fdacb24d2c70ea1a43b74b5315e565)
* [enhancement] Expand upgradeDeluge to allow upgrading libtorrent and deluge through recompiling [325018ec](https://github.com/QuickBox/quickbox_setup/commit/325018ec12075e67ad483531a21a985306e49fd6)
* [security] Disallow php execution from rtorrent/deluge apache servers X2 [3892b6d7](https://github.com/QuickBox/quickbox_setup/commit/3892b6d7ed6284f63dff450a468623dca12ceba6) & [6d3df8d8](https://github.com/QuickBox/quickbox_setup/commit/6d3df8d84e6edc598243841f1631670f9bd03eb2)
* Remove deprecated webconsle code from changeUserpass [3df0ef7c](https://github.com/QuickBox/quickbox_setup/commit/3df0ef7cbb562ff8773ba6d9d4e47b0c4726911f)
* Make /install dir if 10g=yes [9bf4f74e](https://github.com/QuickBox/quickbox_setup/commit/9bf4f74e095961939ac922594a289ab04a05d1ad)
* Apache doesn't need sudo pkill or sudo killall anymore [d88ea63b](https://github.com/QuickBox/quickbox_setup/commit/d88ea63b2c8666c17b185e99a4971034c3d35481)
* Better `fixhome` command [b085d804](https://github.com/QuickBox/quickbox_setup/commit/b085d804cbcbd73fd803e33ee2717de960abea46)
* Setup now checks if a grsec kernel is present and fails if the user declines to upgrade [2b27087c](https://github.com/QuickBox/quickbox_setup/commit/2b27087c99ac7136972c4ac46e381fdaebe52d5a)



---

## CHANGELOG v2.4.5:
_Changelog updated Saturday, August 21, 2016_

---


### General adjustments & additions
* Version Bump set to **2.4.5**

### Commit Histories by Submodule v2.4.5
---

#### QuickBox/quickbox_dashboard version 2.4.5

* updated language files to reflect new batch installer command `box install <app>` [4f7565b0](https://github.com/QuickBox/quickbox_dashboard/commit/4f7565b0d5735d278d49d0cb12dff629d8611c66)
 <sup>+ BOX is QuickBoxs new batch installer for easy installation of applications simultaneously. Type `box install <app>` to install an application from cli or type: `box install <app> <app>` to install multiple applications at once.</sup>
* minor adjustments... _file: lang_fr_ [a399dbc7](https://github.com/QuickBox/quickbox_dashboard/commit/a399dbc79d4fede615bae6659700dc5ef74447c7)
* Translation added for 'select language' + minor changes. [4113237b](https://github.com/QuickBox/quickbox_dashboard/commit/4113237b692cdfc3516fa2222b74308fd0fd199d)
* we only need to log errors from the dashboard [4e649551](https://github.com/QuickBox/quickbox_dashboard/commit/4e649551aa5319cb0fbe091eb84a42e7daf7f9ed)
* added additional value to language files - 'Select Language' [472ae6fd](https://github.com/QuickBox/quickbox_dashboard/commit/472ae6fdafaa6de12a0a1c1617bc6508a634df07)
* address minor whitespacing [e9cb6383](https://github.com/QuickBox/quickbox_dashboard/commit/e9cb6383f3829d1dcec060b6877577474ef3dd31)
* **[fix]** addressed missing opening php operator [2e5ac300](https://github.com/QuickBox/quickbox_dashboard/commit/2e5ac300fff0d6e825edfd55059bd2934e93fe84)
* Add translations for CLI and OpenVPN package + quickVPN command [cc295426](https://github.com/QuickBox/quickbox_dashboard/commit/cc2954260b05aa8bc65aa506517f5bd54287c942)
* better openvpn info and integration within dashboard [95c04757](https://github.com/QuickBox/quickbox_dashboard/commit/95c04757113839a57b3ba7e7f15a5a98f8ff02a7)
* **[ux]** minor dashboard aesthetics [9375d909](https://github.com/QuickBox/quickbox_dashboard/commit/9375d9090e1aae22e24bcdc436099bba0cdafb71)
* add openvpn config file download to 'Downloads' Menu if installed per user [9c0baa3a](https://github.com/QuickBox/quickbox_dashboard/commit/9c0baa3a6b6b257c3bafd8ec81f6d3d122a1df99)
* **[enhancement]** better approximation of bandwidth chart values [1992369c](https://github.com/QuickBox/quickbox_dashboard/commit/1992369caafeb1c36a629fdb83cd5cec424c39a4)
* **[fix]** addressed issue with disk percentage calculations on certain environments [56163af6](https://github.com/QuickBox/quickbox_dashboard/commit/56163af66b45c1bf1096560bbcdbcf48134b4e41)
 <sup>+ special thanks to @sadpanda for granting access to a machine to troubleshoot and find a fix for this</sup>


#### QuickBox/quickbox-packages version 2.4.5

* New method for determining IFACE, more OpenVZ-friendly [d539c3b5](https://github.com/QuickBox/quickbox_packages/commit/d539c3b5092d3c19972e7a57fff1fe7c453595ba)
* Remove outdated and refactored QBPM [d0c171dd](https://github.com/QuickBox/quickbox_packages/commit/d0c171dd0afbe0d6c164d37cd24bad5a04068c71)
* Introducing box -- QBPM just got better! [d4e0a24b](https://github.com/QuickBox/quickbox_packages/commit/d4e0a24badd14cada54a1ffe70e0e45dc9a78631)
* updated installer packages for cleaner and quieter execution [6e29dd38](https://github.com/QuickBox/quickbox_packages/commit/6e29dd38f02403e6efa3db86c89a4b7bf6e1a58f)

##### What is `box`?
Building off of our CLI based updater, there have been a few tweaks to make installing and/or removing multiple applications at once a total breeze. Additionally, the new QBPM (QuickBox Package Management) codenamed: BOX now supports updating/upgrading your QuickBox. It's truly an all-in-one management utility for those of us who prefer rolling in ssh.
If you prefer to still use the GUI aspect of the installer you can call box without any arguments. box -h will show you a help screen.
**BOX CHEATSHEET**
```
# box -h
Usage: box [ install | remove | list | update ] pkg

Note: Only install and remove functions accept package arguments. List
   and update must be run without modifiers



install        The install function requires at least one package
               name as an argument but will accept multiples

               Example: box install quassel syncthing znc

remove         The remove function requires at least one package
               name as an argument but will accept multiples

               Example: box remove quassel syncthing znc

update         The update command will update your local QuickBox
               repository to the latest version

list           The list command will list packages available to be
               managed by box

```


#### QuickBox/quickbox-rutorrent version 2.4.5

* No Changes


#### QuickBox/quickbox_rutorrent-plugins version 2.4.5

* .gitignore added - ignore plugins conf.php [88355fa9](https://github.com/QuickBox/quickbox_rutorrent-plugins/commit/88355fa9ebba7e6adea571e555f80c2455fc83ab)
* update rutorrent plugins to latest novik/master (fix utf8 encodings) [c3c0c5c4](https://github.com/QuickBox/quickbox_rutorrent-plugins/commit/c3c0c5c4d9566538c6498a9d2cbba097ee70f936)


#### QuickBox/quickbox-setup version 2.4.5

* added newer archived source of plex [2c561819](https://github.com/QuickBox/quickbox_setup/commit/2c5618197c4a8bc48ae972ee20ba877190380309)
* **[enhancement]** new aliases for added for easy disk widget fixes/updates [cbcd144f](https://github.com/QuickBox/quickbox_setup/commit/cbcd144fdffb072f780ec65970fa7d038df47782)
 <sup>+ `fix-disk_widget_home` - easily copy over updated/default disk widget from local repository to your dashboard, home mounted installs only</sup>
* **[fix]** 10g function defaults addressed [042d1a6a](https://github.com/QuickBox/quickbox_setup/commit/042d1a6a75ca75b02cc6f67dab72016abdc537e9)
* quickVPN added to sudoers allowed aliases [70f3c926](https://github.com/QuickBox/quickbox_setup/commit/70f3c9262ffa27d5ddbeee38c805f8a8dbd5dc82)
* Remove old cron function from running [23ed699a](https://github.com/QuickBox/quickbox_setup/commit/23ed699a74d6d629d58f4f847c25fef272cef2cc)
* New method for determining IFACE, more OpenVZ-friendly [a1aee6ca](https://github.com/QuickBox/quickbox_setup/commit/a1aee6ca0c698a028b57216381d39773b577592a)
* **[enhancement]** ffmpeg on script install now includes x265 libraries on build [5ea7358f](https://github.com/QuickBox/quickbox_setup/commit/5ea7358f20722015a1163b2494f8a555cfe71536)
* 10G inclusion and deluge password changing improved [9ec8b7ae](https://github.com/QuickBox/quickbox_setup/commit/9ec8b7ae118ab4323fd24b6c259385cd3bbf1109)
* **[enhancement]** 10G function added to setup script [886d3ff5](https://github.com/QuickBox/quickbox_setup/commit/886d3ff5380a0d802b53b325b3850c32c3638688)
* allow `install_ffmpeg` to be fired as system command [79a62e8a](https://github.com/QuickBox/quickbox_setup/commit/79a62e8a540dcad21efbc930179312ab25357d21)
