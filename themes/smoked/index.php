<?php
  include ('inc/config.php');
  include ('inc/panel.header.php');
  include ('inc/panel.menu.php');
?>

  <div class="mainpanel">
    <!--<div class="pageheader">
      <h2><i class="fa fa-home"></i> Dashboard</h2>
    </div>-->
    <div class="contentpanel">

      <div class="row">

        <div class="col-md-8">

          <!--BANDWIDTH CHART & DATA-->
          <div class="panel panel-main panel-inverse">
            <div class="panel-heading">
              <h4 class="panel-title"><?php echo T('BANDWIDTH_DATA'); ?></h4>
            </div>
            <div class="panel-body text-center" style="padding:0 0 0 5px; overflow: hidden !important">
              <div style="margin-right: -30px">
                  <div id="mainbw" style="width:100%;height:350px;"></div>
                </div>
            </div>
            <div class="row panel-footer panel-statistics" style="padding:0">
              <div class="col-md-12">
                <div class="table-responsive">
                  <table class="table table-hover table-bordered nomargin">
                    <thead>
                      <tr>
                        <th style="width:33%;padding: 4px 4px 4px 12px"><?php echo T('NETWORK'); ?></th>
                        <th style="width:33%;padding: 4px 4px 4px 12px"><?php echo T('UPLOAD'); ?></th>
                        <th style="width:33%;padding: 4px 4px 4px 12px"><?php echo T('DOWNLOAD'); ?></th>
                      </tr>
                    </thead>
                    <tbody>
                      <?php if (false !== ($strs = @file("/proc/net/dev"))) : ?>
                      <?php for ($i = 2; $i < count($strs); $i++ ) : ?>
                      <?php preg_match_all( "/([^\s]+):[\s]{0,}(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)/", $strs[$i], $info );?>
                      <tr>
                        <td style="font-size:14px;font-weight:bold;padding: 2px 2px 2px 12px"><?php echo $info[1][0]?></td>
                        <td style="font-size:11px;padding: 2px 2px 2px 12px"><span class="text-success"><span id="NetOutSpeed<?php echo $i?>">0B/s</span></span></td>
                        <td style="font-size:11px;padding: 2px 2px 2px 12px"><span class="text-primary"><span id="NetInputSpeed<?php echo $i?>">0B/s</span></span></td>
                      </tr>
                      <?php endfor; ?>
                      <?php endif; ?>
                    </tbody>
                  </table>
                </div>
              </div>
            </div>
          </div>
          <div class="panel panel-inverse">
            <div class="panel-heading">
              <h4 class="panel-title"><?php echo T('VIEW_ADDITIONAL_BANDWIDTH_DETAILS'); ?></h4>
            </div>
            <div class="panel-body" style="padding:0">
              <div class="row" style="padding: 0; margin: 0"><div id="bw_tables" style="padding:0;margin:0;"></div></div>
            </div>
          </div>

          <!--SERVICE CONTROL CENTER-->
          <div class="panel panel-inverse">
            <div class="panel-heading">
              <h4 class="panel-title"><?php echo T('SERVICE_CONTROL_CENTER'); ?></h4>
            </div>
            <div class="panel-body" style="padding: 0">
              <div class="table-responsive">
                <table class="table table-hover nomargin" style="font-size:14px">
                  <thead>
                    <tr>
                      <th class="text-center"><?php echo T('SERVICE_STATUS'); ?></th>
                      <th class="text-center"><?php echo T('RESTART_SERVICES'); ?></th>
                      <th class="text-center"><?php echo T('ENABLE_DISABLE_SERVICES'); ?></th>
                    </tr>
                  </thead>
                  <tbody>
                  <?php if (file_exists("/install/.rtorrent.lock")) { ?>
                    <tr>
                      <?php
                      $rtorrentrc = '/home/'.$username.'/.rtorrent.rc';
                      if (file_exists($rtorrentrc)) {
                          $rtorrentrc_data = file_get_contents($rtorrentrc);
                          $scgiport = search($rtorrentrc_data, 'localhost:', "\n");
                      }
                      ?>
                      <td><?php echo "$rval"; ?> RTorrent <span class="tooltips" data-toggle="tooltip" title="scgi_port: <?php echo $scgiport; ?>" data-placement="right"><i class="tooltips fa fa-usb"></i><span></td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=rtorrent'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodyr"; ?></td>
                    </tr>
                  <?php } ?>

                  <?php if (file_exists("/install/.autodlirssi.lock")) { ?>
                    <tr>
                      <td><?php echo "$ival"; ?> iRSSi-AutoDL </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=irssi'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodyi"; ?></td>
                    </tr>
                    <?php } ?>

                  <?php if (file_exists("/install/.deluge.lock")) { ?>
                    <tr>
                      <td><?php echo "$dval"; ?> DelugeD </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=deluged'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodyd"; ?></td>
                    </tr>
                    <tr>
                      <td><?php echo "$dwval"; ?> Deluge Web </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=delugeweb'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodydw"; ?></td>
                    </tr>
                    <?php } ?>

                <?php if ($username == "$master") { ?>

                    <tr>
                      <td><?php echo "$wcval"; ?> Web Console </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=shellinabox'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$wcbodyb"; ?></td>
                    </tr>

                    <!--
                    <//?php if (file_exists("/install/.csf.lock")) { ?>
                    <tr>
                      <td><//?php echo "$csfval"; ?> CSF (firewall) </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=csf'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <//?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><//?php echo "$cbodycf"; ?></td>
                    </tr>
                    <//?php } ?>
                    -->

                    <?php if (file_exists("/install/.btsync.lock")) { ?>
                    <tr>
                      <td><?php echo "$bval"; ?> BTSync </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=btsync'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodyb"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.couchpotato.lock")) { ?>
                    <tr>
                      <td><?php echo "$cpval"; ?> CouchPotato </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=couchpotato'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodycp"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.emby.lock")) { ?>
                    <tr>
                      <td><?php echo "$eval"; ?> Emby </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=emby'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodye"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.jackett.lock")) { ?>
                    <tr>
                      <td><?php echo "$jval"; ?> Jackett </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=jackett'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodyj"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.plex.lock")) { ?>
                    <tr>
                      <td><?php echo "$pval"; ?> Plex </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=plex'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodyp"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.plexpy.lock")) { ?>
                    <tr>
                      <td><?php echo "$ppval"; ?> PlexPy </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=plexpy'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodypp"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.ombi.lock")) { ?>
                    <tr>
                      <td><?php echo "$prval"; ?> Ombi </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=ombi'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodypr"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.pyload.lock")) { ?>
                    <tr>
                      <td><?php echo "$plval"; ?> pyLoad </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=pyload'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodypl"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.sabnzbd.lock")) { ?>
                    <tr>
                      <td><?php echo "$srval"; ?> SABnzbd </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=sabnzbd'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodysz"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.sickrage.lock")) { ?>
                    <tr>
                      <td><?php echo "$srval"; ?> SickRage </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=sickrage'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodysr"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.sonarr.lock")) { ?>
                    <tr>
                      <td><?php echo "$sval"; ?> Sonarr </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=sonarr'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodys"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.subsonic.lock")) { ?>
                    <tr>
                      <td><?php echo "$ssval"; ?> Subsonic </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=subsonic'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodyss"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.syncthing.lock")) { ?>
                    <tr>
                      <td><?php echo "$stval"; ?> Syncthing </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=syncthing'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodyst"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.sample.lock")) { ?>
                    <tr>
                      <td><?php echo "$val"; ?> SAMPLE </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=sample'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbody"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php } ?>
                  </tbody>
                </table>
              </div><!-- table-responsive -->
            </div>
          </div><!-- panel -->

          <?php if ($username == "$master") { ?>
          <!--PACKAGE MANAGEMENT CENTER-->
          <div class="panel panel-main panel-inverse">
            <div class="panel-heading">
              <h4 class="panel-title"><?php echo T('PACKAGE_MANAGEMENT_CENTER'); ?></h4>
            </div>
            <div class="panel-body text-center" style="padding:0;">
              <div class="alert alert-danger">
                <button type="button" class="close" data-dismiss="alert" aria-hidden="true">&times;</button>
                <?php echo T('PMC_NOTICE_TXT'); ?>
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
                      <td>Jackett</td>
                      <td><?php echo T('JACKETT'); ?></td>
                      <?php if (file_exists("/install/.jackett.lock")) { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="javascript:void()" data-toggle="modal" data-target="#jackettRemovalConfirm" class="btn btn-xs btn-success"><?php echo T('INSTALLED'); ?></a></td>
                      <?php } else { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="?installpackage-jackett=true" data-toggle="modal" data-target="#sysResponse" id="jackettInstall" class="btn btn-xs btn-default"><?php echo T('INSTALL'); ?></a></td>
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
                      <td>Ombi (formerly Plex Requests.NET)</td>
                      <td><?php echo T('PLEX_REQUESTS'); ?></td>
                      <?php if (file_exists("/install/.ombi.lock")) { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="javascript:void()" data-toggle="modal" data-target="#ombiRemovalConfirm" class="btn btn-xs btn-success"><?php echo T('INSTALLED'); ?></a></td>
                      <?php } else { ?>
                        <td style="vertical-align: middle; text-align: center"><a href="?installpackage-ombi=true" data-toggle="modal" data-target="#sysResponse" id="ombiInstall" class="btn btn-xs btn-default"><?php echo T('INSTALL'); ?></a></td>
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
          <?php } ?>
        </div>

        <div class="col-md-4 dash-right">
          <div class="row">
            <div class="col-sm-12">
              <div class="panel panel-side panel-inverse-full panel-updates">
                <div class="panel-heading">
                  <h4 class="panel-title text-success"><?php echo T('SERVER_LOAD'); ?></h4>
                </div>
                <div class="panel-body">
                  <div class="row">
                    <div class="col-sm-9">
                      <h4><span id="cpuload"></span></h4>
                      <p><?php echo T('SL_TXT'); ?></p>
                    </div>
                    <div class="col-sm-3 text-right">
                      <i class="fa fa-heartbeat text-danger" style="font-size: 70px"></i>
                    </div>
                    <div class="row">
                      <div class="col-sm-12 mt20 text-center">
                        <strong><?php echo T('UPTIME'); ?>:</strong> <span id="uptime"></span>
                      </div>
                    </div>
                  </div>
                </div>
              </div><!-- panel -->
            </div><!-- SERVER LOAD WIDGET -->
            <div class="col-sm-12">
              <div class="panel panel-side panel-inverse">
                <div class="panel-heading">
                  <h4 class="panel-title"><?php echo T('YOUR_DISK_STATUS'); ?></h4>
                </div>
                <div class="panel-body">
                  <div id="disk_data"></div>
                </div>
              </div>
            </div><!-- DISK WIDGET -->
            <div class="col-sm-12">
              <div class="panel panel-side panel-inverse">
                <div class="panel-heading">
                  <h4 class="panel-title"><?php echo T('SYSTEM_RAM_STATUS'); ?></h4>
                </div>
                <div class="panel-body">
                  <div id="meterram"></div>
                </div>
              </div>
            </div><!-- RAM WIDGET -->
            <div class="col-sm-12">
              <div class="panel panel-side panel-inverse">
                <div class="panel-heading">
                  <h4 class="panel-title"><?php echo T('CPU_STATUS'); ?></h4>
                </div>
                <div class="panel-body" style="overflow:hidden">
                  <div style="padding:0;margin:-15px -30 -15px -15px">
                    <div id="flot-placeholder1" style="width:100%;height:200px;"></div>
                    <!--div id="metercpu"></div-->
                  </div>
                  <hr />
                  <span class="nomargin" style="font-size:14px">
                    <?php echo $sysInfo['cpu']['model'];?><br/>
                    [<span style="color:#999;font-weight:600">x<?php echo $sysInfo['cpu']['num']; ?></span> core]
                  </span>
                </div>
              </div>
            </div><!-- CPU WIDGET -->
            <?php if ($username == "$master") { ?>
            <div class="col-sm-12">
              <div id="project-commits" class="panel panel-inverse">
                <div class="panel-heading">
                  <h4 class="panel-title text-success"><?php echo T('RECENT_UPDATES'); ?>

                    <small><a href="https://github.com/QuickBox/QB/compare/<?php echo $version ?>...master" target="blank" title="Compare recent QuickBox versions to latest commits" data-placement="top" class="label label-primary tooltips" style="font-size:10px; padding-top:0; padding-bottom:0px; top: -2px; position: relative;" target='_blank'>QuickBox :: <span style="color: #fff;text-shadow: 0px 0px 6px #fff;"><?php echo "$version"; ?></span></a></small>

                  </h4>
                </div>
                <div class="panel-body ps-container" style="max-height: 350px; padding: 0;">
                  <div class="alert alert-default" style="margin-bottom: 4px; text-align: center" role="alert">
                    <strong><?php echo T('RECENT_UPDATES_TXT'); ?></strong>
                  </div>
                  <div id="activityfeed"></div>
                </div>
                <div class="panel-footer">
                <a href="?updateQuickBox=true" data-toggle="modal" data-target="#sysResponse"><button class="btn btn-success btn-quirk btn-block"><i class="fa fa-bell text-success"></i> <?php echo T('UPDATE'); ?> </button></a>
                </div>
              </div>
            </div><!-- QUICKBOX UPDATE WIDGET -->
            <?php } ?>
          </div><!-- row -->
        </div>
      </div>
    </div><!-- contentpanel -->
  </div><!-- mainpanel -->

  <?php
    include ('inc/panel.scripts.php');
    include ('inc/panel.end.php');
  ?>
