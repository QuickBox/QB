<?php
  $current_vs = "<span id=\"version-result\"></span>";
?>
<body class="body">
<header>
  <div class="headerpanel">
    <div class="logopanel">
      <h2><?php include("db/branding-l.php"); ?></h2>
    </div><!-- logopanel -->
    <div class="headerbar">
      <a id="menuToggle" class="menutoggle"><i class="fa fa-bars"></i></a>
      <div class="header-right">
        <ul class="headermenu">
          <?php if (file_exists('/install/.developer.lock')) { ?>
          <li>
            <div class="btn-group">
              <button type="button" class="btn btn-logged">
                <a href="#" class="label label-warning" style="">You are on the QuickBox Development Repo</a>
              </button>
            </div>
          </li>
          <?php } ?>
          <?php if ($username == "$master") { ?>
          <li>
            <div id="noticePanel" class="btn-group">
              <button class="btn" data-toggle="dropdown">
                <i class="fa fa-menu"></i> QuickBox Menu <span class="caret"></span>
              </button>
              <div id="noticeDropdown" class="dropdown-menu dm-notice pull-right">
                <div role="tabpanel">
                  <!-- Nav tabs -->
                  <ul class="nav nav-tabs nav-justified" role="tablist">
                    <li class="active"><a data-target="#quickplus" data-toggle="tab">QuickBox+</a></li>
                    <!--li><a data-target="#chat" data-toggle="tab">Chat</a></li-->
                    <li><a data-target="#dashadjust" data-toggle="tab">Dashboard</a></li>
                  </ul>

                  <!-- Tab panes -->
                  <div class="tab-content">
                    <div role="tabpanel" class="tab-pane active" id="quickplus">
                      <ul class="list-group">
                        <li class="list-group-item">
                          <div class="row">
                            <div class="col-xs-12">
                              <h5>QuickBox :: <span style="color: #fff;text-shadow: 0px 0px 6px #fff;"><?php echo "$version"; ?></span></h5>
                              <small><a href="https://quickbox.io/readme-md/" target="_blank">README.md</a></small>
                              <small><a href="https://github.com/QuickBox/QB/compare/v2.5.3...<?php echo $version; ?>" target="_blank">CHANGELOG</a></small>
                            </div>
                          </div>
                        </li>

                        <li class="list-group-item">
                          <div class="row">
                            <div class="col-xs-12">
                              <h5>QuickBox.IO</h5>
                            </div>
                            <div class="col-xs-12">
                              <div class="col-xs-12 col-md-6" style="padding: 0">
                                <ul style="padding-left: 5px">
                                <li><small><a href="https://plaza.quickbox.io/categories" target="_blank" alt="View all QuickBox Community Categories">Plaza</a></small></li>
                                <li><small><a href="https://quickbox.io/kb/faq" target="_blank" alt="QuickBox FAQs">FAQs</a></small></li>
                                </ul>
                              </div>
                              <div class="col-xs-12 col-md-6" style="padding: 0">
                                <ul style="padding-left: 5px">
                                <li><small><a href="https://quickbox.io/kb/faq" target="_blank" alt="QuickBox Wiki - How-to's, Tips and Tricks">Wiki</a></small></li>
                                <li><small><a href="https://plaza.quickbox.io/c/quickbox-support" target="_blank"><?php echo T('ISSUE_REPORT_TXT'); ?></a></small></li>
                                </ul>
                              </div>
                            </div>
                          </div>
                        </li>
                      </ul>
                      <!--a class="btn-more" href="">View More QuickBox <i class="fa fa-long-arrow-right"></i></a-->
                    </div><!-- tab-pane -->

                    <!--div role="tabpanel" class="tab-pane" id="chat">
                      <ul class="list-group notice-list">
                        <li class="list-group-item" style="padding-top: 10px;">
                          <div class="row">
                            <div class="col-xs-2">
                              <i class="fa fa-comment"></i>
                            </div>
                            <div class="col-xs-10">
                              <h5><?php echo T('JOIN_US_TXT'); ?></h5>
                              <small style="color:#fff"><strong style="color: #4CD4B0">host:</strong> chat.quickbox.io</small>
                              <small style="color:#fff"><strong style="color: #4CD4B0">chan:</strong> #QB-Support</small>
                              <small style="margin-top:15px;"><a href="https://chat.quickbox.io/channel/QB-Support" class="label label-success" target="_blank"><?php echo T('CHAT_CONNECT_TXT'); ?></a></small>
                            </div>
                          </div>
                        </li>
                      </ul>
                    </div><!-- tab-pane -->

                    <div role="tabpanel" class="tab-pane" id="dashadjust">
                      <ul class="list-group">
                        <li class="list-group-item">
                          <div class="row">
                            <div class="col-xs-12">
                              <div class="col-xs-12 col-md-6" style="padding: 0">
                                <?php $language = array();
                                $language[] = array('file' => 'lang_zh', 'title' =>'Chinese');             
                                $language[] = array('file' => 'lang_dk', 'title' =>'Danish');
                                $language[] = array('file' => 'lang_en', 'title' =>'English');
                                $language[] = array('file' => 'lang_fr', 'title' =>'French');
                                $language[] = array('file' => 'lang_de', 'title' =>'German');
                                $language[] = array('file' => 'lang_es', 'title' =>'Spanish'); { ?>
                                <h5><?php echo T('LANG_SELECT'); ?></h5>
                                <?php foreach($language as $lang) { ?>
                                  <small><a href='?langSelect-<?php echo $lang['file'] ?>=true'><img class='lang-flag' src='lang/flag_<?php echo $lang['file'] ?>.png' /><?php echo $lang['title'] ?></a></small>
                                <?php } ?>
                                <?php } ?>
                              </div>
                              <div class="col-xs-12 col-md-6" style="padding: 0">
                              <?php $option = array();
                              $option[] = array('file' => 'defaulted', 'title' =>'Defaulted');
                              $option[] = array('file' => 'smoked', 'title' =>'Smoked'); { ?>
                                <h5><?php echo T('THEME_SELECT'); ?></h5>
                                <?php foreach($option as $theme) { ?>
                                  <small><a href="javascript:void()" data-toggle="modal" data-target="#themeSelect<?php echo $theme['file'] ?>Confirm"><img class='lang-flag' src='img/themes/opt_<?php echo $theme['file'] ?>.png' /><?php echo $theme['title'] ?></a></small>
                                <?php } ?>
                              <?php } ?>
                              </div>
                            </div>
                          </div>
                        </li>
                      </ul>

                    </div>
                  </div>
                </div>
              </div>
            </div>
          </li>
          <?php } ?>
          <li>
            <div class="btn-group">
              <button type="button" class="btn btn-logged" data-toggle="dropdown">
                <?php echo "$username"; ?>
                <span class="caret"></span>
              </button>
              <?php include("db/branding-m.php"); ?>
            </div>
          </li>
        </ul>
      </div><!-- header-right -->
    </div><!-- headerbar -->
  </div><!-- header-->
