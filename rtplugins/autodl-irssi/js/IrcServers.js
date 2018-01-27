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

function IrcServers()
{
}

IrcServers.prototype.createDialogBox =
function(multiSelectDlgBox, okHandler)
{
	theDialogManager.make("autodl-ircsrvs", theUILang.autodlIrcServers,
		'<div id="autodl-ircsrvs">' +
			'<div id="autodl-ircsrvs-left">' +
				'<div id="autodl-ircsrvs-list" />' +
				'<div id="autodl-ircsrvs-list-buttons" align="center">' +
					'<div>' +
						'<input type="button" class="Button" id="autodl-ircsrvs-new-button" value="' + theUILang.autodlNew + '" />' +
						'<input type="button" class="Button" id="autodl-ircsrvs-remove-button" value="' + theUILang.autodlRemove + '" />' +
					'</div>' +
					'<div>' +
						'<input type="button" class="Button" id="autodl-ircsrvs-announce-channels-button" value="' + theUILang.autodlServers2 + '" />' +
					'</div>' +
				'</div>' +
			'</div>' +
			'<div id="autodl-ircsrvs-right">' +
				'<fieldset id="autodl-ircsrvs-servers-fieldset">' +
					'<legend>' + theUILang.autodlIrcsrvs1 + '</legend>' +
					'<div>' +
						'<label for="autodl-ircsrvs-server">' + theUILang.autodlIrcsrvs2 + '</label>' +
						'<input type="text" class="textbox-12" id="autodl-ircsrvs-server" title="' + theUILang.autodlIrcsrvs3 + '" placeholder="' + theUILang.autodlIrcsrvs4 + '"/>' +
						'<label for="autodl-ircsrvs-port">' + theUILang.autodlIrcsrvs5 + '</label>' +
						'<input type="text" class="textbox-5" id="autodl-ircsrvs-port" title="' + theUILang.autodlIrcsrvs6 + '" placeholder="' + theUILang.autodlIrcsrvs7 + '"/>' +
						'<input type="checkbox" id="autodl-ircsrvs-ssl" title="' + theUILang.autodlIrcsrvs8 + '"/>' +
						'<label for="autodl-ircsrvs-ssl" title="' + theUILang.autodlIrcsrvs9 + '">' + theUILang.autodlIrcsrvs10 + '</label>' +
					'</div>' +
					'<div>' +
						'<label for="autodl-ircsrvs-nick">' + theUILang.autodlIrcsrvs11 + '</label>' +
						'<input type="text" class="textbox-5" id="autodl-ircsrvs-nick" title="' + theUILang.autodlIrcsrvs12 + '"/>' +
						'<label for="autodl-ircsrvs-identpass">' + theUILang.autodlPassword + '</label>' +
						'<input type="password" class="textbox-5" id="autodl-ircsrvs-identpass" title="' + theUILang.autodlIrcsrvs14 + '"/>' +
						'<label for="autodl-ircsrvs-identemail">' + theUILang.autodlIrcsrvs15 + '</label>' +
						'<input type="text" class="textbox-7" id="autodl-ircsrvs-identemail" title="' + theUILang.autodlIrcsrvs16 + '"/>' +
					'</div>' +
					'<div>' +
						'<label for="autodl-ircsrvs-server-password">' + theUILang.autodlIrcsrvs32 + '</label>' +
						'<input type="text" class="textbox-12" id="autodl-ircsrvs-server-password" title="' + theUILang.autodlIrcsrvs33 + '"/>' +
						'<input type="checkbox" id="autodl-ircsrvs-bnc" title="' + theUILang.autodlIrcsrvs34 + '"/>' +
						'<label for="autodl-ircsrvs-bnc" title="' + theUILang.autodlIrcsrvs35 + '">' + theUILang.autodlIrcsrvs36 + '</label>' +
					'</div>' +
				'</fieldset>' +
				'<fieldset id="autodl-ircsrvs-channels-fieldset">' +
					'<legend>' + theUILang.autodlIrcsrvs17 + '</legend>' +
					'<div>' +
						'<select id="autodl-ircsrvs-channels"/>' +
						'<input type="button" class="Button" id="autodl-ircsrvs-new-channel-button" value="' + theUILang.autodlNew + '" />' +
						'<input type="button" class="Button" id="autodl-ircsrvs-remove-channel-button" value="' + theUILang.autodlRemove + '" />' +
					'</div>' +
					'<div>' +
						'<label for="autodl-ircsrvs-channel">' + theUILang.autodlIrcsrvs18 + '</label>' +
						'<input type="text" class="textbox-11" id="autodl-ircsrvs-channel" title="' + theUILang.autodlIrcsrvs19 + '" placeholder="' + theUILang.autodlIrcsrvs20 + '"/>' +
						'<label for="autodl-ircsrvs-channelpass">' + theUILang.autodlPassword + '</label>' +
						'<input type="password" class="textbox-8" id="autodl-ircsrvs-channelpass" title="' + theUILang.autodlIrcsrvs22 + '"/>' +
					'</div>' +
					'<table>' +
						'<tbody>' +
							'<tr>' +
								'<td><label for="autodl-ircsrvs-chaninvite">' + theUILang.autodlIrcsrvs23 + '</label></td>' +
								'<td><input type="text" class="textbox-20" id="autodl-ircsrvs-chaninvite" title="' + theUILang.autodlIrcsrvs24 + '" placeholder="' + theUILang.autodlIrcsrvs25 + '"/></td>' +
							'</tr>' +
							'<tr>' +
								'<td><label for="autodl-ircsrvs-httpurl">' + theUILang.autodlIrcsrvs26 + '</label></td>' +
								'<td><input type="text" class="textbox-20" id="autodl-ircsrvs-httpurl" title="' + theUILang.autodlIrcsrvs27 + '"/></td>' +
							'</tr>' +
							'<tr>' +
								'<td><label for="autodl-ircsrvs-httpheader">' + theUILang.autodlIrcsrvs28 + '</label></td>' +
								'<td><input type="text" class="textbox-20" id="autodl-ircsrvs-httpheader" title="' + theUILang.autodlIrcsrvs29 + '"/></td>' +
							'</tr>' +
							'<tr>' +
								'<td><label for="autodl-ircsrvs-httpdata">' + theUILang.autodlIrcsrvs30 + '</label></td>' +
								'<td><input type="text" class="textbox-20" id="autodl-ircsrvs-httpdata" title="' + theUILang.autodlIrcsrvs31 + '"/></td>' +
							'</tr>' +
						'</tbody>' +
					'</table>' +
				'</fieldset>' +
			'</div>' +
			'<div class="aright buttons-list dialog-buttons">' +
				'<input type="button" id="autodl-ircsrvs-ok-button" value="' + theUILang.ok + '" class="OK Button" />' +
				'<input type="button" value="' + theUILang.Cancel + '" class="Cancel Button" />' +
			'</div>' +
		'</div>'
	);

	var this_ = this;

	this.serverOptions =
	[
		new DialogOptionText("autodl-ircsrvs-port", "port", ""),
		new DialogOptionBool("autodl-ircsrvs-ssl", "ssl", "false"),
		new DialogOptionText("autodl-ircsrvs-nick", "nick", ""),
		new DialogOptionText("autodl-ircsrvs-identpass", "ident-password", ""),
		new DialogOptionText("autodl-ircsrvs-identemail", "ident-email", ""),
		new DialogOptionText("autodl-ircsrvs-server-password", "server-password", ""),
		new DialogOptionBool("autodl-ircsrvs-bnc", "bnc", "false")
	];
	this.channelOptions =
	[
		new DialogOptionText("autodl-ircsrvs-channelpass", "password", ""),
		new DialogOptionText("autodl-ircsrvs-chaninvite", "invite-command", ""),
		new DialogOptionText("autodl-ircsrvs-httpurl", "invite-http-url", ""),
		new DialogOptionText("autodl-ircsrvs-httpheader", "invite-http-header", ""),
		new DialogOptionText("autodl-ircsrvs-httpdata", "invite-http-data", "")
	];

	$("#autodl-ircsrvs-new-button").click(function()
	{
		this_._onNewServerClicked();
	});
	$("#autodl-ircsrvs-remove-button").click(function()
	{
		this_._onRemoveServerClicked();
	});
	$("#autodl-ircsrvs-announce-channels-button").click(function()
	{
		theDialogManager.show("autodl-servers");
	});
	$("#autodl-ircsrvs-new-channel-button").click(function()
	{
		this_._onNewChannelClicked();
	});
	$("#autodl-ircsrvs-remove-channel-button").click(function()
	{
		this_._onRemoveChannelClicked();
	});
	$("#autodl-ircsrvs-server").keyup(function(e)
	{
		this_._onServerNameModified();
	});
	$("#autodl-ircsrvs-channel").keyup(function(e)
	{
		this_._onChannelNameModified();
	});

	this.serversListBox = new ListBox("autodl-ircsrvs-list");
	this.serversListBox.onSelected = function(oldObj, newObj) { this_._onServerSelected(oldObj, newObj); }

	this.channelsDropdown = new DropDownBox("autodl-ircsrvs-channels");
	this.channelsDropdown.onChange = function(oldValue, newValue)
	{
		this_.onChannelsDropDownChange(oldValue, newValue);
	};

	$("#autodl-ircsrvs-ok-button").click(function(e) { okHandler() });
}

