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

                    <?php if (file_exists("/install/.headphones.lock")) { ?>
                    <tr>
                      <td><?php echo "$hpval"; ?> Headphones </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=headphones'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodyhp"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.jackett.lock")) { ?>
                    <tr>
                      <td><?php echo "$jval"; ?> Jackett </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=jackett'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodyj"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.medusa.lock")) { ?>
                    <tr>
                      <td><?php echo "$mval"; ?> Medusa </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=medusa'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodypm"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.nzbget.lock")) { ?>
                    <tr>
                      <td><?php echo "$nzgval"; ?> NZBGet </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=nzbget'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodynzg"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.nzbhydra.lock")) { ?>
                    <tr>
                      <td><?php echo "$nzbval"; ?> NZBHydra </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=nzbhydra'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodynzb"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.ombi.lock")) { ?>
                    <tr>
                      <td><?php echo "$prval"; ?> Ombi </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=ombi'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodypr"; ?></td>
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

                    <?php if (file_exists("/install/.pyload.lock")) { ?>
                    <tr>
                      <td><?php echo "$plval"; ?> pyLoad </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=pyload'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodypl"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.radarr.lock")) { ?>
                    <tr>
                      <td><?php echo "$radval"; ?> Radarr </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=radarr'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodyrad"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.sabnzbd.lock")) { ?>
                    <tr>
                      <td><?php echo "$szval"; ?> SABnzbd </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=sabnzbd'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodysz"; ?></td>
                    </tr>
                    <?php } ?>

                    <?php if (file_exists("/install/.sickgear.lock")) { ?>
                    <tr>
                      <td><?php echo "$sgval"; ?> SickGear </td>
                      <td class="text-center"><a href="javascript:;" onclick="location.href='?id=88&servicestart=sickgear'" class="btn btn-xs btn-default"><i class="fa fa-refresh text-info"></i> <?php echo T('REFRESH'); ?></a></td>
                      <td class="text-center"><?php echo "$cbodysg"; ?></td>
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