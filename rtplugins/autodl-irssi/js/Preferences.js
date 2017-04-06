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

function Preferences()
{
}

function SitesButton(buttonId, textboxId, multiSelectDlgBox)
{
	this.buttonElem = document.getElementById(buttonId);
	this.textboxElem = document.getElementById(textboxId);
	this.multiSelectDlgBox = multiSelectDlgBox;

	var this_ = this;
	$(this.buttonElem).click(function(e)
	{
		this_._onClick(e);
	});
}

SitesButton.prototype._onClick =
function()
{
	var listboxData = [];
	for (var i = 0; i < this.trackerInfos.length; i++)
	{
		var trackerInfo = this.trackerInfos[i];
		listboxData.push(
		{
			displayName: trackerInfo.longName,
			validNames:
			[
				trackerInfo.type,
				trackerInfo.siteName,
				trackerInfo.longName
			]
		});
	}

	var data =
	{
		selectedText: $(this.textboxElem).val(),
		listboxData: listboxData,
		multiSelect: true,
		title: theUILang.autodlSelectTrackers,
		text: theUILang.autodlHoldCtrl,
		okClicked: function()
		{
			$(this_.textboxElem).val(data.selectedText);
		}
	};

	var this_ = this;
	this.multiSelectDlgBox.show(data)
}

SitesButton.prototype._setTrackerInfos =
function(trackerInfos)
{
	this.trackerInfos = trackerInfos;
}