IrcServers.prototype.onBeforeShow =
function(configFile, trackerInfos, trackersId)
{
	this.configFile = configFile;
	this.activeServerInfo = null;
	this.currentChannelSection = null;

	this.initServers();
}

IrcServers.prototype.onAfterHide =
function()
{
	this.channelsDropdown.empty();
	this.serversListBox.removeAll();
	delete this.serverObjs;
	this.configFile = null;
	this.activeServerInfo = null;
	this.currentChannelSection = null;
}

IrcServers.prototype.onOkClicked =
function()
{
	this._saveServerObj(this.serversListBox.getSelectedData());

	// Save it all
	var servers = [];
	for (var i = 0; i < this.serverObjs.length; i++)
		servers.push(this.serverObjs[i].serverInfo);
	this.configFile.setServers(servers);

	return true;	// autodl.cfg updated
}

function _cloneServerInfo(serverInfo)
{
	var rv =
	{
		serverSection: serverInfo.serverSection.clone(),
		channels: []
	}

	for (var i = 0; i < serverInfo.channels.length; i++)
		rv.channels.push(serverInfo.channels[i].clone());

	return rv;
}

IrcServers.prototype.initServers =
function()
{
	this.serverObjs = [];
	var ary = this.configFile.getIrcServers();
	ary.sort(function(a, b)
	{
		return stringCompare(canonicalizeServerName(a.serverSection.name), canonicalizeServerName(b.serverSection.name));
	});
	for (var i = 0; i < ary.length; i++)
		this._addServer(_cloneServerInfo(ary[i]));

	if (this.serverObjs.length === 0)
		this._onServerSelected();
	else
		this.serversListBox.select(0);
}