</header>
<section>
  <div class="leftpanel ps-container">
    <div class="leftpanelinner">
      <ul class="nav nav-tabs nav-justified nav-sidebar">
        <li class="tooltips active" data-toggle="tooltip" title="<?php echo T('MAIN_MENU'); ?>" data-placement="bottom"><a data-toggle="tab" data-target="#mainmenu"><i class="tooltips fa fa-ellipsis-h"></i></a></li>
        <?php if ($username == "$master"){ ?>
          <li class="tooltips" data-toggle="tooltip" title="<?php echo T('RPLUGIN_MENU'); ?>" data-placement="bottom"><a data-toggle="tab" data-target="#plugins"><i class="tooltips fa fa-puzzle-piece"></i></a></li>
        <?php } ?>
        <li class="tooltips" data-toggle="tooltip" title="<?php echo T('HELP_COMMANDS'); ?>" data-placement="bottom"><a data-toggle="tab" data-target="#help"><i class="tooltips fa fa-question-circle"></i></a></li>
      </ul>
      <div class="tab-content">
        <!-- ################# MAIN MENU ################### -->
        <div class="tab-pane active" id="mainmenu">
          <h5 class="sidebar-title"><?php echo T('MAIN_MENU'); ?></h5>
          <ul class="nav nav-pills nav-stacked nav-quirk">
            <!--li class="active"><a href="index.php"><i class="fa fa-home"></i> <span>Dashboard</span></a></li-->
            <!-- // RUTORRENT // -->
            <?php if (file_exists('/home/'.$username.'/.sessions/rtorrent.lock')) { ?>
              <li><a class="grayscale" href="/rutorrent" target="_blank"><img src="img/brands/rtorrent.png" class="brand-ico"> <span>ruTorrent</span></a></li>
            <?php } ?>
            <!-- // DELUGE-WEB // -->
            <?php if (processExists("deluge-web",$username) && file_exists('/install/.deluge.lock')) { ?>
              <li><a class="grayscale" href="<?php echo "$dwURL"; ?>" target="_blank"><img src="img/brands/deluge.png" class="brand-ico"> <span>Deluge Web</span></a></li>
            <?php } ?>
            <!-- // TRANSMISSION // -->
            <?php if (processExists("transmission-daemon",$username) && file_exists('/install/.transmission.lock')) { ?>
              <li><a href="<?php echo "$transmissionURL"; ?>" class="grayscale" target="_blank"><img src="img/brands/transmission.png" class="brand-ico"> <span>Transmission Web Control</span></a></li>
            <?php } ?>
			<!-- // QBITTORRENT // -->
            <?php if (processExists("qbittorrent-nox",$username) && file_exists('/install/.qbittorrent.lock')) { ?>
              <li><a href="<?php echo "$qbittorrentURL"; ?>" class="grayscale" target="_blank"><img src="img/brands/qbittorrent.png" class="brand-ico"> <span>qBittorrent</span></a></li>
            <?php } ?>
            <?php if ($username == "$master") { ?>
              <?php if (processExists("resilio-sync",rslsync) && file_exists('/install/.btsync.lock')) { ?>
                <li><a class="grayscale" href="<?php echo "$btsyncURL"; ?>" target="_blank"><img src="img/brands/btsync.png" class="brand-ico"> <span>BTSync</span></a></li>
              <?php } ?>
              <?php if (file_exists('/install/.couchpotato.lock')) { ?>
                <li><a class="grayscale" href="<?php echo "$cpURL"; ?>" target="_blank"><img src="img/brands/couchpotato.png" class="brand-ico"> <span>CouchPotato</span></a></li>
              <?php } ?>
              <?php if (file_exists('/install/.csf.lock')) { ?>
                <li><a class="grayscale" href="<?php echo "$csfURL"; ?>" target="_blank"><img src="img/brands/csf.png" class="brand-ico"> <span>CSF (firewall)</span></a></li>
              <?php } ?>
              <?php if (file_exists('/install/.emby.lock')) { ?>
                <li><a class="grayscale" href="<?php echo "$embyURL"; ?>" target="_blank"><img src="img/brands/emby.png" class="brand-ico"> <span>Emby</span></a></li>
              <?php } ?>
              <?php if (file_exists('/install/.headphones.lock')) { ?>
                <li><a class="grayscale" href="<?php echo "$headphonesURL"; ?>" target="_blank"><img src="img/brands/headphones.png" class="brand-ico"> <span>Headphones</span></a></li>
              <?php } ?>
              <?php if (file_exists('/install/.jackett.lock')) { ?>
                <li><a class="grayscale" href="<?php echo "$jackettURL"; ?>" target="_blank"><img src="img/brands/jackett.png" class="brand-ico"> <span>Jackett</span></a></li>
              <?php } ?>
              <?php if (file_exists('/install/.lidarr.lock')) { ?>
                <li><a class="grayscale" href="<?php echo "$lidarrURL"; ?>" target="_blank"><img src="img/brands/lidarr.png" class="brand-ico"> <span>Lidarr</span></a></li>
              <?php } ?>
			  <?php if (file_exists('/install/.medusa.lock')) { ?>
                <li><a class="grayscale" href="<?php echo "$medusaURL"; ?>" target="_blank"><img src="img/brands/medusa.png" class="brand-ico"> <span>Medusa</span></a></li>
              <?php } ?>
              <?php if (file_exists('/install/.nextcloud.lock')) { ?>
                <li><a class="grayscale" href="<?php echo "$nextcloudURL"; ?>" target="_blank"><img src="img/brands/nextcloud.png" class="brand-ico"> <span>NextCloud</span></a></li>
              <?php } ?>
              <?php if (file_exists('/install/.nzbget.lock')) { ?>
                <li><a class="grayscale" href="<?php echo "$nzbgetURL"; ?>" target="_blank"><img src="img/brands/nzbget.png" class="brand-ico"> <span>NZBGet</span></a></li>
              <?php } ?>
              <?php if (file_exists('/install/.nzbhydra.lock')) { ?>
                <li><a class="grayscale" href="<?php echo "$nzbhydraURL"; ?>" target="_blank"><img src="img/brands/nzbhydra.png" class="brand-ico"> <span>NZBHydra</span></a></li>
              <?php } ?>
              <?php if (file_exists('/install/.plex.lock')) { ?>
                <li><a class="grayscale" href="<?php echo "$plexURL"; ?>" target="_blank"><img src="img/brands/plex.png" class="brand-ico"> <span>Plex</span></a></li>
              <?php } ?>
              <?php if (file_exists("/install/.Tautulli.lock")) { ?>
                <li><a class="grayscale" href="<?php echo "$TautulliURL"; ?>" target="_blank"><img src="img/brands/Tautulli.png" class="brand-ico"> <span>Tautulli</span></a></li>
              <?php } ?>
              <?php if (file_exists("/install/.ombi.lock")) { ?>
                <li><a class="grayscale" href="<?php echo "$ombiURL"; ?>" target="_blank"><img src="img/brands/plexrequests-net.png" class="brand-ico"> <span>Ombi</span></a></li>
              <?php } ?>
              <?php if (file_exists('/install/.pyload.lock')) { ?>
                <li><a class="grayscale" href="<?php echo "$pyloadURL"; ?>" target="_blank"><img src="img/brands/pyload.png" class="brand-ico"> <span>pyLoad</span></a></li>
              <?php } ?>
              <?php if (file_exists('/install/.radarr.lock')) { ?>
                <li><a class="grayscale" href="<?php echo "$radarrURL"; ?>" target="_blank"><img src="img/brands/radarr.png" class="brand-ico"> <span>Radarr</span></a></li>
              <?php } ?>
              <?php if (file_exists('/install/.rapidleech.lock')) { ?>
                <li><a class="grayscale" href="<?php echo "$rapidleechURL"; ?>" target="_blank"><img src="img/brands/rapidleech.png" class="brand-ico"> <span>Rapidleech</span></a></li>
              <?php } ?>
              <?php if (file_exists('/install/.sabnzbd.lock')) { ?>
                <li><a class="grayscale" href="<?php echo "$sabnzbdURL"; ?>" target="_blank"><img src="img/brands/sabnzbd.png" class="brand-ico"> <span>SABnzbd</span></a></li>
              <?php } ?>
              <?php if (file_exists('/install/.sickgear.lock')) { ?>
                <li><a class="grayscale" href="<?php echo "$sickgearURL"; ?>" target="_blank"><img src="img/brands/sickgear.png" class="brand-ico"> <span>SickGear</span></a></li>
              <?php } ?>
              <?php if (file_exists('/install/.sickrage.lock')) { ?>
                <li><a class="grayscale" href="<?php echo "$sickrageURL"; ?>" target="_blank"><img src="img/brands/sickrage.png" class="brand-ico"> <span>SickRage</span></a></li>
              <?php } ?>
              <?php if (processExists("nzbdrone",$username) && file_exists('/install/.sonarr.lock')) { ?>
                <li><a class="grayscale" href="<?php echo "$sonarrURL"; ?>" target="_blank"><img src="img/brands/sonarr.png" class="brand-ico"> <span>Sonarr</span></a></li>
              <?php } ?>
              <?php if (file_exists('/install/.subsonic.lock')) { ?>
                <li><a class="grayscale" href="<?php echo "$subsonicURL"; ?>" target="_blank"><img src="img/brands/subsonic.png" class="brand-ico"> <span>Subsonic</span></a></li>
              <?php } ?>
              <?php if (file_exists('/install/.syncthing.lock')) { ?>
                <li><a class="grayscale" href="<?php echo "$syncthingURL"; ?>" target="_blank"><img src="img/brands/syncthing.png" class="brand-ico"> <span>Syncthing</span></a></li>
              <?php } ?>
              <?php if (file_exists('/install/.znc.lock')) { ?>
                <li><a class="grayscale" href="<?php echo "$zncURL"; ?>" target="_blank"><img src="img/brands/znc.png" class="brand-ico"> <span>ZNC</span></a></li>
              <?php } ?>
            <?php } ?>
            <li class="nav-parent">
              <a href=""><i class="fa fa-download"></i> <span><?php echo T('DOWNLOADS'); ?></span></a>
              <ul class="children">
                <li><a href="/<?php echo "$username"; ?>.rtorrent.downloads" target="_blank">ruTorrent</a></a></li>
                <?php if (file_exists('/install/.deluge.lock')) { ?>
                  <li><a href="/<?php echo "$username"; ?>.deluge.downloads" target="_blank">Deluge</a></li>
                <?php } ?>
                <?php if (file_exists('/install/.transmission.lock')) { ?>
                  <li><a href="/<?php echo "$username"; ?>.transmission.downloads" target="_blank">Transmission</a></li>
                <?php } ?>
				<?php if (file_exists('/install/.qbittorrent.lock')) { ?>
                  <li><a href="/<?php echo "$username"; ?>.qbittorrent.downloads" target="_blank">qBittorrent</a></li>
                <?php } ?>
                <?php if (file_exists('/home/'. $username .'/public_html/'. $username .'.zip')) { ?>
                  <li><a href="/~<?php echo "$username"; ?>/<?php echo "$username"; ?>.zip" target="_blank"> <span>OpenVPN Config</span></a></li>
                <?php } ?>
              </ul>
            </li>
            <?php if (processExists("shellinabox",shellinabox) && ($username == "$master")) { ?>
            <li><a href="/<?php echo "$username"; ?>.console" target="_blank"><i class="fa fa-keyboard-o"></i> <span><?php echo T('WEB_CONSOLE'); ?></span></a></li>
            <?php } ?>
            <!-- /// BEGIN INSERT CUSTOM MENU /// -->
            <?php include ($_SERVER['DOCUMENT_ROOT'].'/custom/custom.menu.php'); ?>
            <!-- /// END INSERT CUSTOM MENU /// -->
          </ul>
        </div><!-- tab pane -->

        <!-- ######################## HELP MENU TAB ##################### -->
        <div class="tab-pane" id="help">
          <?php if ($username == "$master") { ?>
          <h5 class="sidebar-title"><?php echo T('QUICK_SYSTEM_TIPS'); ?></h5>
          <ul class="nav nav-pills nav-stacked nav-quirk nav-mail">
            <li style="padding: 7px"><span style="font-size: 12px; color:#eee">clean_mem</span><br/>
              <small><?php echo T('CLEAN_MEM_TXT'); ?></small>
            </li>
            <li style="padding: 7px"><span style="font-size: 12px; color:#eee">disktest</span><br/>
              <small><?php echo T('DISKTEST_TXT'); ?></small>
            </li>
            <li style="padding: 7px"><span style="font-size: 12px; color:#eee">fixhome</span><br/>
              <small><?php echo T('FIXHOME_TXT'); ?></small>
            </li>
          </ul>
          <h5 class="sidebar-title"><?php echo T('SEEDBOX_COMMANDS'); ?></h5>
          <ul class="nav nav-pills nav-stacked nav-quirk nav-mail">
            <li style="padding: 7px"><span style="font-size: 12px; color:#eee">changeUserpass</span><br/>
              <small><?php echo T('CHANGEUSERPASS_TXT'); ?></small>
            </li>
            <li style="padding: 7px"><span style="font-size: 12px; color:#eee">createSeedboxUser</span><br/>
              <small><?php echo T('CREATESEEDBOXUSER_TXT'); ?></small>
            </li>
            <li style="padding: 7px"><span style="font-size: 12px; color:#eee">deleteSeedboxUser</span><br/>
              <small><?php echo T('DELETESEEDBOXUSER_TXT'); ?></small>
            </li>
            <li style="padding: 7px"><span style="font-size: 12px; color:#eee">setdisk</span><br/>
              <small><?php echo T('SETDISK_TXT'); ?></small>
            </li>
            <li style="padding: 7px"><span style="font-size: 12px; color:#eee">showspace</span><br/>
              <small><?php echo T('SHOWSPACE_TXT'); ?></small>
            </li>
            <li style="padding: 7px"><span style="font-size: 12px; color:#eee">upgradeDeluge</span><br/>
              <small><?php echo T('UPGRADEDELUGE_TXT'); ?></small>
            </li>
            <h5 class="sidebar-title"><?php echo T('PACKAGE_COMMANDS'); ?></h5>
            <li style="padding: 7px"><span style="font-size: 12px; color:#eee">quickVPN</span><br/>
              <small><?php echo T('QUICKVPN_TXT'); ?></small>
            </li>
            <li style="padding: 7px"><span style="font-size: 12px; color:#eee">setup-pyLoad</span><br/>
              <small><?php echo T('SETUPPYLOAD_TXT'); ?></small>
            </li>
            <li style="padding: 7px"><span style="font-size: 12px; color:#eee">upgradeBTSync</span><br/>
              <small><?php echo T('UPGRADEBTSYNC_TXT'); ?></small>
            </li>
            <li style="padding: 7px"><span style="font-size: 12px; color:#eee">upgradeJackett</span><br/>
              <small><?php echo T('UPGRADEJACKETT_TXT'); ?></small>
            </li>
            <li style="padding: 7px"><span style="font-size: 12px; color:#eee">upgradeOmbi</span><br/>
              <small><?php echo T('UPGRADEOMBI_TXT'); ?></small>
            </li>
            <li style="padding: 7px"><span style="font-size: 12px; color:#eee">upgradePlex</span><br/>
              <small><?php echo T('UPGRADEPLEX_TXT'); ?></small>
            </li>
            <li style="padding: 7px"><span style="font-size: 12px; color:#eee">upgradepyLoad</span><br/>
              <small><?php echo T('UPGRADEPYLOAD_TXT'); ?></small>
            </li>
            <li style="padding: 7px"><span style="font-size: 12px; color:#eee">upgradeSABnzbd</span><br/>
              <small><?php echo T('UPGRADESABNZBD_TXT'); ?></small>
            </li>
          </ul>
          <?php } ?>
          <h5 class="sidebar-title"><?php echo T('ESSENTIAL_USER_COMMANDS'); ?></h5>
          <ul class="nav nav-pills nav-stacked nav-quirk nav-mail">
            <li style="padding: 7px"><span style="font-size: 12px; color:#eee">systemctl restart rtorrent@<?php echo $username ?>.service</span><br/>
            <small><?php echo T('SCREEN_RTORRNENT_TXT'); ?></small></li>
            <li style="padding: 7px"><span style="font-size: 12px; color:#eee">systemctl restart irssi@<?php echo $username ?>.service</span><br/>
            <small><?php echo T('SCREEN_IRSSI_TXT'); ?></small></li>
          </ul>
        </div><!-- tab-pane -->

        <!-- ######################## RUTORRENT PLUGINS TAB ##################### -->
        <div class="tab-pane" id="plugins">
          <h5 class="sidebar-title"><?php echo T('PLUGIN_MENU'); ?></h5>
          <ul class="nav nav-pills nav-stacked nav-quirk">
            <li class="nav-parent nav-active">
              <a href=""><i class="fa fa-puzzle-piece"></i> <span><?php echo T('PLUGINS'); ?></span></a>
              <ul class="children">
                <li class="info-quote"><p class="info-quote"><?php echo T('PMENU_NOTICE_TXT'); ?></p></li>
                <?php foreach ($plugins as $plugin) { ?>
                  <li>
                  <?php if(file_exists('/srv/rutorrent/plugins/'.$plugin.'/plugin.info')) {
                    echo "<a href=\"javascript:void()\">$plugin</a> <div class=\"toggle-wrapper pull-right\" style=\"margin-right: -10px; margin-top: 5px;\"> <div class=\"toggle-pen toggle-modern\" onclick=\"location.href='?removeplugin-$plugin=true'\"></div></div>";
                  } else {
                    echo "<a href=\"javascript:void()\">$plugin</a> <div class=\"toggle-wrapper pull-right\" style=\"margin-right: -10px; margin-top: 5px;\"> <div class=\"toggle-pdis toggle-modern\" onclick=\"location.href='?installplugin-$plugin=true'\"></div></div>";
                  } ?>
                  </li>
                <?php } ?>
              </ul>
            </li>
          </ul>
        </div><!-- tab-pane -->

      </div><!-- tab-content -->
    </div><!-- leftpanelinner -->
  </div><!-- leftpanel -->
