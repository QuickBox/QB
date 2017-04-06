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

// Code for the autodl-irssi tab

function fromPixels(s)
{
	if (s === undefined)
		return 0;
	var ary = s.match(/^(\d+).?(\d*)px$/);
	if (!ary)
		return 0;
	return parseInt(ary[1], 10);
}

function jqSetHeight(elem, height)
{
	var extraHeight = elem.outerHeight() - elem.height();
	elem.height(height - extraHeight);
}

function jqSetWidth(elem, width)
{
	var extraWidth = elem.outerWidth() - elem.width();
	elem.width(width - extraWidth);
}

function isScrolledToBottom(elem)
{
	var paddingTop = fromPixels(elem.css("paddingTop"));
	var paddingBottom = fromPixels(elem.css("paddingBottom"));
	var padding = paddingTop + paddingBottom;
	var visibleBottom = elem.scrollTop() + elem.height() + padding;

	// Horiz scrollbars may be visible...
	return visibleBottom >= elem.prop("scrollHeight") - 20;
}

function scrollToBottom(elem)
{
	var scrollHeight = elem.prop("scrollHeight");

	// Horiz scrollbars may be visible...
	elem.scrollTop(scrollHeight + 20);
}

function AutodlIrssiTab(dialogManager, plugin)
{
	var this_ = this;

	this.gettingLines = false;
	this.visible = false;
	this.pluginUrl = plugin.path;

	// For the Restore button
	$(document.body).append($('<iframe name="autodl-restore-iframe" />').css("visibility", "hidden").width(0).height(0));

	// For the Backup button
	var backupIframe = $('<iframe name="autodl-backup-iframe" />').css("visibility", "hidden").width(0).height(0);
	$(document.body).append(backupIframe);

	plugin.attachPageToTabs($(
		'<div id="autodl-irssi-tab">' +
			'<div id="autodl-log-buttons">' +
				'<input type="button" id="autodl-log-clear-button" class="Button" value="' + theUILang.ClearButton + '" title="' + theUILang.autodlClear2  + '" />' +
				'<input type="button" id="autodl-log-update-button" class="Button" value="' + theUILang.autodlUpdate1 + '" title="' + theUILang.autodlUpdate2  + '" />' +
				'<input type="button" id="autodl-log-whatsnew-button" class="Button" value="' + theUILang.autodlWhatsNew1 + '" title="' + theUILang.autodlWhatsNew2  + '" />' +
				'<input type="button" id="autodl-log-version-button" class="Button" value="' + theUILang.autodlVersion1 + '" title="' + theUILang.autodlVersion2  + '" />' +
				'<input type="button" id="autodl-log-reload-trackers-button" class="Button" value="' + theUILang.autodlReloadTrackers1 + '" title="' + theUILang.autodlReloadTrackers2  + '" />' +
				'<input type="button" id="autodl-log-backup-button" class="Button" value="' + theUILang.autodlBackup1 + '" title="' + theUILang.autodlBackup2  + '" />' +
				'<form target="autodl-restore-iframe" id="autodl-restore" method="POST" action="plugins/autodl-irssi/writeconfig.php" enctype="multipart/form-data">' +
					'<input type="hidden" name="restoring" value="1" />' +
					'<input type="submit" id="autodl-log-restore-button" class="Button" value="' + theUILang.autodlRestore1 + '" title="' + theUILang.autodlRestore2  + '" />' +
					'<input id="autodl-restore-file" type="file" name="file" title="' + theUILang.autodlRestore3  + '" />' +
				'</form>' +
			'</div>' +
			'<div id="autodl-irssi-log" class="autodl-fg-default autodl-bg-default">' +
				'<table>' +
					'<tbody id="autodl-log-tbody"/>' +
				'</table>' +
			'</div>' +
		'</div>'
	).get(0), "autodl-irssi", "lcont");

	$("#autodl-restore").submit(function()
	{
		if ($("#autodl-restore-file").val() == "")
		{
			alert(theUILang.autodlMissingFilename)
			return false;
		}
		dialogManager.clearConfigFileCache();
		return true;
	});
	$("#autodl-log-backup-button").click(function()
	{
		backupIframe.attr("src", "plugins/autodl-irssi/getfile.php?file=autodl.cfg");
	});
	$("#autodl-log-update-button").click(function()
	{
		this_._sendAutodlCommand("update");
	});
	$("#autodl-log-whatsnew-button").click(function()
	{
		this_._sendAutodlCommand("whatsnew");
	});
	$("#autodl-log-version-button").click(function()
	{
		this_._sendAutodlCommand("version");
	});
	$("#autodl-log-reload-trackers-button").click(function()
	{
		this_._sendAutodlCommand("reloadtrackers");
	});

	this._initResizeBottom();
	this._initOnShow();

	$("#autodl-log-clear-button").click(function(e)
	{
		$("#autodl-log-tbody").empty();
	});

	setInterval(function() { this_._getNewLines() }, this.CHECK_NEW_LINES_EVERY_SECS * 1000);
	this._getNewLines();
}

