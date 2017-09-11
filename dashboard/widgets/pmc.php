<?php
  include ('..inc/config.php');
  include ('..inc/panel.header.php');
  include ('..inc/panel.menu.php');
?>


          <!--PACKAGE MANAGEMENT CENTER-->
          <div class="panel panel-main panel-inverse">
            <div class="panel-heading">
              <h4 class="panel-title"><?php echo T('PACKAGE_MANAGEMENT_CENTER'); ?></h4>
            </div>
            <div class="panel-body text-center" style="padding:0;">
              <div class="alert alert-danger">
                <button type="button" class="close" data-dismiss="alert" aria-hidden="true">&times;</button>
                <div align = "center"><?php echo T('PMC_NOTICE_TXT'); ?></div>
              </div>
              <div class="table-responsive ps-container">
                <table id="dataTable1" class="table table-bordered table-striped-col" style="font-size: 12px">
                  <thead>
                    <tr>
                      <th><?php echo T('NAME'); ?></th>
                      <th><?php echo T('DETAILS'); ?></th>
                      <th><?php echo T('AVAILABILITY'); ?></th>
                    </tr>
                  </thead>

                  <tbody>
                    <tr>
                      <td>BTSync</td>
                      <td><?php echo T('BTSYNC_DETAILS'); ?></td>
                      <?php if (file_exists("/install/.btsync.lock")) { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="javascript:void()" data-toggle="modal" data-target="#btsyncRemovalConfirm" class="btn btn-xs btn-success"><?php echo T('INSTALLED'); ?></a></td>
                      <?php } else { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="?installpackage-btsync=true" data-toggle="modal" data-target="#sysResponse" id="btsyncInstall" class="btn btn-xs btn-default"><?php echo T('INSTALL'); ?></a></td>
                      <?php } ?>
                    </tr>
                    <tr>
                      <td>CouchPotato</td>
                      <td><?php echo T('COUCHPOTATO'); ?></td>
                      <?php if (file_exists("/install/.couchpotato.lock")) { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="javascript:void()" data-toggle="modal" data-target="#couchpotatoRemovalConfirm" class="btn btn-xs btn-success"><?php echo T('INSTALLED'); ?></a></td>
                      <?php } else { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="?installpackage-couchpotato=true" data-toggle="modal" data-target="#sysResponse" id="couchpotatoInstall" class="btn btn-xs btn-default"><?php echo T('INSTALL'); ?></a></td>
                      <?php } ?>
                    </tr>
                    <tr>
                      <td>CSF (Firewall)</td>
                      <td><?php echo T('CSF'); ?></td>
                      <?php if (file_exists("/install/.csf.lock")) { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="javascript:void()" data-toggle="modal" data-target="#csfRemovalConfirm" class="btn btn-xs btn-success"><?php echo T('INSTALLED'); ?></a></td>
                      <?php } else { ?>
                        <td style="vertical-align: middle; text-align: center"><button data-toggle="tooltip" title="<?php echo T('BOX_TOOLTIP_CSF'); ?>" data-placement="top" class="btn btn-xs btn-danger disabled tooltips"><?php echo T('BOX'); ?></button></td>
                      <?php } ?>
                    </tr>
                    <tr>
                      <td>Deluge</td>
                      <td><?php echo T('DELUGE'); ?></td>
                      <?php if (file_exists("/install/.deluge.lock")) { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="javascript:void()" data-toggle="modal" data-target="#delugeRemovalConfirm" class="btn btn-xs btn-success"><?php echo T('INSTALLED'); ?></a></td>
                      <?php } else { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="?installpackage-deluge=true" data-toggle="modal" data-target="#sysResponse" id="delugeInstall" class="btn btn-xs btn-default"><?php echo T('INSTALL'); ?></a></td>
                      <?php } ?>
                    </tr>
                    <tr>
                      <td>Emby</td>
                      <td><?php echo T('EMBY'); ?></td>
                      <?php if (file_exists("/install/.emby.lock")) { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="javascript:void()" data-toggle="modal" data-target="#embyRemovalConfirm" class="btn btn-xs btn-success"><?php echo T('INSTALLED'); ?></a></td>
                      <?php } else { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="?installpackage-emby=true" data-toggle="modal" data-target="#sysResponse" id="embyInstall" class="btn btn-xs btn-default"><?php echo T('INSTALL'); ?></a></td>
                      <?php } ?>
                    </tr>
                    <tr>
                      <td>Headphones</td>
                      <td><?php echo T('HEADPHONES'); ?></td>
                      <?php if (file_exists("/install/.headphones.lock")) { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="javascript:void()" data-toggle="modal" data-target="#headphonesRemovalConfirm" class="btn btn-xs btn-success"><?php echo T('INSTALLED'); ?></a></td>
                      <?php } else { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="?installpackage-headphones=true" data-toggle="modal" data-target="#sysResponse" id="headphonesInstall" class="btn btn-xs btn-default"><?php echo T('INSTALL'); ?></a></td>
                      <?php } ?>
                    </tr>
                    <tr>
                      <td>Jackett</td>
                      <td><?php echo T('JACKETT'); ?></td>
                      <?php if (file_exists("/install/.jackett.lock")) { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="javascript:void()" data-toggle="modal" data-target="#jackettRemovalConfirm" class="btn btn-xs btn-success"><?php echo T('INSTALLED'); ?></a></td>
                      <?php } else { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="?installpackage-jackett=true" data-toggle="modal" data-target="#sysResponse" id="jackettInstall" class="btn btn-xs btn-default"><?php echo T('INSTALL'); ?></a></td>
                      <?php } ?>
                    </tr>
                    <tr>
                      <td>Medusa</td>
                      <td><?php echo T('MEDUSA'); ?></td>
                      <?php if (file_exists("/install/.medusa.lock")) { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="javascript:void()" data-toggle="modal" data-target="#medusaRemovalConfirm" class="btn btn-xs btn-success"><?php echo T('INSTALLED'); ?></a></td>
                      <?php } else { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="?installpackage-medusa=true" data-toggle="modal" data-target="#sysResponse" id="medusaInstall" class="btn btn-xs btn-default"><?php echo T('INSTALL'); ?></a></td>
                      <?php } ?>
                    </tr>
                    <tr>
                      <td>NextCloud</td>
                      <td><?php echo T('NEXTCLOUD'); ?></td>
                      <?php if (file_exists("/install/.nextcloud.lock")) { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="javascript:void()" data-toggle="modal" data-target="#nextcloudRemovalConfirm" class="btn btn-xs btn-success"><?php echo T('INSTALLED'); ?></a></td>
                      <?php } else { ?>
                        <td style="vertical-align: middle; text-align: center"><button data-toggle="tooltip" title="<?php echo T('BOX_TOOLTIP_NEXTCLOUD'); ?>" data-placement="top" class="btn btn-xs btn-danger disabled tooltips"><?php echo T('BOX'); ?></button></td>
                      <?php } ?>
                    </tr>
                    <tr>
                      <td>NZBGet</td>
                      <td><?php echo T('NZBGET'); ?></td>
                      <?php if (file_exists("/install/.nzbget.lock")) { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="javascript:void()" data-toggle="modal" data-target="#nzbgetRemovalConfirm" class="btn btn-xs btn-success"><?php echo T('INSTALLED'); ?></a></td>
                      <?php } else { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="?installpackage-nzbget=true" data-toggle="modal" data-target="#sysResponse" id="nzbgetInstall" class="btn btn-xs btn-default"><?php echo T('INSTALL'); ?></a></td>
                      <?php } ?>
                    </tr>
                    <tr>
                      <td>NZBHydra</td>
                      <td><?php echo T('NZBHYDRA'); ?></td>
                      <?php if (file_exists("/install/.nzbhydra.lock")) { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="javascript:void()" data-toggle="modal" data-target="#nzbhydraRemovalConfirm" class="btn btn-xs btn-success"><?php echo T('INSTALLED'); ?></a></td>
                      <?php } else { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="?installpackage-nzbhydra=true" data-toggle="modal" data-target="#sysResponse" id="nzbhydraInstall" class="btn btn-xs btn-default"><?php echo T('INSTALL'); ?></a></td>
                      <?php } ?>
                    </tr>
                    <tr>
                      <td>Ombi</td>
                      <td><?php echo T('OMBI'); ?></td>
                      <?php if (file_exists("/install/.ombi.lock")) { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="javascript:void()" data-toggle="modal" data-target="#ombiRemovalConfirm" class="btn btn-xs btn-success"><?php echo T('INSTALLED'); ?></a></td>
                      <?php } else { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="?installpackage-ombi=true" data-toggle="modal" data-target="#sysResponse" id="ombiInstall" class="btn btn-xs btn-default"><?php echo T('INSTALL'); ?></a></td>
                      <?php } ?>
                    </tr>
                    <tr>
                      <td>OpenVPN</td>
                      <td><?php echo T('OVPN'); ?></td>
                      <?php if (file_exists("/install/.vpn.lock")) { ?>
                        <td style="vertical-align: middle; text-align: center"><button data-toggle="tooltip" title="<?php echo T('OVPN_TOOLTIP_U'); ?>" data-placement="top" class="btn btn-xs btn-success disabled tooltips"><?php echo T('CLI'); ?></button></td>
                      <?php } else { ?>
                        <td style="vertical-align: middle; text-align: center"><button data-toggle="tooltip" title="<?php echo T('OVPN_TOOLTIP_I'); ?>" data-placement="top" class="btn btn-xs btn-danger disabled tooltips"><?php echo T('CLI'); ?></button></td>
                      <?php } ?>
                    </tr>
                    <tr>
                      <td>Plex</td>
                      <td><?php echo T('PLEX'); ?></td>
                      <?php if (file_exists("/install/.plex.lock")) { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="javascript:void()" data-toggle="modal" data-target="#plexRemovalConfirm" class="btn btn-xs btn-success"><?php echo T('INSTALLED'); ?></a></td>
                      <?php } else { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="?installpackage-plex=true" data-toggle="modal" data-target="#sysResponse" id="plexInstall" class="btn btn-xs btn-default"><?php echo T('INSTALL'); ?></a></td>
                      <?php } ?>
                    </tr>
                    <tr>
                      <td>PlexPy</td>
                      <td><?php echo T('PLEXPY'); ?></td>
                      <?php if (file_exists("/install/.plexpy.lock")) { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="javascript:void()" data-toggle="modal" data-target="#plexpyRemovalConfirm" class="btn btn-xs btn-success"><?php echo T('INSTALLED'); ?></a></td>
                      <?php } else { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="?installpackage-plexpy=true" data-toggle="modal" data-target="#sysResponse" id="plexpyInstall" class="btn btn-xs btn-default"><?php echo T('INSTALL'); ?></a></td>
                      <?php } ?>
                    </tr>
                    <tr>
                      <td>pyLoad</td>
                      <td><?php echo T('PYLOAD'); ?></td>
                      <?php if (file_exists("/install/.pyload.lock")) { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="javascript:void()" data-toggle="modal" data-target="#pyloadRemovalConfirm" class="btn btn-xs btn-success"><?php echo T('INSTALLED'); ?></a></td>
                      <?php } else { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="?installpackage-pyload=true" data-toggle="modal" data-target="#sysResponse" id="pyloadInstall" class="btn btn-xs btn-default"><?php echo T('INSTALL'); ?></a></td>
                      <?php } ?>
                    </tr>
                    <tr>
                      <td>Quassel</td>
                      <td><?php echo T('QUASSEL'); ?></td>
                      <?php if (file_exists("/install/.quassel.lock")) { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="javascript:void()" data-toggle="modal" data-target="#quasselRemovalConfirm" class="btn btn-xs btn-success"><?php echo T('INSTALLED'); ?></a></td>
                      <?php } else { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="?installpackage-quassel=true" data-toggle="modal" data-target="#sysResponse" id="quasselInstall" class="btn btn-xs btn-default"><?php echo T('INSTALL'); ?></a></td>
                      <?php } ?>
                    </tr>
                    <tr>
                      <td>Quotas</td>
                      <td><?php echo T('QUOTAS'); ?></td>
                      <?php if (file_exists("/install/.quota.lock")) { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="javascript:void()" data-toggle="modal" data-target="#quotaRemovalConfirm" class="btn btn-xs btn-success"><?php echo T('INSTALLED'); ?></a></td>
                      <?php } else { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="?installpackage-quota=true" data-toggle="modal" data-target="#sysResponse" id="quotaInstall" class="btn btn-xs btn-default"><?php echo T('INSTALL'); ?></a></td>
                      <?php } ?>
                    </tr>
                    <tr>
                      <td>Radarr</td>
                      <td><?php echo T('RADARR'); ?></td>
                      <?php if (file_exists("/install/.radarr.lock")) { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="javascript:void()" data-toggle="modal" data-target="#radarrRemovalConfirm" class="btn btn-xs btn-success"><?php echo T('INSTALLED'); ?></a></td>
                      <?php } else { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="?installpackage-radarr=true" data-toggle="modal" data-target="#sysResponse" id="radarrInstall" class="btn btn-xs btn-default"><?php echo T('INSTALL'); ?></a></td>
                      <?php } ?>
                    </tr>
                    <tr>
                      <td>Rapidleech</td>
                      <td><?php echo T('RAPIDLEECH'); ?></td>
                      <?php if (file_exists("/install/.rapidleech.lock")) { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="javascript:void()" data-toggle="modal" data-target="#rapidleechRemovalConfirm" class="btn btn-xs btn-success"><?php echo T('INSTALLED'); ?></a></td>
                      <?php } else { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="?installpackage-rapidleech=true" data-toggle="modal" data-target="#sysResponse" id="rapidleechInstall" class="btn btn-xs btn-default"><?php echo T('INSTALL'); ?></a></td>
                      <?php } ?>
                    </tr>
                    <tr>
                      <td>SABnzbd</td>
                      <td><?php echo T('SABNZBD'); ?></td>
                      <?php if (file_exists("/install/.sabnzbd.lock")) { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="javascript:void()" data-toggle="modal" data-target="#sabnzbdRemovalConfirm" class="btn btn-xs btn-success"><?php echo T('INSTALLED'); ?></a></td>
                      <?php } else { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="?installpackage-sabnzbd=true" data-toggle="modal" data-target="#sysResponse" id="sabnzbdInstall" class="btn btn-xs btn-default"><?php echo T('INSTALL'); ?></a></td>
                      <?php } ?>
                    </tr>
                    <tr>
                      <td>SickGear</td>
                      <td><?php echo T('SICKGEAR'); ?></td>
                      <?php if (file_exists("/install/.sickgear.lock")) { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="javascript:void()" data-toggle="modal" data-target="#sickgearRemovalConfirm" class="btn btn-xs btn-success"><?php echo T('INSTALLED'); ?></a></td>
                      <?php } else { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="?installpackage-sickgear=true" data-toggle="modal" data-target="#sysResponse" id="sickgearInstall" class="btn btn-xs btn-default"><?php echo T('INSTALL'); ?></a></td>
                      <?php } ?>
                    </tr>
                    <tr>
                      <td>SickRage</td>
                      <td><?php echo T('SICKRAGE'); ?></td>
                      <?php if (file_exists("/install/.sickrage.lock")) { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="javascript:void()" data-toggle="modal" data-target="#sickrageRemovalConfirm" class="btn btn-xs btn-success"><?php echo T('INSTALLED'); ?></a></td>
                      <?php } else { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="?installpackage-sickrage=true" data-toggle="modal" data-target="#sysResponse" id="sickrageInstall" class="btn btn-xs btn-default"><?php echo T('INSTALL'); ?></a></td>
                      <?php } ?>
                    </tr>
                    <tr>
                      <td>Sonarr</td>
                      <td><?php echo T('SONARR'); ?></td>
                      <?php if (file_exists("/install/.sonarr.lock")) { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="javascript:void()" data-toggle="modal" data-target="#sonarrRemovalConfirm" class="btn btn-xs btn-success"><?php echo T('INSTALLED'); ?></a></td>
                      <?php } else { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="?installpackage-sonarr=true" data-toggle="modal" data-target="#sysResponse" id="sonarrInstall" class="btn btn-xs btn-default"><?php echo T('INSTALL'); ?></a></td>
                      <?php } ?>
                    </tr>
                    <tr>
                      <td>Subsonic</td>
                      <td><?php echo T('SUBSONIC'); ?></td>
                      <?php if (file_exists("/install/.subsonic.lock")) { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="javascript:void()" data-toggle="modal" data-target="#subsonicRemovalConfirm" class="btn btn-xs btn-success"><?php echo T('INSTALLED'); ?></a></td>
                      <?php } else { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="?installpackage-subsonic=true" data-toggle="modal" data-target="#sysResponse" id="subsonicInstall" class="btn btn-xs btn-default"><?php echo T('INSTALL'); ?></a></td>
                      <?php } ?>
                    </tr>
                    <td>Syncthing</td>
                    <td><?php echo T('SYNCTHING'); ?></td>
                    <?php if (file_exists("/install/.syncthing.lock")) { ?>
                      <td style="vertical-align: middle; text-align: center"><a href="javascript:void()" data-toggle="modal" data-target="#syncthingRemovalConfirm" class="btn btn-xs btn-success"><?php echo T('INSTALLED'); ?></a></td>
                    <?php } else { ?>
                      <td style="vertical-align: middle; text-align: center"><a href="?installpackage-syncthing=true" data-toggle="modal" data-target="#sysResponse" id="syncthingInstall" class="btn btn-xs btn-default"><?php echo T('INSTALL'); ?></a></td>
                    <?php } ?>
                  </tr>
                    <tr>
                      <td>x2Go</td>
                      <td><?php echo T('X2GO'); ?></td>
                      <?php if (file_exists("/install/.x2go.lock")) { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="javascript:void()" data-toggle="modal" data-target="#x2goRemovalConfirm" class="btn btn-xs btn-success"><?php echo T('INSTALLED'); ?></a></td>
                      <?php } else { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="?installpackage-x2go=true" data-toggle="modal" data-target="#sysResponse" id="x2goInstall" class="btn btn-xs btn-default"><?php echo T('INSTALL'); ?></a></td>
                      <?php } ?>
                    </tr>
                    <tr>
                      <td>ZNC</td>
                      <td><?php echo T('ZNC'); ?></td>
                      <?php if (file_exists("/install/.znc.lock")) { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="javascript:void()" data-toggle="modal" data-target="#zncRemovalConfirm" class="btn btn-xs btn-success"><?php echo T('INSTALLED'); ?></a></td>
                      <?php } else { ?>
                        <td style="vertical-align: middle; text-align: center"><button data-toggle="tooltip" title="<?php echo T('BOX_TOOLTIP_ZNC'); ?>" data-placement="top" class="btn btn-xs btn-danger disabled tooltips"><?php echo T('BOX'); ?></button></td>
                      <?php } ?>
                    </tr>
                  </tbody>
                </table>
              </div>
            </div>
          </div><!-- package center panel -->
