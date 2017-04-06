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

function Trackers()
{
}

Trackers.prototype.createDialogBox =
function(multiSelectDlgBox, okHandler)
{
	theDialogManager.make("autodl-trackers", theUILang.autodlTrackers,
		'<div id="autodl-trackers">' +
			'<div id="autodl-trackers-left">' +
				'<div id="autodl-trackers-list" />' +
			'</div>' +
			'<div id="autodl-trackers-right" />' +
			'<div class="aright buttons-list dialog-buttons">' +
				'<input type="button" id="autodl-trackers-ok-button" value="' + theUILang.ok + '" class="OK Button" />' +
				'<input type="button" value="' + theUILang.Cancel + '" class="Cancel Button" />' +
			'</div>' +
		'</div>'
	);

	var this_ = this;

	this.trackerListBox = new ListBox("autodl-trackers-list");
	this.trackerListBox.onSelected = function(oldObj, newObj) { this_._onTrackerSelected(oldObj, newObj); }

	$("#autodl-trackers-ok-button").click(function(e) { okHandler() });
}

Trackers.prototype._getSortedTrackerInfos =
function(trackerInfos)
{
	var ary = [];

	for (var i = 0; i < trackerInfos.length; i++)
		ary[i] = trackerInfos[i];
	ary.sort(function(a, b)
	{
		return stringCompare(a.longName.toLowerCase(), b.longName.toLowerCase());
	});

	return ary;
}

Trackers.prototype.onBeforeShow =
function(configFile, trackerInfos, trackersId)
{
	this.configFile = configFile;

	if (trackersId !== this.trackersId)
	{
		this.trackerInfos = this._getSortedTrackerInfos(trackerInfos);
		this._createListbox();
		this._createContents();
		this._createOptions();
		this.trackerListBox.select(0);
		this.trackersId = trackersId;
	}

	this._initDialog();
}

Trackers.prototype.onAfterHide =
function()
{
	this.configFile = null;
}

Trackers.prototype.onOkClicked =
function()
{
	this._saveTrackers();

	return true;	// autodl.cfg updated
}

Trackers.prototype._saveTrackers =
function()
{
	for (var i = 0; i < this.trackerInfos.length; i++)
	{
		var trackerInfo = this.trackerInfos[i];

		var section = this.configFile.getSection("tracker", trackerInfo.type);
		section.dontPrintEmpty();

		var options = this.trackerOptions[trackerInfo.type];
		saveDialogOptions(section, options);
	}
}

Trackers.prototype._initDialog =
function()
{
	for (var i = 0; i < this.trackerInfos.length; i++)
	{
		var trackerInfo = this.trackerInfos[i];

		var section = this.configFile.getSection("tracker", trackerInfo.type);
		var options = this.trackerOptions[trackerInfo.type];
		initDialogOptions(section, options);
	}
}

Trackers.prototype._createOptions =
function()
{
	this.trackerOptions = {};
	for (var i = 0; i < this.trackerInfos.length; i++)
	{
		var trackerInfo = this.trackerInfos[i];

		var options = this.trackerOptions[trackerInfo.type] = [];
		var settings = trackerInfo.settings;
		for (var j = 0; j < settings.length; j++)
		{
			var setting = settings[j];
			var id = this._settingIdFromName(trackerInfo, setting.name);
			switch (setting.type)
			{
			case "bool":
				options.push(new DialogOptionBool(id, setting.name, setting.defaultValue));
				break;

			case "integer":
				options.push(new DialogOptionInt(id, setting.name, setting.defaultValue));
				break;

			case "textbox":
				options.push(new DialogOptionText(id, setting.name, setting.defaultValue));
				break;

			case "description":
				break;

			default:
				log("Unknown type: " + setting.type);
				break;
			}
		}
	}
}

Trackers.prototype._createListbox =
function()
{
	this.trackerListBox.removeAll();
	for (var i = 0; i < this.trackerInfos.length; i++)
	{
		var trackerInfo = this.trackerInfos[i];

		var obj =
		{
			trackerInfo: trackerInfo
		};
		var checkboxId = this._settingIdFromName(trackerInfo, "enabled");
		obj.checkboxElem = $('<input type="checkbox" />').attr("id", checkboxId)[0];
		obj.labelElem = $('<label />').text(trackerInfo.longName)[0];

		this.trackerListBox.append($(obj.checkboxElem).add(obj.labelElem), obj);
	}
}

Trackers.prototype._createContents =
function()
{
	$("#autodl-trackers-right").empty();
	for (var i = 0; i < this.trackerInfos.length; i++)
	{
		var elem = this._createTrackerContent(this.trackerInfos[i]);
		elem.css("display", "none");
		$("#autodl-trackers-right").append(elem);
	}
}