// Max number of visible lines
AutodlIrssiTab.prototype.MAX_AUTODL_IRSSI_LINES = 500;

// Start removing lines when it reaches this limit
AutodlIrssiTab.prototype.REMOVE_LINES_LIMIT = AutodlIrssiTab.prototype.MAX_AUTODL_IRSSI_LINES + 10;

// How often we'll get new autodl-irssi output lines from the server
AutodlIrssiTab.prototype.CHECK_NEW_LINES_EVERY_SECS = 1;

AutodlIrssiTab.prototype._sendAutodlCommand =
function(subcmd)
{
	try
	{
		function onComplete(errorMessage)
		{
			if (errorMessage)
				log("Command failed: " + errorMessage);
		}

		$.ajax(
		{
			url: this.pluginUrl + "command.php?type=autodl&arg1=" + subcmd,
			type: "POST",
			dataType: "json",
			success: function(data, status) { onComplete(data.error) },
			error: function(xhr, status, ex)
			{
				onComplete(getAjaxErrorString(status, ex));
			}
		});
	}
	catch (ex)
	{
		log("AutodlIrssiTab::_sendAutodlCommand: " + formatException(ex));
	}
}

AutodlIrssiTab.prototype._initResizeBottom =
function()
{
	var this_ = this;

	this.oldResizeBottom = theWebUI.resizeBottom;
	theWebUI.resizeBottom = function(width, height)
	{
		this_._onResizeBottom(width, height);
	};

	this._onResize();
}

AutodlIrssiTab.prototype._initOnShow =
function()
{
	var this_ = this;

	this.oldOnShow = theTabs.onShow;
	theTabs.onShow = function(id)
	{
		this_.onShow(id);
	};
}

AutodlIrssiTab.prototype._getNewLines =
function()
{
	if (this.gettingLines)
		return;

	try
	{
		var this_ = this;
		this.gettingLines = true;
		autodl_ajax(
		{
			url: this.pluginUrl + "getlines.php?cid=" + (this.cid != null ? this.cid : ""),
			type: "GET",
			dataType: "json",
			success: function(data, status) { this_._onGetLines(data); },
			error: function(xhr, status, ex)
			{
				this_.gettingLines = false;
				// Ignore errors
			}
		});
	}
	catch (ex)
	{
		this.gettingLines = false;
		log("AutodlIrssiTab._getNewLines: " + formatException(ex));
	}
}

AutodlIrssiTab.prototype._onGetLines =
function(data)
{
	try
	{
		this.gettingLines = false;
		if (data.error)
			return;	// Ignore errors

		this.cid = data.cid;
		var lines = data.lines;
		if (lines.length === 0)
			return;

		var elem = $("#autodl-irssi-log");
		var wasScrolledDown = isScrolledToBottom(elem);

		for (var i = 0; i < lines.length; i++)
		{
			var info = lines[i];
			this._addLine(new Date(info.time * 1000), info.line);
		}

		if (wasScrolledDown)
			scrollToBottom(elem);
	}
	catch (ex)
	{
		log("AutodlIrssiTab::_onGetLines: " + formatException(ex));
	}
}

AutodlIrssiTab.prototype._resetSizeChrome =
function()
{
	// Fix for Google Chrome. Without it Chrome will add horiz and vertical scroll bars when we
	// make the tabs area smaller.
	$("#autodl-irssi-log").width(0).height(0);
}

AutodlIrssiTab.prototype._onResizeBottom =
function(width, height)
{
	this._resetSizeChrome();
	this.oldResizeBottom.call(theWebUI, width, height);
	this._onResize();
}

AutodlIrssiTab.prototype.onShow =
function(id)
{
	if (id !== "autodl-irssi-tab")
	{
		this.visible = false;
		return this.oldOnShow(id);
	}

	this._resetSizeChrome();
	this.visible = true;
	this._onResize();
	scrollToBottom($("#autodl-irssi-log"));
}