IrcServers.prototype._fixName =
function(name)
{
	return name || theUILang.autodlNoName;
}

IrcServers.prototype._addServer =
function(serverInfo)
{
	var obj =
	{
		serverInfo: serverInfo
	};
	obj.checkboxElem = $('<input type="checkbox" />')[0];
	obj.labelElem = $('<label />').text(this._fixName(serverInfo.serverSection.name))[0];

	if (serverInfo.serverSection.getOption("enabled", "true", "bool").getValue())
		$(obj.checkboxElem).attr("checked", "checked");

	this.serversListBox.append($(obj.checkboxElem).add(obj.labelElem), obj);

	this.serverObjs.push(obj);
	return obj;
}

IrcServers.prototype._saveServerObj =
function(obj)
{
	if (obj)
	{
		var section = obj.serverInfo.serverSection;

		saveDialogOptions(section, this.serverOptions);

		section.name = $("#autodl-ircsrvs-server").val();

		var enabled = obj.checkboxElem.checked;
		section.getOption("enabled").setValue(enabled.toString());

		this._saveChannel(this.currentChannelSection);
	}
}

IrcServers.prototype._onServerSelected =
function(oldObj, newObj)
{
	this._saveServerObj(oldObj);
	this.currentChannelSection = null;

	this.activeServerInfo = (newObj || {}).serverInfo;
	var section = (this.activeServerInfo || {}).serverSection;

	initDialogOptions(section, this.serverOptions);
	$("#autodl-ircsrvs-server").val(section ? section.name : "");

	this.channelsDropdown.empty();
	if (this.activeServerInfo)
	{
		var channels = this.activeServerInfo.channels;
		var sorted = [];
		for (var i = 0; i < channels.length; i++)
		{
			var channel = channels[i];
			if (channel == null)
				continue;	// Removed

			sorted.push(
			{
				channelSection: channel,
				index: i
			});
		}
		sorted.sort(function(a, b)
		{
			a = a.channelSection.getOption("name", "", "text").getValue();
			b = b.channelSection.getOption("name", "", "text").getValue();
			return stringCompare(a.toLowerCase(), b.toLowerCase());
		});

		for (var i = 0; i < sorted.length; i++)
		{
			var channelSection = sorted[i].channelSection;
			this.channelsDropdown.add(sorted[i].index, this._fixName(channelSection.getOption("name", "", "text").getValue()));
		}
		if (sorted.length > 0)
			this.channelsDropdown.select(sorted[0].index);
		else
			this._initChannel();
	}
	else
		this._initChannel();

	var elems = $("#autodl-ircsrvs-servers-fieldset").find("input, select").add("#autodl-ircsrvs-remove-button, #autodl-ircsrvs-new-channel-button");
	enableJqueryElem(elems, newObj);
}