Trackers.prototype._id =
function(trackerInfo)
{
	var type = trackerInfo.type.replace(/[^\w\-]/, "_");
	return "autodl-trackers-content-" + type;
}

Trackers.prototype._settingIdFromName =
function(trackerInfo, name)
{
	return this._id(trackerInfo) + "-" + name;
}

Trackers.prototype._onTrackerSelected =
function(oldObj, newObj)
{
	if (oldObj)
		$("#" + this._id(oldObj.trackerInfo)).css("display", "none");
	if (newObj)
		$("#" + this._id(newObj.trackerInfo)).css("display", "block");
}

Trackers.prototype._createTrackerContent =
function(trackerInfo)
{
	var div = $("<div />").attr("id", this._id(trackerInfo));

	var settings = trackerInfo.settings;
	var tbody = null;
	for (var i = 0; i < settings.length; i++)
	{
		var setting = settings[i];

		// This is the listbox item checkbox so no need to display it here again
		if (setting.name === "enabled")
			continue;

		var elem = this._createTrackerSettingsElem(setting, trackerInfo);

		if (setting.type === "textbox" || setting.type === "integer")
		{
			elem = $('<tr />').append($('<td />').append(elem[0]))
							.append($('<td />').append(elem[1]));

			if (tbody == null)
			{
				tbody = $('<tbody />');
				div.append($('<br /><table />').append(tbody));
			}
			tbody.append(elem);
		}
		else
		{
			elem = $('<div />').append(elem);
			div.append(elem);
		}
	}

	return div;
}

Trackers.prototype._createTrackerSettingsElem =
function(setting, trackerInfo)
{
	var id = this._settingIdFromName(trackerInfo, setting.name);
	var tooltipText = setting.tooltiptext;

	switch (setting.type)
	{
	case "bool":
		var checkbox = $('<input type="checkbox" />').attr("id", id).attr("title", tooltipText);
		var label = $('<label />').attr("for", id).text(setting.text).attr("title", tooltipText);
		return checkbox.add(label);

	case "textbox":
	case "integer":
		var label = $('<label />').attr("for", id).text(setting.text);
		var textbox = $('<input type="text" class="textbox-22" />')
							.attr("id", id)
							.attr("title", tooltipText)
							.attr("placeholder", setting.placeholder || "");
		if (setting.pasteRegex && setting.pasteGroup)
		{
			var this_ = this;
			textbox.change(function(e)
			{
				this_._onPaste(trackerInfo, setting.pasteGroup, textbox);
			});
			textbox.keyup(function(e)
			{
				this_._onPaste(trackerInfo, setting.pasteGroup, textbox);
			});
			textbox.on('input', function(e)
			{
				this_._onPaste(trackerInfo, setting.pasteGroup, textbox);
			});
		}
		return label.add(textbox);

	case "description":
		return this._addHtmlLinks(setting.text);

	default:
		log("Unknown tracker setting: " + setting.type);
		return $();
	}
}

Trackers.prototype._addHtmlLinks =
function(text)
{
	// Fix for IE8, it doesn't support using parens in split to extract the links.
	var foundLink = false;
	var ary = text.split(/((?:javascript|https?)\S+)/);
	var res = "";
	for (var i = 0; i < ary.length; i++)
	{
		var s = ary[i];
		if (s.match(/^(?:javascript|https?)/))
		{
			foundLink = true;
			res += '<a href="' + s + '" target="_blank">' + s + '</a>';
		}
		else
			res += s;
	}
	if (!foundLink)
		res = text;
	res = "<p>" + res + "</p>";
	return $(res);
}

Trackers.prototype._findSetting =
function(trackerInfo, name)
{
	for (var i = 0; i < trackerInfo.settings.length; i++)
	{
		var setting = trackerInfo.settings[i];
		if (setting.name === name)
			return setting;
	}

	return null;
}

Trackers.prototype._onPaste =
function(trackerInfo, pasteGroup, textboxElem)
{
	var s = textboxElem.val();
	var names = pasteGroup.split(",");
	for (var i = 0; i < names.length; i++)
	{
		var name = $.trim(names[i]);
		var setting = this._findSetting(trackerInfo, name);
		if (!setting)
			continue;

		var textbox = $("#" + this._settingIdFromName(trackerInfo, name));
		var ary = s.match(setting.pasteRegex);
		if (textbox.length > 0 && ary && ary.length > 1)
			textbox.val(ary[1]);
	}
}
