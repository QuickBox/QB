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

function Servers()
{
}

Servers.prototype.createDialogBox =
function(multiSelectDlgBox, okHandler)
{
	theDialogManager.make("autodl-servers", theUILang.autodlServers,
		'<div id="autodl-servers">' +
			'<div>' +
				'<p>' + theUILang.autodlCantEditHere + '</p>' +
			'</div>' +
			'<div id="autodl-servers-left">' +
				'<div id="autodl-servers-list" />' +
			'</div>' +
			'<div id="autodl-servers-right">' +
				'<table>' +
					'<tbody>' +
						'<tr>' +
							'<td><label for="autodl-servers-network">' + theUILang.autodlNetwork + '</label></td>' +
							'<td><input type="text" class="textbox-18" id="autodl-servers-network" /></td>' +
						'</tr>' +
						'<tr>' +
							'<td><label for="autodl-servers-servers">' + theUILang.autodlServer + '</label></td>' +
							'<td><input type="text" class="textbox-18" id="autodl-servers-servers" /></td>' +
						'</tr>' +
						'<tr>' +
							'<td><label for="autodl-servers-channels">' + theUILang.autodlChannels + '</label></td>' +
							'<td><input type="text" class="textbox-18" id="autodl-servers-channels" /></td>' +
						'</tr>' +
						'<tr>' +
							'<td><label for="autodl-servers-announcers">' + theUILang.autodlAnnouncer + '</label></td>' +
							'<td><input type="text" class="textbox-18" id="autodl-servers-announcers" /></td>' +
						'</tr>' +
					'</tbody>' +
				'</table>' +
			'</div>' +
			'<div class="aright buttons-list dialog-buttons">' +
				'<input type="button" id="autodl-servers-close-button" value="' + theUILang.autodlClose + '" class="Button" />' +
			'</div>' +
		'</div>'
	);

	var this_ = this;

	this.trackersId = null;

	this.trackerListBox = new ListBox("autodl-servers-list");
	this.trackerListBox.onSelected = function(oldObj, newObj) { this_._onTrackerSelected(oldObj, newObj); }

	$("#autodl-servers-close-button").click(function(e) { okHandler() });
}

Servers.prototype.onOkClicked =
function()
{
	return false;	// autodl.cfg NOT updated
}

Servers.prototype._getSortedTrackerInfos =
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

Servers.prototype.onBeforeShow =
function(configFile, trackerInfos, trackersId)
{
	if (this.trackersId !== trackersId)
	{
		trackerInfos = this._getSortedTrackerInfos(trackerInfos);
		this._initListbox(trackerInfos);
		this.trackerListBox.select(0);
		this.trackersId = trackersId;
	}
}

Servers.prototype.onAfterHide =
function()
{
}

Servers.prototype._initListbox =
function(trackerInfos)
{
	this.trackerListBox.removeAll();

	for (var i = 0; i < trackerInfos.length; i++)
	{
		var trackerInfo = trackerInfos[i];

		for (var j = 0; j < trackerInfo.servers.length; j++)
		{
			var server = trackerInfo.servers[j];

			var obj =
			{
				server: server,
				trackerInfo: trackerInfo
			};
			obj.labelElem = $('<label />').text(trackerInfo.longName)[0];

			this.trackerListBox.append(obj.labelElem, obj);
		}
	}
}

Servers.prototype._onTrackerSelected =
function(oldObj, newObj)
{
	function setIt(obj, id, propName)
	{
		var server = (obj || {}).server || {};
		$("#" + id).val(server[propName] || "");
	}
	setIt(newObj, "autodl-servers-network", "network");
	setIt(newObj, "autodl-servers-servers", "serverNames");
	setIt(newObj, "autodl-servers-channels", "channelNames");
	setIt(newObj, "autodl-servers-announcers", "announcerNames");
}