Preferences.prototype.createDialogBox =
function(multiSelectDlgBox, okHandler)
{
	theDialogManager.make("autodl-preferences", theUILang.autodlPreferences,
		'<div id="autodl-prefs">' +
			'<div id="autodl-prefs-tabs">' +
				'<ul class="tabbar">' +
					'<li><a id="autodl-prefs-tab-general">' + theUILang.autodlGeneral + '</a></li>' +
					'<li><a id="autodl-prefs-tab-upload">' + theUILang.autodlUploadMethod + '</a></li>' +
					'<li><a id="autodl-prefs-tab-webui">' + theUILang.autodlWebui + '</a></li>' +
					'<li><a id="autodl-prefs-tab-ftp">' + theUILang.autodlFtp + '</a></li>' +
					'<li><a id="autodl-prefs-tab-irc">' + theUILang.autodlIrc + '</a></li>' +
					'<li><a id="autodl-prefs-tab-programs">' + theUILang.autodlPrograms + '</a></li>' +
					'<li><a id="autodl-prefs-tab-advanced">' + theUILang.autodlAdvanced + '</a></li>' +
				'</ul>' +
			'</div>' +
			'<div id="autodl-prefs-contents">' +
				'<div id="autodl-prefs-contents-general">' +
					'<div>' +
						'<td><label for="autodl-max-saved-releases">' + theUILang.autodlMaxSavedRels + '</label></td>' +
						'<td><input type="text" class="textbox-13" id="autodl-max-saved-releases" title="' + theUILang.autodlTitle31 + '"/></td>' +
					'</div>' +
					'<div>' +
						'<td>' +
							'<input type="checkbox" id="autodl-save-download-history" title="' + theUILang.autodlTitle32 + '"/>' +
							'<label for="autodl-save-download-history" title="' + theUILang.autodlTitle32 + '">' + theUILang.autodlSaveDlHist + '</label>' +
						'</td>' +
					'</div>' +
					'<div>' +
						'<td>' +
							'<input type="checkbox" id="autodl-download-duplicates" title="' + theUILang.autodlTitle33 + '"/>' +
							'<label for="autodl-download-duplicates" title="' + theUILang.autodlTitle33 + '">' + theUILang.autodlDownloadDupes + '</label>' +
						'</td>' +
					'</div>' +
					'<div>' +
						'<td>' +
							'<input type="checkbox" id="autodl-use-regex" title="' + theUILang.autodlTitle63 + '"/>' +
							'<label for="autodl-use-regex" title="' + theUILang.autodlTitle63 + '">' + theUILang.autodlUseRegex + '</label>' +
						'</td>' +
					'</div>' +
					'<div>' +
						'<td>' +
							'<input type="checkbox" id="autodl-unique-name" title="' + theUILang.autodlUniqueNameTitle + '"/>' +
							'<label for="autodl-unique-name" title="' + theUILang.autodlUniqueNameTitle + '">' + theUILang.autodlUniqueName + '</label>' +
						'</td>' +
					'</div>' +
					'<div>' +
						'<td>' +
							'<label for="autodl-automatic-updates">' + theUILang.autodlAutomaticUpdates + '</label>' +
							'<select id="autodl-automatic-updates" />' +
						'</td>' +
					'</div>' +
				'</div>' +
				'<div id="autodl-prefs-contents-upload"/>' +
				'<div id="autodl-prefs-contents-webui">' +
					'<p>' + theUILang.autodlOnlyUtorrentWebui2 + '</p>' +
					'<table>' +
						'<tbody>' +
							'<tr>' +
								'<td><label for="autodl-webui-user">' + theUILang.autodlUserName + '</label></td>' +
								'<td><input type="text" class="textbox-13" id="autodl-webui-user" title="' + theUILang.autodlTitle34 + '"/></td>' +
								'<td><label for="autodl-webui-password">' + theUILang.autodlPassword + '</label></td>' +
								'<td><input type="password" class="textbox-13" id="autodl-webui-password" title="' + theUILang.autodlTitle35 + '"/></td>' +
							'</tr>' +
							'<tr>' +
								'<td><label for="autodl-webui-hostname">' + theUILang.autodlIpAddress + '</label></td>' +
								'<td><input type="text" class="textbox-13" id="autodl-webui-hostname" title="' + theUILang.autodlTitle36 + '"/></td>' +
								'<td><label for="autodl-webui-port">' + theUILang.autodlPort + '</label></td>' +
								'<td><input type="text" class="textbox-13" id="autodl-webui-port" title="' + theUILang.autodlTitle37 + '"/></td>' +
							'</tr>' +
						'</tbody>' +
					'</table>' +
					'<input type="checkbox" id="autodl-webui-ssl" />' +
					'<label for="autodl-webui-ssl">' + theUILang.autodlUseSsl + '</label>' +
				'</div>' +
				'<div id="autodl-prefs-contents-ftp">' +
					'<table>' +
						'<tbody>' +
							'<tr>' +
								'<td><label for="autodl-ftp-user">' + theUILang.autodlUserName + '</label></td>' +
								'<td><input type="text" class="textbox-13" id="autodl-ftp-user" title="' + theUILang.autodlTitle38 + '"/></td>' +
								'<td><label for="autodl-ftp-password">' + theUILang.autodlPassword + '</label></td>' +
								'<td><input type="password" class="textbox-13" id="autodl-ftp-password" title="' + theUILang.autodlTitle39 + '"/></td>' +
							'</tr>' +
							'<tr>' +
								'<td><label for="autodl-ftp-hostname">' + theUILang.autodlHostname + '</label></td>' +
								'<td><input type="text" class="textbox-13" id="autodl-ftp-hostname" title="' + theUILang.autodlTitle40 + '"/></td>' +
								'<td><label for="autodl-ftp-port">' + theUILang.autodlPort + '</label></td>' +
								'<td><input type="text" class="textbox-13" id="autodl-ftp-port" title="' + theUILang.autodlTitle41 + '"/></td>' +
							'</tr>' +
						'</tbody>' +
					'</table>' +
				'</div>' +
				'<div id="autodl-prefs-contents-irc">' +
					'<input type="checkbox" id="autodl-irc-autoconn-enabled" title="' + theUILang.autodlAutoConnEnabled + '"/>' +
					'<label for="autodl-irc-autoconn-enabled" title="' + theUILang.autodlAutoConnEnabled + '">' + theUILang.autodlAutoConnEnabled + '</label>' +
					'<table>' +
						'<tbody>' +
							'<tr>' +
								'<td><label for="autodl-irc-user-name">' + theUILang.autodlUserName + '</label></td>' +
								'<td><input type="text" class="textbox-13" id="autodl-irc-user-name" title="' + theUILang.autodlYourUserName + '"/></td>' +
								'<td><label for="autodl-irc-real-name">' + theUILang.autodlRealName + '</label></td>' +
								'<td><input type="text" class="textbox-13" id="autodl-irc-real-name" title="' + theUILang.autodlYourRealName + '"/></td>' +
							'</tr>' +
							'<tr>' +
								'<td colspan="4">' + theUILang.autodlIrcOutputDesc + '</td>' +
							'</tr>' +
							'<tr>' +
								'<td><label for="autodl-irc-server">' + theUILang.autodlOutputServer + '</label></td>' +
								'<td><input type="text" class="textbox-13" id="autodl-irc-server" title="' + theUILang.autodlOutputServerTitle + '" placeholder="' + theUILang.autodlOutputServerEmpty + '"/></td>' +
								'<td><label for="autodl-irc-channel">' + theUILang.autodlOutputChannel + '</label></td>' +
								'<td><input type="text" class="textbox-13" id="autodl-irc-channel" title="' + theUILang.autodlOutputChannelTitle + '" placeholder="' + theUILang.autodlOutputChannelEmpty + '"/></td>' +
							'</tr>' +
						'</tbody>' +
					'</table>' +
				'</div>' +
				'<div id="autodl-prefs-contents-programs">' +
					'<table>' +
						'<tbody>' +
							'<tr>' +
								'<td><label for="autodl-programs-utorrent">' + theUILang.autodlUtorrentExe + '</label></td>' +
								'<td><input type="text" class="textbox-30" id="autodl-programs-utorrent" title="' + theUILang.autodlTitle42 + '" placeholder="' + theUILang.autodlHint31 + '"/></td>' +
							'</tr>' +
						'</tbody>' +
					'</table>' +
				'</div>' +
				'<div id="autodl-prefs-contents-advanced">' +
					'<table>' +
						'<tbody>' +
							'<tr>' +
								'<td><label for="autodl-advanced-user-agent">' + theUILang.autodlDownloadUserAgent + '</label></td>' +
								'<td><input type="text" class="textbox-20" id="autodl-advanced-user-agent" title="' + theUILang.autodlTitle43 + '" placeholder="' + theUILang.autodlHint32 + '"/></td>' +
							'</tr>' +
							'<tr>' +
								'<td><label for="autodl-advanced-max-download-retry-time">' + theUILang.autodlMaxDlRetryTime + '</label></td>' +
								'<td><input type="text" class="textbox-20" id="autodl-advanced-max-download-retry-time" title="' + theUILang.autodlTitle46 + '"/></td>' +
								'<td><label for="autodl-advanced-max-download-retry-time">' + theUILang.autodlSeconds + '</label></td>' +
							'</tr>' +
							'<tr>' +
								'<td><label for="autodl-advanced-output-level">' + theUILang.autodlOutputLevel + '</label></td>' +
								'<td><input type="text" class="textbox-20" id="autodl-output-level" title="' + theUILang.autodlTitle47 + '"/></td>' +
							'</tr>' +
							'<tr>' +
								'<td><input type="button" id="autodl-advanced-output-sites-button" class="Button" value="' + theUILang.autodlAdvancedOutputSites + '" /></td>' +
								'<td><input type="text" class="textbox-20" id="autodl-advanced-output-sites" title="' + theUILang.autodlTitle61 + '" placeholder="' + theUILang.autodlHint4 + '"/></td>' +
							'</tr>' +
						'</tbody>' +
					'</table>' +
					'<input type="checkbox" id="autodl-advanced-debug" title="' + theUILang.autodlTitle48 + '"/>' +
					'<label for="autodl-advanced-debug" title="' + theUILang.autodlTitle48 + '">' + theUILang.autodlDebug + '</label>' +
				'</div>' +
			'</div>' +
			'<div class="aright buttons-list dialog-buttons">' +
				'<input type="button" id="autodl-prefs-ok-button" value="' + theUILang.ok + '" class="OK Button" />' +
				'<input type="button" value="' + theUILang.Cancel + '" class="Cancel Button" />' +
			'</div>' +
		'</div>'
	);

	this.options =
	[
		new DialogOptionInt("autodl-max-saved-releases", "max-saved-releases", "1000"),
		new DialogOptionBool("autodl-save-download-history", "save-download-history", "true"),
		new DialogOptionBool("autodl-unique-name", "unique-torrent-names", "false"),
		new DialogOptionBool("autodl-download-duplicates", "download-duplicates", "false"),
		new DialogOptionBool("autodl-use-regex", "use-regex", "false"),
		new DialogOptionText("autodl-programs-utorrent", "path-utorrent", ""),
		new DialogOptionText("autodl-advanced-user-agent", "user-agent", "autodl-irssi"),
		new DialogOptionInt("autodl-advanced-max-download-retry-time", "download-retry-time-seconds", "300"),
		new DialogOptionInt("autodl-output-level", "output-level", "3"),
		new DialogOptionText("autodl-advanced-output-sites", "advanced-output-sites", ""),
		new DialogOptionBool("autodl-advanced-debug", "debug", "false")
	];

	this.webui =
	[
		new DialogOptionText("autodl-webui-user", "user", ""),
		new DialogOptionText("autodl-webui-password", "password", ""),
		new DialogOptionText("autodl-webui-hostname", "hostname", ""),
		new DialogOptionInt("autodl-webui-port", "port", "0"),
		new DialogOptionBool("autodl-webui-ssl", "ssl", "false")
	];

	this.ftp =
	[
		new DialogOptionText("autodl-ftp-user", "user", ""),
		new DialogOptionText("autodl-ftp-password", "password", ""),
		new DialogOptionText("autodl-ftp-hostname", "hostname", ""),
		new DialogOptionInt("autodl-ftp-port", "port", "0")
	];

	this.irc =
	[
		new DialogOptionBool("autodl-irc-autoconn-enabled", "auto-connect", "true"),
		new DialogOptionText("autodl-irc-user-name", "user-name", ""),
		new DialogOptionText("autodl-irc-real-name", "real-name", ""),
		new DialogOptionText("autodl-irc-server", "output-server", ""),
		new DialogOptionText("autodl-irc-channel", "output-channel", "")
	];

	var this_ = this;

	this.tabs = new Tabs();
	this.tabs.add("autodl-prefs-tab-general", "autodl-prefs-contents-general");
	this.tabs.add("autodl-prefs-tab-upload", "autodl-prefs-contents-upload");
	this.tabs.add("autodl-prefs-tab-webui", "autodl-prefs-contents-webui");
	this.tabs.add("autodl-prefs-tab-ftp", "autodl-prefs-contents-ftp");
	this.tabs.add("autodl-prefs-tab-irc", "autodl-prefs-contents-irc");
	this.tabs.add("autodl-prefs-tab-programs", "autodl-prefs-contents-programs");
	this.tabs.add("autodl-prefs-tab-advanced", "autodl-prefs-contents-advanced");

	this.dropDownBox = new DropDownBox("autodl-automatic-updates");
	this.dropDownBox.add("auto", theUILang.autodlUpdateAutomatically);
	this.dropDownBox.add("ask", theUILang.autodlAsk);
	this.dropDownBox.add("disabled", theUILang.autodlDontCheckForUpdates);

	this.uploadMethod = new UploadMethod("autodl-preferences", "autodl-prefs-contents-upload", true);

	this.advancedOutputSitesButton = new SitesButton("autodl-advanced-output-sites-button", "autodl-advanced-output-sites", multiSelectDlgBox);

	$("#autodl-prefs-ok-button").click(function(e) { okHandler() });
}

