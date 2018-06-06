$(document).ready(function() {

  /////////////////////////////////////////////
  // BEGIN AJAX APP CALLS ON SERVICE STATUS //
  ///////////////////////////////////////////

  // <<-------- BTSYNC -------->> //
  function appstat_btsync() {
    $.ajax({url: "/widgets/app_status/app_status_btsync.php", cache:true, success: function (result) {
      $('#appstat_btsync').html(result);
      setTimeout(function(){appstat_btsync()}, 1000);
    }});
  }
  appstat_btsync();

  // <<-------- COUCHPOTATO -------->> //
  function appstat_couchpotato() {
    $.ajax({url: "/widgets/app_status/app_status_couchpotato.php", cache:true, success: function (result) {
      $('#appstat_couchpotato').html(result);
      setTimeout(function(){appstat_couchpotato()}, 1000);
    }});
  }
  appstat_couchpotato();

  // <<-------- DELUGED -------->> //
  function appstat_deluged() {
    $.ajax({url: "/widgets/app_status/app_status_deluged.php", cache:true, success: function (result) {
      $('#appstat_deluged').html(result);
      setTimeout(function(){appstat_deluged()}, 1000);
    }});
  }
  appstat_deluged();

  // <<-------- DELUGE WEB -------->> //
  function appstat_delugeweb() {
    $.ajax({url: "/widgets/app_status/app_status_delugeweb.php", cache:true, success: function (result) {
      $('#appstat_delugeweb').html(result);
      setTimeout(function(){appstat_delugeweb()}, 1000);
    }});
  }
  appstat_delugeweb();

  // <<-------- EMBY -------->> //
  function appstat_emby() {
    $.ajax({url: "/widgets/app_status/app_status_emby.php", cache:true, success: function (result) {
      $('#appstat_emby').html(result);
      setTimeout(function(){appstat_emby()}, 1000);
    }});
  }
  appstat_emby();

  // <<-------- HEADPHONES -------->> //
  function appstat_headphones() {
    $.ajax({url: "/widgets/app_status/app_status_headphones.php", cache:true, success: function (result) {
      $('#appstat_headphones').html(result);
      setTimeout(function(){appstat_headphones()}, 1000);
    }});
  }
  appstat_headphones();

  // <<-------- IRSSI -------->> //
  function appstat_irssi() {
    $.ajax({url: "/widgets/app_status/app_status_irssi.php", cache:true, success: function (result) {
      $('#appstat_irssi').html(result);
      setTimeout(function(){appstat_irssi()}, 1000);
    }});
  }
  appstat_irssi();

  // <<-------- JACKETT -------->> //
  function appstat_jackett() {
    $.ajax({url: "/widgets/app_status/app_status_jackett.php", cache:true, success: function (result) {
      $('#appstat_jackett').html(result);
      setTimeout(function(){appstat_jackett()}, 1000);
    }});
  }
  appstat_jackett();
  
  // <<-------- LIDARR -------->> //
  function appstat_lidarr() {
    $.ajax({url: "/widgets/app_status/app_status_lidarr.php", cache:true, success: function (result) {
      $('#appstat_lidarr').html(result);
      setTimeout(function(){appstat_lidarr()}, 1000);
    }});
  }
  appstat_lidarr();

  // <<-------- MEDUSA -------->> //
  function appstat_medusa() {
    $.ajax({url: "/widgets/app_status/app_status_medusa.php", cache:true, success: function (result) {
      $('#appstat_medusa').html(result);
      setTimeout(function(){appstat_medusa()}, 1000);
    }});
  }
  appstat_medusa();

  // <<-------- NEXTCLOUD -------->> //
  function appstat_nextcloud() {
    $.ajax({url: "/widgets/app_status/app_status_nextcloud.php", cache:true, success: function (result) {
      $('#appstat_nextcloud').html(result);
      setTimeout(function(){appstat_nextcloud()}, 1000);
    }});
  }
  appstat_nextcloud();

  // <<-------- NZBGET -------->> //
  function appstat_nzbget() {
    $.ajax({url: "/widgets/app_status/app_status_nzbget.php", cache:true, success: function (result) {
      $('#appstat_nzbget').html(result);
      setTimeout(function(){appstat_nzbget()}, 1000);
    }});
  }
  appstat_nzbget();

  // <<-------- NZBHYDRA -------->> //
  function appstat_nzbhydra() {
    $.ajax({url: "/widgets/app_status/app_status_nzbhydra.php", cache:true, success: function (result) {
      $('#appstat_nzbhydra').html(result);
      setTimeout(function(){appstat_nzbhydra()}, 1000);
    }});
  }
  appstat_nzbhydra();

  // <<-------- OMBI -------->> //
  function appstat_ombi() {
    $.ajax({url: "/widgets/app_status/app_status_ombi.php", cache:true, success: function (result) {
      $('#appstat_ombi').html(result);
      setTimeout(function(){appstat_ombi()}, 1000);
    }});
  }
  appstat_ombi();

  // <<-------- OPENVPN -------->> //
  function appstat_openvpn() {
    $.ajax({url: "/widgets/app_status/app_status_openvpn.php", cache:true, success: function (result) {
      $('#appstat_openvpn').html(result);
      setTimeout(function(){appstat_openvpn()}, 1000);
    }});
  }
  appstat_openvpn();

  // <<-------- PLEX -------->> //
  function appstat_plex() {
    $.ajax({url: "/widgets/app_status/app_status_plex.php", cache:true, success: function (result) {
      $('#appstat_plex').html(result);
      setTimeout(function(){appstat_plex()}, 1000);
    }});
  }
  appstat_plex();

  // <<-------- Tautulli -------->> //
  function appstat_Tautulli() {
    $.ajax({url: "/widgets/app_status/app_status_Tautulli.php", cache:true, success: function (result) {
      $('#appstat_Tautulli').html(result);
      setTimeout(function(){appstat_Tautulli()}, 1000);
    }});
  }
  appstat_Tautulli();

  // <<-------- PYLOAD -------->> //
  function appstat_pyload() {
    $.ajax({url: "/widgets/app_status/app_status_pyload.php", cache:true, success: function (result) {
      $('#appstat_pyload').html(result);
      setTimeout(function(){appstat_pyload()}, 1000);
    }});
  }
  appstat_pyload();

  // <<-------- QUASSEL -------->> //
  function appstat_quassel() {
    $.ajax({url: "/widgets/app_status/app_status_quassel.php", cache:true, success: function (result) {
      $('#appstat_quassel').html(result);
      setTimeout(function(){appstat_quassel()}, 1000);
    }});
  }
  appstat_quassel();

  // <<-------- RADARR -------->> //
  function appstat_radarr() {
    $.ajax({url: "/widgets/app_status/app_status_radarr.php", cache:true, success: function (result) {
      $('#appstat_radarr').html(result);
      setTimeout(function(){appstat_radarr()}, 1000);
    }});
  }
  appstat_radarr();

  // <<-------- RAPIDLEECH -------->> //
  function appstat_rapidleech() {
    $.ajax({url: "/widgets/app_status/app_status_rapidleech.php", cache:true, success: function (result) {
      $('#appstat_rapidleech').html(result);
      setTimeout(function(){appstat_rapidleech()}, 1000);
    }});
  }
  appstat_rapidleech();

  // <<-------- RTORRENT -------->> //
  function appstat_rtorrent() {
    $.ajax({url: "/widgets/app_status/app_status_rtorrent.php", cache:true, success: function (result) {
      $('#appstat_rtorrent').html(result);
      setTimeout(function(){appstat_rtorrent()}, 1000);
    }});
  }
  appstat_rtorrent();

  // <<-------- SABNZBD -------->> //
  function appstat_sabnzbd() {
    $.ajax({url: "/widgets/app_status/app_status_sabnzbd.php", cache:true, success: function (result) {
      $('#appstat_sabnzbd').html(result);
      setTimeout(function(){appstat_sabnzbd()}, 1000);
    }});
  }
  appstat_sabnzbd();

  // <<-------- SICKGEAR -------->> //
  function appstat_sickgear() {
    $.ajax({url: "/widgets/app_status/app_status_sickgear.php", cache:true, success: function (result) {
      $('#appstat_sickgear').html(result);
      setTimeout(function(){appstat_sickgear()}, 1000);
    }});
  }
  appstat_sickgear();

  // <<-------- SICKRAGE -------->> //
  function appstat_sickrage() {
    $.ajax({url: "/widgets/app_status/app_status_sickrage.php", cache:true, success: function (result) {
      $('#appstat_sickrage').html(result);
      setTimeout(function(){appstat_sickrage()}, 1000);
    }});
  }
  appstat_sickrage();

  // <<-------- SONARR -------->> //
  function appstat_sonarr() {
    $.ajax({url: "/widgets/app_status/app_status_sonarr.php", cache:true, success: function (result) {
      $('#appstat_sonarr').html(result);
      setTimeout(function(){appstat_sonarr()}, 1000);
    }});
  }
  appstat_sonarr();

  // <<-------- SUBSONIC -------->> //
  function appstat_subsonic() {
    $.ajax({url: "/widgets/app_status/app_status_subsonic.php", cache:true, success: function (result) {
      $('#appstat_subsonic').html(result);
      setTimeout(function(){appstat_subsonic()}, 1000);
    }});
  }
  appstat_subsonic();

  // <<-------- SYNCTHING -------->> //
  function appstat_syncthing() {
    $.ajax({url: "/widgets/app_status/app_status_syncthing.php", cache:true, success: function (result) {
      $('#appstat_syncthing').html(result);
      setTimeout(function(){appstat_syncthing()}, 1000);
    }});
  }
  appstat_syncthing();

  // <<-------- TRANSMISSION -------->> //
  function appstat_transmission() {
    $.ajax({url: "/widgets/app_status/app_status_transmission.php", cache:true, success: function (result) {
      $('#appstat_transmission').html(result);
      setTimeout(function(){appstat_transmission()}, 1000);
    }});
  }
  appstat_transmission();

  // <<-------- WEB CONSOLE -------->> //
  function appstat_webconsole() {
    $.ajax({url: "/widgets/app_status/app_status_webconsole.php", cache:true, success: function (result) {
      $('#appstat_webconsole').html(result);
      setTimeout(function(){appstat_webconsole()}, 1000);
    }});
  }
  appstat_webconsole();

  // <<-------- X2GO -------->> //
  function appstat_x2go() {
    $.ajax({url: "/widgets/app_status/app_status_x2go.php", cache:true, success: function (result) {
      $('#appstat_x2go').html(result);
      setTimeout(function(){appstat_x2go()}, 1000);
    }});
  }
  appstat_x2go();

  // <<-------- ZNC -------->> //
  function appstat_znc() {
    $.ajax({url: "/widgets/app_status/app_status_znc.php", cache:true, success: function (result) {
      $('#appstat_znc').html(result);
      setTimeout(function(){appstat_znc()}, 1000);
    }});
  }
  appstat_znc();
  

  ///////////////////////////////////////////
  // END AJAX APP CALLS ON SERVICE STATUS //
  /////////////////////////////////////////

  function uptime() {
    $.ajax({url: "/widgets/up.php", cache:true, success: function (result) {
      $('#uptime').html(result);
      setTimeout(function(){uptime()}, 1000);
    }});
  }
  uptime();

  function sload() {
    $.ajax({url: "/widgets/load.php", cache:true, success: function (result) {
      $('#cpuload').html(result);
      setTimeout(function(){sload()}, 1000);
    }});
  }
  sload();

  function bwtables() {
    $.ajax({url: "/widgets/bw_tables.php", cache:false, success: function (result) {
      $('#bw_tables').html(result);
      setTimeout(function(){bwtables()}, 60000);
    }});
  }
  bwtables();

  function diskstats() {
    $.ajax({url: "/widgets/disk_data.php", cache:false, success: function (result) {
      $('#disk_data').html(result);
      setTimeout(function(){diskstats()}, 15000);
    }});
  }
  diskstats();

  function ramstats() {
    $.ajax({url: "/widgets/ram_stats.php", cache:false, success: function (result) {
      $('#meterram').html(result);
      setTimeout(function(){ramstats()}, 1000);
    }});
  }
  ramstats();

  function activefeed() {
    $.ajax({url: "/widgets/activity_feed.php", cache:false, success: function (result) {
      $('#activityfeed').html(result);
      setTimeout(function(){activefeed()}, 300000);
    }});
  }
  activefeed();

  function msgoutput() {
    $.ajax({url: "/db/output.log", cache:false, success: function (result) {
      $('#sshoutput').html(result);
      setTimeout(function(){msgoutput()}, 1000);
    }});
    jQuery( function(){
      var pre = jQuery("#sysPre");
      pre.scrollTop( pre.prop("scrollHeight") );
    });
  }
  msgoutput();

  });
  //success: function (result)
