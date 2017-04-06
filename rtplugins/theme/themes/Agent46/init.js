/*
 *  Agent46 skin and scripts for ruTorrent
 *  Author: Mahngiel
 */ 

// Color graphs
plugin.Agent46AllDone = plugin.allDone;
plugin.allDone = function()
{
	plugin.Agent46AllDone.call(this);
	$.each(["diskspace","quotaspace","cpuload"], function(ndx,name)
	{
		var plg = thePlugins.get(name);
		if(plg && plg.enabled)
		{
			plg.prgStartColor = new RGBackground("#ccf0ff");
			plg.prgEndColor = new RGBackground("#07B4FB");
		}
	});
}

// Color download progress
plugin.oldTableCreate = dxSTable.prototype.create;
dxSTable.prototype.create = function(ele, styles, aName)
{
	plugin.oldTableCreate.call(this, ele, styles, aName);
	this.prgStartColor = new RGBackground("#ccf0ff");
	this.prgEndColor = new RGBackground("#07B4FB");
}

// add entry into About
var Mahngiel = "<strong><a href=\"http://codezyne.me\">Theme development by Mahngiel</a></strong><br /><br />";
var target = $('#dlgAbout').children('.content').children('center');
$(Mahngiel).insertBefore(target);

// Remove parentheses
jQuery(function() {
	setTimeout(function() {
		$('#CatList li').each(function() {
			$(this).html( $(this).html().replace(/\<scr(.*)ipt>/g, '').replace(/\(<span/g, '<span').replace(/span>\)/g, 'span>') );
		});	
	}, 500);
});