Preferences.prototype.onBeforeShow =
function(configFile, trackerInfos, trackersId)
{
	this.configFile = configFile;	// onOkClicked() needs it

	this.uploadMethod.onBeforeShow();
	this.uploadMethod.initFields(this.configFile.getSection("options", null));
	this.advancedOutputSitesButton._setTrackerInfos(trackerInfos);
	initDialogOptions(this.configFile.getSection("options", null), this.options);
	initDialogOptions(this.configFile.getSection("webui", null), this.webui);
	initDialogOptions(this.configFile.getSection("ftp", null), this.ftp);
	initDialogOptions(this.configFile.getSection("irc", null), this.irc);

	var section = this.configFile.getSection("options", null);
	var option = section.getOption("update-check", "ask", "text");
	this.dropDownBox.select(option.getValue());
}

Preferences.prototype.onAfterHide =
function()
{
	this.uploadMethod.onAfterHide();
	this.advancedOutputSitesButton._setTrackerInfos(null);
	this.configFile = null;
}

Preferences.prototype.onOkClicked =
function()
{
	var optionsSection = this.configFile.getSection("options", null);
	this.uploadMethod.saveOptions(optionsSection);
	saveDialogOptions(optionsSection, this.options);
	saveDialogOptions(this.configFile.getSection("webui", null), this.webui);
	saveDialogOptions(this.configFile.getSection("ftp", null), this.ftp);
	saveDialogOptions(this.configFile.getSection("irc", null), this.irc);

	optionsSection.getOption("update-check").setValue(this.dropDownBox.getSelectedValue());

	return true;	// autodl.cfg updated
}