AutodlIrssiTab.prototype._onResize =
function()
{
	if (!this.visible)
		return;

	var width = fromPixels($("#tdetails").css("width"));
	width -= 8;
	var height = fromPixels($("#tdetails").css("height"));
	height -= ($("#tabbar").height());
	$("#tdcont").height( height );
	height -= 2;

	var logElem = $("#autodl-irssi-log");
	if (width != null)
		jqSetWidth(logElem, width);
	if (height != null)
		jqSetHeight(logElem, height - $("#autodl-log-buttons").outerHeight());
}

function getMircColorInfo(s)
{
	var rv = [];

	var attrs =
	{
		bold: false,
		underline: false,
		fg: -1,
		bg: -2,
		s: ""
	};
	var oldAttrs = {};

	function attrsChanged(attrs1, attrs2)
	{
		return attrs1.bold !== attrs2.bold ||
			   attrs1.underline !== attrs2.underline ||
			   attrs1.fg !== attrs2.fg ||
			   attrs1.bg !== attrs2.bg;
	}

	for (var i = 0; i < s.length; i++)
	{
		$.extend(oldAttrs, attrs);
		var code = s.charCodeAt(i);
		if (code >= 0x20)
		{
			attrs.s += s.charAt(i);
		}
		else if (code === 2)
		{
			// Bold text
			attrs.bold = !attrs.bold;
		}
		else if (code === 3)
		{
			// Change fg/bg: \x03, \x03FF?, \x03FF?,BB?

			var s2 = s.substr(i+1);
			var ary = s2.match(/^(?:(\d\d?)(?:,(\d\d?))?)?/);
			attrs.fg = -1;
			attrs.bg = -2;
			if (ary[1] != null && ary[1] !== "")
			{
				attrs.fg = parseInt(ary[1], 10) % 16;
				i += ary[1].length;
				if (ary[2] != null && ary[2] !== "")
				{
					attrs.bg = parseInt(ary[2], 10) % 16;
					i += 1 + ary[2].length;	// Don't forget the comma
				}
			}
		}
		else if (code === 15)
		{
			// Revert back to normal text
			attrs.bold = false;
			attrs.underline = false;
			attrs.fg = -1;
			attrs.bg = -2;
		}
		else if (code === 22)
		{
			// Reverse
			var tmp = attrs.fg;
			attrs.fg = attrs.bg;
			attrs.bg = tmp;
		}
		else if (code === 31)
		{
			// Underline
			attrs.underline = !attrs.underline;
		}
		else if (code === 13)
			;	// Ignore it
		else
		{
			attrs.s += s.charAt(i);
		}

		if (attrsChanged(oldAttrs, attrs))
		{
			if (attrs.s !== "")
				rv.push($.extend({}, oldAttrs));
			attrs.s = "";
		}
	}
	if (attrs.s !== "")
		rv.push($.extend({}, attrs));

	return rv;
}

AutodlIrssiTab.prototype._getTimeString =
function(time)
{
	function f2(v)
	{
		return ("0" + v).substr(-2);
	}

	return f2(time.getHours()) + ":" + f2(time.getMinutes()) + ":" + f2(time.getSeconds());
}

AutodlIrssiTab.prototype._addLine =
function(time, line)
{
	var lineElem = $('<div class="autodl-line"/>');
	var infos = getMircColorInfo(line);
	for (var i = 0; i < infos.length; i++)
	{
		var info = infos[i];

		var lines = info.s.split("\n");
		for (var j = 0; j < lines.length; j++)
		{
			if (j !== 0)
				lineElem.append($("<br/>"));

			var elem = $('<span/>').text(lines[j]);
			lineElem.append(elem);
			if (info.bold)
				elem.addClass("autodl-bold");
			if (info.underline)
				elem.addClass("autodl-underline");

			if (info.fg === -1)
				;	// Nothing, inherit from parent
			else if (info.fg === -2)
				elem.addClass("autodl-bg-as-fg");
			else
				elem.addClass("autodl-fg-" + info.fg);

			if (info.bg === -2)
				;	// Nothing, inherit from parent
			else if (info.bg === -1)
				elem.addClass("autodl-fg-as-bg");
			else
				elem.addClass("autodl-bg-" + info.bg);
		}
	}

	var tr = $('<tr/>').attr("title", time.toLocaleString())
			.append($('<td class="autodl-line-time"/>').text(this._getTimeString(time)))
			.append($('<td class="autodl-line-sep"/>').text("-!-"))
			.append($('<td class="autodl-line-text"/>').append(lineElem));

	var tbody = $("#autodl-log-tbody");
	tbody.append(tr);

	if (tbody.children().length >= this.REMOVE_LINES_LIMIT)
		tbody.children(":lt(" + (tbody.children().length - this.MAX_AUTODL_IRSSI_LINES) + ")").remove();
}
