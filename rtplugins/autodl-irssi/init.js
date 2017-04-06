/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is IRC Auto Downloader.
 *
 * The Initial Developer of the Original Code is
 * David Nilsson.
 * Portions created by the Initial Developer are Copyright (C) 2010, 2011
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * ***** END LICENSE BLOCK ***** */

window.formatException = function(ex)
{
	try
	{
		if (ex == null)
			return "";

		var msg = "";

		function addProp(name)
		{
			if (ex[name] != null)
				msg += name + " = " + ex[name] + "; ";
		}

		addProp("name");
		addProp("message");
		addProp("fileName");
		addProp("lineNumber");

		if (msg === "")
			msg = ex.toString();

		return "Exception info: " + msg;
	}
	catch (ex)
	{
		return "ex: " + ex;
	}
}

plugin.loadJavaScriptFiles =
function(aryFiles, func)
{
	var numFiles = aryFiles.length;
	if (!numFiles)
		return func();
	for (var i = 0; i < aryFiles.length; i++)
	{
		injectScript(this.path + 'js/' + aryFiles[i], function()
		{
			if (--numFiles === 0)
				func();
		});
	}
}

// Returns the theme name.
plugin.getThemeName =
function()
{
	var themeName = "Standard";
	var theme = thePlugins.get("theme");
	if (theme)
	{
		// Use the path since it's never localized
		var ary = theme.path.match(/themes\/([^\/]+)\//);
		if (ary && ary[1])
			themeName = ary[1];
	}
	return themeName.toLowerCase();
}

// Initialize the plugin (load CSS and JS files). Continues in onLangLoaded()
plugin.__init =
function()
{
	var this_ = this;

	this.loadCSS("css/" + this.getThemeName() + ".min");

	// Note: These files aren't necessarily loaded in order! They must not rely on other JS files.
	this.loadJavaScriptFiles(
	[
		'ListBox.js',
		'Tabs.js',
		'DialogUtils.js',
		'MultiSelect.js',
		'UploadMethod.js',
		'Preferences.js',
		'Filters.js',
		'Trackers.js',
		'Servers.js',
		'IrcServers.js',
		'Help.js',
		'AutodlFilesDownloader.js',
		'ConfigFileParser.js',
		'TrackerInfo.js',
		'AutodlIrssiTab.js',
		'MyDialogManager.js'
	], function()
	{
		this_.loadLang(true);
	});
}

// Called when all of our files have been loaded
plugin.onLangLoaded =
function()
{
	this.__init2();
}

// All files have been loaded. Now initialize the rest.
plugin.__init2 =
function()
{
	try
	{
		var this_ = this;
		this.dialogManager = new MyDialogManager(this.path);
		this._addToToolbar();
		this.autodlIrssiTab = new AutodlIrssiTab(this.dialogManager, this);
	}
	catch (ex)
	{
		log("autodl-irssi: __init2: ex: " + formatException(ex));
		throw ex;
	}
}

plugin._addToToolbar =
function()
{
	var beforeId = "settings";
	this.addButtonToToolbar("autodl-tb", "autodl-irssi", "", beforeId);
	this.addSeparatorToToolbar(beforeId);

	var this_ = this;
	$("#mnu_autodl-tb").click(function(e)
	{
		this_._onClickToolbarButton(e);
	}).
	mouseup(function(e)
	{
		// Prevent ruTorrent from closing the popup menu
		e.stopPropagation();
	});
}

plugin._onClickToolbarButton =
function(e)
{
	var this_ = this;

	theContextMenu.clear();
	theContextMenu.add([theUILang.autodlFilters2, function() { this_.dialogManager.toggleDialog("filters"); }]);
	theContextMenu.add([theUILang.autodlIrcServers2, function() { this_.dialogManager.toggleDialog("ircsrvs"); }]);
	theContextMenu.add([theUILang.autodlTrackers2, function() { this_.dialogManager.toggleDialog("trackers"); }]);
	theContextMenu.add([theUILang.autodlPreferences2, function() { this_.dialogManager.toggleDialog("preferences"); }]);
	theContextMenu.add([CMENU_SEP]);
	theContextMenu.add([theUILang.autodlHelp2, function() { this_.dialogManager.toggleDialog("help"); }]);

	var offset = $("#autodl-tb").offset();
	var x = offset.left - 5;
	var y = offset.top + 5 + $("#autodl-tb").height();
	theContextMenu.show(x, y);
}

if (plugin.enabled)
	plugin.__init();
