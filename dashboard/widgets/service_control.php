<?php
  include ('..inc/config.php');
  include ('..inc/panel.header.php');
  include ('..inc/panel.menu.php');
?>

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
                      <td><span id="appstat_rtorrent"></span> RTorrent <span class="tooltips" data-toggle="tooltip" title="scgi_port: <?php echo $scgiport; ?>" data-placement="right"><i class="tooltips fa fa-usb"></i><span></td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=rtorrent'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodyr"; ?></td>
                    </tr>
                  <?php } ?>

                  <?php if (file_exists("/install/.autodlirssi.lock")) { ?>
                    <tr>
                      <td><span id="appstat_irssi"></span> iRSSi-AutoDL </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=irssi'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodyi"; ?></td>
                    </tr>
                    <?php } ?>

                  <?php if (file_exists("/install/.deluge.lock")) { ?>
                    <tr>
                      <td><span id="appstat_deluged"></span> DelugeD </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=deluged'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodyd"; ?></td>
                    </tr>
                    <tr>
                      <td><span id="appstat_delugeweb"></span> Deluge Web </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=delugeweb'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodydw"; ?></td>
                    </tr>
                    <?php } ?>

                  <?php if (file_exists("/install/.transmission.lock")) { ?>
                    <tr>
                      <td><span id="appstat_transmission"></span> Transmission </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=transmission'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodytr"; ?></td>
                    </tr>
                    <?php } ?>

                <?php if ($username == "$master") { ?>

                    <tr>
                      <td><span id="appstat_webconsole"></span> Web Console </td>
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
                      <td><span id="appstat_btsync"></span> BTSync </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=btsync'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodyb"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.couchpotato.lock")) { ?>
                    <tr>
                      <td><span id="appstat_couchpotato"></span> CouchPotato </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=couchpotato'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodycp"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.emby.lock")) { ?>
                    <tr>
                      <td><span id="appstat_emby"></span> Emby </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=emby'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodye"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.headphones.lock")) { ?>
                    <tr>
                      <td><span id="appstat_headphones"></span> Headphones </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=headphones'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodyhp"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.jackett.lock")) { ?>
                    <tr>
                      <td><span id="appstat_jackett"></span> Jackett </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=jackett'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodyj"; ?></td>
                    </tr>
                    <?php } ?>
					
					<?php if (file_exists("/install/.lidarr.lock")) { ?>
                    <tr>
                      <td><span id="appstat_lidarr"></span> Lidarr </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=lidarr'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodylid"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.medusa.lock")) { ?>
                    <tr>
                      <td><span id="appstat_medusa"></span> Medusa </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=medusa'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodym"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.nzbget.lock")) { ?>
                    <tr>
                      <td><span id="appstat_nzbget"></span> NZBGet </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=nzbget'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodynzg"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.nzbhydra.lock")) { ?>
                    <tr>
                      <td><span id="appstat_nzbhydra"></span> NZBHydra </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=nzbhydra'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodynzb"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.ombi.lock")) { ?>
                    <tr>
                      <td><span id="appstat_ombi"></span> Ombi </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=ombi'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodypr"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.plex.lock")) { ?>
                    <tr>
                      <td><span id="appstat_plex"></span> Plex </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=plex'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodyp"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.Tautulli.lock")) { ?>
                    <tr>
                      <td><span id="appstat_Tautulli"></span> Tautulli </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=Tautulli'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodypp"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.pyload.lock")) { ?>
                    <tr>
                      <td><span id="appstat_pyload"></span> pyLoad </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=pyload'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodypl"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.radarr.lock")) { ?>
                    <tr>
                      <td><span id="appstat_radarr"></span> Radarr </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=radarr'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodyrad"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.sabnzbd.lock")) { ?>
                    <tr>
                      <td><span id="appstat_sabnzbd"></span> SABnzbd </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=sabnzbd'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodysz"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.sickgear.lock")) { ?>
                    <tr>
                      <td><span id="appstat_sickgear"></span> SickGear </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=sickgear'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodysg"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.sickrage.lock")) { ?>
                    <tr>
                      <td><span id="appstat_sickrage"></span> SickRage </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=sickrage'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodysr"; ?></td>
                    </tr>
                    <?php } ?>
                                                                                                                                                                                              
                    <?php if (file_exists("/install/.sonarr.lock")) { ?>
                    <tr>
                      <td><span id="appstat_sonarr"></span> Sonarr </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=sonarr'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodys"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.subsonic.lock")) { ?>
                    <tr>
                      <td><span id="appstat_subsonic"></span> Subsonic </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=subsonic'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodyss"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.syncthing.lock")) { ?>
                    <tr>
                      <td><span id="appstat_syncthing"></span> Syncthing </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=syncthing'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodyst"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.sample.lock")) { ?>
                    <tr>
                      <td><?php echo "$sampleval"; ?> SAMPLE </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=sample'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$samplebody"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php } ?>
                  </tbody>
                </table>
              </div><!-- table-responsive -->
            </div>
          </div><!-- panel -->
