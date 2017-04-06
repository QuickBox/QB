<html lang="en">
<head>
  <!-- META -->
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0">
  <meta name="description" content="<?php echo $panel['description'] ?>">
  <meta name="author" content="<?php echo $panel['author'] ?>">
  <title><?php echo $panel['title'] ?></title>
  <meta name="robots" content="<?php echo $panel['robots'] ?>">
  <meta name="theme-color" content="#ffffff">
  <!-- FAVICON ASSETTS -->
  <link rel="apple-touch-icon" sizes="180x180" href="img/favicon/apple-touch-icon.png">
  <link rel="icon" type="image/png" href="img/favicon/favicon-32x32.png" sizes="32x32">
  <link rel="icon" type="image/png" href="img/favicon//favicon-16x16.png" sizes="16x16">
  <link rel="manifest" href="img/favicon/manifest.json">
  <link rel="mask-icon" href="img/favicon/safari-pinned-tab.svg" color="#5bbad5">
  <!-- CSS STYLESHEETS AND ASSETTS -->
  <link rel="shortcut icon" href="/img/favicon.ico" type="image/ico">
  <link rel="stylesheet" href="lib/jquery-ui/jquery-ui.css">
  <link rel="stylesheet" href="lib/Hover/hover.css">
  <link rel="stylesheet" href="lib/jquery-toggles/toggles-full.css">
  <link rel="stylesheet" href="lib/jquery.gritter/jquery.gritter.css">
  <link rel="stylesheet" href="lib/animate.css/animate.css">
  <link rel="stylesheet" href="lib/font-awesome/font-awesome.css">
  <link rel="stylesheet" href="lib/ionicons/css/ionicons.css">
  <link rel="stylesheet" href="lib/select2/select2.css">
  <link rel="stylesheet" href="skins/quick.css">
  <link rel="stylesheet" href="skins/lobipanel.css"/>
  <!-- JAVASCRIPT -->
  <script src="lib/modernizr/modernizr.js"></script>
  <script src="lib/jquery/jquery.js"></script>
  <script type="text/javascript" src="lib/flot/jquery.flot.js"></script>
  <script type="text/javascript" src="lib/flot/jquery.flot.time.js"></script>
  <script type="text/javascript" src="lib/flot/jquery.flot.resize.js"></script>
  <script type="text/javascript" src="lib/flot/jquery.flot.canvas.js"></script>
  <script type="text/javascript" src="lib/flot-spline/jquery.flot.spline.js"></script>
  <!--script type="text/javascript" src="lib/flot/jquery.flot.axislabels.js"></script-->

  <!--///// BANDWIDTH CHART /////-->
  <script id="source" language="javascript" type="text/javascript">
  $(document).ready(function() {
    var options = {
      series: {
        lines: {
          show: false
        },
        splines: {
          show: true,
          tension: 0.4,
          lineWidth: 1,
          fill: 0.4
        }
      },
      points: { show: false },
      legend: {
        show: true,
        labelBoxBorderColor: '#222',
        position: 'ne',
        margin: 10
      },
      grid: {
        borderWidth: 0,
        border: { show: false },
        color: '#333',
        labelMargin: 5,
        backgroundColor: '#222'
      },
      xaxis: {
        mode: "time",
        tickSize: [60, "second"],
        tickFormatter: function (v, axis) {
          var date = new Date(v);
          if (date.getSeconds() % 20 == 0) {
            var hours = date.getHours() < 10 ? "0" + date.getHours() : date.getHours();
            var minutes = date.getMinutes() < 10 ? "0" + date.getMinutes() : date.getMinutes();
            var seconds = date.getSeconds() < 10 ? "0" + date.getSeconds() : date.getSeconds();
            return hours + ":" + minutes + ":" + seconds;
          } else {
            return "";
          }
        },
        axisLabel: "Time",
        axisLabelUseCanvas: true,
        axisLabelPadding: 1,
        font: {
          size: 9,
          style: "normal",
          color: "#999999",
          weight: "light",
          family: "open sans",
          variant: "small-caps"
        }
      },
      yaxis: {
        min: 0,
        tickFormatter: function(val, axis) { return val < axis.max ? val.toFixed(2) : "MB/s"; },
        font: {
          size: 11,
          style: "normal",
          color: "#999",
          weight: "light",
          family: "open sans",
          variant: "small-caps"
        }
      },
      colors: ["#44BBFF", "#66CC99"],
      border: { show: false },
      shadowSize: 0
    };
    window.onresize = function(event) {
      var data = [];
      var placeholder = $("#mainbw");
      $.plot(placeholder, data, options);
        var iteration = 0;
        function fetchData() {
          ++iteration;
          function onDataReceived(series) {
            // we get all the data in one go, if we only got partial
            // data, we could merge it with what we already got
            data = series;
            var updateInterval = 30;
            var now = new Date().getTime();
            $.plot($("#mainbw"), data, options);
          fetchData();
          }
          $.ajax({
            url: "widgets/data.php",
            method: 'GET',
            dataType: 'json',
            success: onDataReceived
          });
        }
      setTimeout(fetchData, 30);
    }
  });
  </script>
  <!--///// CPU CHART /////-->
  <script id="source" language="javascript" type="text/javascript">
  var cpu = [];
  var dataset;
  var totalPoints = 100;
  var updateInterval = 1000;
  var now = new Date().getTime();
  var options = {
    series: {
      lines: {
        show: true,
        lineWidth: 1.5,
        fill: 0.4
      }
    },
    points: { show: false },
    legend: {
      show: true,
      noColumns: 0,
      labelBoxBorderColor: '#222',
      position: 'ne',
    },
    grid: {
      borderWidth: 0,
      border: { show: false },
      color: '#333',
      labelMargin: 5,
      backgroundColor: '#222'
    },
    xaxis: {
      mode: "time",
      tickSize: [60, "second"],
      tickFormatter: function (v, axis) {
        var date = new Date(v);
        if (date.getSeconds() % 20 == 0) {
          var hours = date.getHours() < 10 ? "0" + date.getHours() : date.getHours();
          var minutes = date.getMinutes() < 10 ? "0" + date.getMinutes() : date.getMinutes();
          var seconds = date.getSeconds() < 10 ? "0" + date.getSeconds() : date.getSeconds();
          return hours + ":" + minutes + ":" + seconds;
        } else {
          return "";
        }
      },
      axisLabel: "Time",
      axisLabelUseCanvas: true,
      axisLabelPadding: 1,
      font: {
        size: 9,
        style: "normal",
        color: "#999999",
        weight: "light",
        family: "open sans",
        variant: "small-caps"
      }
    },
    yaxes: [
      {
        min: 0,
        max: 100,
        tickSize: 5,
        tickFormatter: function (v, axis) {
          if (v % 10 == 0) {
            return v + "%";
          } else {
            return "";
          }
        },
        axisLabel: "CPU loading",
        axisLabelUseCanvas: true,
        axisLabelFontSizePixels: 6,
        //axisLabelFontSizePixels: 8,
        //axisLabelFontFamily: 'open sans',
        axisLabelPadding: 1,
        font: {
          size: 9,
          style: "normal",
          color: "#999999",
          weight: "light",
          family: "open sans",
          variant: "small-caps"
        }
      }
    ],
    border: { show: false },
    shadowSize: 0
  };
  function initData() {
    for (var i = 0; i < totalPoints; i++) {
      var temp = [now += updateInterval, 0];
      cpu.push(temp)
    }
  }
  function GetData() {
    $.ajaxSetup({ cache: false });

    $.ajax({
      url: "widgets/cpu.php",
      dataType: 'json',
      success: update,
      error: function () {
        setTimeout(GetData, updateInterval);
      }
    });
  }
  var temp;
  function update(_data) {
      cpu.shift();
      now += updateInterval
      temp = [now, _data.cpu];
      cpu.push(temp);
      dataset = [
        { label: "CPU:" + _data.cpu + "%", data: cpu, lines: { fill: 0.2, lineWidth: 1.5 }, color: "#B0A4BE" }
      ];
      $.plot($("#flot-placeholder1"), dataset, options);
      setTimeout(GetData, updateInterval);
  }
  $(document).ready(function () {
    initData();
    dataset = [
      { label: "CPU", data: cpu, lines:{fill:0.2, lineWidth:1}, color: "#B0A4BE" }
    ];
    $.plot($("#flot-placeholder1"), dataset, options);
    setTimeout(GetData, updateInterval);
  });
  </script>
  <script language="javascript" type="text/javascript">
  $(document).ready(function() {
  function uptime() {
    $.ajax({url: "widgets/up.php", cache:true, success: function (result) {
      $('#uptime').html(result);
      setTimeout(function(){uptime()}, 1000);
    }});
  }
  uptime();

  function sload() {
    $.ajax({url: "widgets/load.php", cache:true, success: function (result) {
      $('#cpuload').html(result);
      setTimeout(function(){sload()}, 1000);
    }});
  }
  sload();

  function bwtables() {
    $.ajax({url: "widgets/bw_tables.php", cache:false, success: function (result) {
      $('#bw_tables').html(result);
      setTimeout(function(){bwtables()}, 60000);
    }});
  }
  bwtables();

  function diskstats() {
    $.ajax({url: "widgets/disk_data.php", cache:false, success: function (result) {
      $('#disk_data').html(result);
      setTimeout(function(){diskstats()}, 15000);
    }});
  }
  diskstats();

  function ramstats() {
    $.ajax({url: "widgets/ram_stats.php", cache:false, success: function (result) {
      $('#meterram').html(result);
      setTimeout(function(){ramstats()}, 1000);
    }});
  }
  ramstats();

  function activefeed() {
    $.ajax({url: "widgets/activity_feed.php", cache:false, success: function (result) {
      $('#activityfeed').html(result);
      setTimeout(function(){activefeed()}, 300000);
    }});
  }
  activefeed();

  function msgoutput() {
    $.ajax({url: "db/output.log", cache:false, success: function (result) {
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
</script>

<script type="text/javascript">
$(document).ready(function(){getJSONData();});
var OutSpeed2=<?php echo floor($NetOutSpeed[2]) ?>;
var OutSpeed3=<?php echo floor($NetOutSpeed[3]) ?>;
var OutSpeed4=<?php echo floor($NetOutSpeed[4]) ?>;
var OutSpeed5=<?php echo floor($NetOutSpeed[5]) ?>;
var InputSpeed2=<?php echo floor($NetInputSpeed[2]) ?>;
var InputSpeed3=<?php echo floor($NetInputSpeed[3]) ?>;
var InputSpeed4=<?php echo floor($NetInputSpeed[4]) ?>;
var InputSpeed5=<?php echo floor($NetInputSpeed[5]) ?>;
function getJSONData()
{
  setTimeout("getJSONData()", 1000);
  $.getJSON('?act=rt&callback=?', displayData);
}
function ForDight(Dight,How)
{
  if (Dight<0){
    var Last=0+"B/s";
  }else if (Dight<1024){
    var Last=Math.round(Dight*Math.pow(10,How))/Math.pow(10,How)+"B/s";
  }else if (Dight<1048576){
    Dight=Dight/1024;
    var Last=Math.round(Dight*Math.pow(10,How))/Math.pow(10,How)+"KB/s";
  }else{
    Dight=Dight/1048576;
    var Last=Math.round(Dight*Math.pow(10,How))/Math.pow(10,How)+"MB/s";
  }
  return Last;
}
function displayData(dataJSON) {
  $("#NetOut2").html(dataJSON.NetOut2);
  $("#NetOut3").html(dataJSON.NetOut3);
  $("#NetOut4").html(dataJSON.NetOut4);
  $("#NetOut5").html(dataJSON.NetOut5);
  $("#NetOut6").html(dataJSON.NetOut6);
  $("#NetOut7").html(dataJSON.NetOut7);
  $("#NetOut8").html(dataJSON.NetOut8);
  $("#NetOut9").html(dataJSON.NetOut9);
  $("#NetOut10").html(dataJSON.NetOut10);
  $("#NetInput2").html(dataJSON.NetInput2);
  $("#NetInput3").html(dataJSON.NetInput3);
  $("#NetInput4").html(dataJSON.NetInput4);
  $("#NetInput5").html(dataJSON.NetInput5);
  $("#NetInput6").html(dataJSON.NetInput6);
  $("#NetInput7").html(dataJSON.NetInput7);
  $("#NetInput8").html(dataJSON.NetInput8);
  $("#NetInput9").html(dataJSON.NetInput9);
  $("#NetInput10").html(dataJSON.NetInput10);
  $("#NetOutSpeed2").html(ForDight((dataJSON.NetOutSpeed2-OutSpeed2),3)); OutSpeed2=dataJSON.NetOutSpeed2;
  $("#NetOutSpeed3").html(ForDight((dataJSON.NetOutSpeed3-OutSpeed3),3)); OutSpeed3=dataJSON.NetOutSpeed3;
  $("#NetOutSpeed4").html(ForDight((dataJSON.NetOutSpeed4-OutSpeed4),3)); OutSpeed4=dataJSON.NetOutSpeed4;
  $("#NetOutSpeed5").html(ForDight((dataJSON.NetOutSpeed5-OutSpeed5),3)); OutSpeed5=dataJSON.NetOutSpeed5;
  $("#NetInputSpeed2").html(ForDight((dataJSON.NetInputSpeed2-InputSpeed2),3)); InputSpeed2=dataJSON.NetInputSpeed2;
  $("#NetInputSpeed3").html(ForDight((dataJSON.NetInputSpeed3-InputSpeed3),3)); InputSpeed3=dataJSON.NetInputSpeed3;
  $("#NetInputSpeed4").html(ForDight((dataJSON.NetInputSpeed4-InputSpeed4),3)); InputSpeed4=dataJSON.NetInputSpeed4;
  $("#NetInputSpeed5").html(ForDight((dataJSON.NetInputSpeed5-InputSpeed5),3)); InputSpeed5=dataJSON.NetInputSpeed5;
}
</script>

  <!-- HTML5 shim and Respond.js IE8 support of HTML5 elements and media queries -->
  <!--[if lt IE 9]>
  <script src="../lib/html5shiv/html5shiv.js"></script>
  <script src="../lib/respond/respond.src.js"></script>
  <![endif]-->

  <style>
  #sysPre{
    max-height : 600px;
    overflow-y: scroll;
  }
  .legend > table{
    background-color: transparent !important;
    color: #acacac !important;
    font-size: 11px !important;
  }
  </style>

  <style>
  <?php include ('custom/custom.css'); ?>
  </style>

</head>