IrcServers.prototype.onChannelsDropDownChange =
function(oldValue, newValue)
{
	if (!this.activeServerInfo)
		return;

	var channels = this.activeServerInfo.channels;
	var oldChannelSection = oldValue == null ? null : channels[oldValue];
	var newChannelSection = newValue == null ? null : channels[newValue];

	if (oldChannelSection)
		this._saveChannel(oldChannelSection);

	this._initChannel(newChannelSection);

	this.currentChannelSection = newChannelSection;
}

IrcServers.prototype._saveChannel =
function(channelSection)
{
	if (channelSection)
	{
		saveDialogOptions(channelSection, this.channelOptions);

		var newName = $("#autodl-ircsrvs-channel").val();
		channelSection.getOption("name", "", "text").setValue(newName);
	}

}

IrcServers.prototype._initChannel =
function(channelSection)
{
	initDialogOptions(channelSection, this.channelOptions);
	var name = channelSection ? channelSection.getOption("name", "", "text").getValue() : "";
	$("#autodl-ircsrvs-channel").val(name);

	var elems = $("#autodl-ircsrvs-channels-fieldset").find("input, select").filter(":not(#autodl-ircsrvs-new-channel-button)");
	enableJqueryElem(elems, channelSection);
}

IrcServers.prototype._onServerNameModified =
function()
{
	var obj = this.serversListBox.getSelectedData();
	if (!obj)
		return;
	var newText = this._fixName($("#autodl-ircsrvs-server").val());
	$(obj.labelElem).text(newText);
}

IrcServers.prototype._onChannelNameModified =
function()
{
	var newText = this._fixName($("#autodl-ircsrvs-channel").val());
	this.channelsDropdown.renameSelected(newText);
}

IrcServers.prototype._onNewServerClicked =
function()
{
	var serverInfo =
	{
		serverSection: new ConfigSection(null, "server", ""),
		channels: []
	}
	var obj = this._addServer(serverInfo);
	this.serversListBox.selectData(obj);

	$("#autodl-ircsrvs-server").focus();
}

IrcServers.prototype._onRemoveServerClicked =
function()
{
	if (confirm(theUILang.autodlDeleteIrcServer))
	{
		var selectedIndex = this.serversListBox.getSelectedIndex();
		this.serverObjs.splice(selectedIndex, 1);
		this.serversListBox.removeSelected();
	}
}

IrcServers.prototype._onNewChannelClicked =
function()
{
	if (!this.activeServerInfo)
		return;

	var index = this.activeServerInfo.channels.length;
	var channelSection = new ConfigSection(null, "channel", "");
	this.activeServerInfo.channels.push(channelSection);
	this.channelsDropdown.add(index, this._fixName(channelSection.getOption("name", "", "text").getValue()));
	this.channelsDropdown.select(index);
	$("#autodl-ircsrvs-channel").focus();
}

IrcServers.prototype._onRemoveChannelClicked =
function()
{
	if (!this.activeServerInfo)
		return;
	var index = this.channelsDropdown.getSelectedValue();
	if (index == null)
		return;

	if (confirm(theUILang.autodlDeleteIrcChannel))
	{
		this.channelsDropdown.removeSelected();
		this.activeServerInfo.channels[index] = null;
	}
}
