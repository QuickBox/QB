$(document).ready(function() {

  /////////////////////////////////////////////
  // BEGIN AJAX APP CALLS ON SERVICE STATUS //
  ///////////////////////////////////////////


  function appstat_rtorrent() {
    $.ajax({url: "/widgets/app_status/app_status_rtorrent.php", cache:true, success: function (result) {
      $('#appstat_rtorrent').html(result);
      setTimeout(function(){appstat_rtorrent()}, 1000);
    }});
  }
  appstat_rtorrent();

  function appstat_irssi() {
    $.ajax({url: "/widgets/app_status/app_status_irssi.php", cache:true, success: function (result) {
      $('#appstat_irssi').html(result);
      setTimeout(function(){appstat_irssi()}, 1000);
    }});
  }
  appstat_irssi();

  function appstat_webconsole() {
    $.ajax({url: "/widgets/app_status/app_status_webconsole.php", cache:true, success: function (result) {
      $('#appstat_webconsole').html(result);
      setTimeout(function(){appstat_webconsole()}, 1000);
    }});
  }
  appstat_webconsole();

  function appstat_medusa() {
    $.ajax({url: "/widgets/app_status/app_status_medusa.php", cache:true, success: function (result) {
      $('#appstat_medusa').html(result);
      setTimeout(function(){appstat_medusa()}, 1000);
    }});
  }
  appstat_medusa();

  function appstat_plex() {
    $.ajax({url: "/widgets/app_status/app_status_plex.php", cache:true, success: function (result) {
      $('#appstat_plex').html(result);
      setTimeout(function(){appstat_plex()}, 1000);
    }});
  }
  appstat_plex();
  

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
