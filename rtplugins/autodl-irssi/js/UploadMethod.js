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

function UploadMethod(idDlg, idParent, isPrefs)
{
	this.num = UploadMethod.num++;

	if (isPrefs)
		var text = [theUILang.autodlChoose, theUILang.autodlChooseMethod];
	else
		var text = [theUILang.autodlOverride1, theUILang.autodlOverride2];

	// The 'rtorrent' part takes up a whole lot of space. The preferences dialog box is wide
	// enough for some optimizations.
	var isWide = idDlg === "autodl-preferences";

	var html = '' +
		'<label for="' + this.id("method") + '">' + theUILang.autodlChooseUploadMethod + '</label>' +
		'<select id="' + this.id("method") + '">' +
			'<option id="' + this.id("method-nothing") + '">' + text[0] + '</option>' +
			'<option id="' + this.id("method-rtorrent") + '">' + theUILang.autodlRtorrent + '</option>' +
			'<option id="' + this.id("method-watchdir") + '">' + theUILang.autodlSaveToWatchFolder + '</option>' +
			'<option id="' + this.id("method-webui") + '">' + theUILang.autodlUtorrentWebui + '</option>' +
			'<option id="' + this.id("method-ftp") + '">' + theUILang.autodlFtpUpoad + '</option>' +
			'<option id="' + this.id("method-program") + '">' + theUILang.autodlRunProgram + '</option>' +
			'<option id="' + this.id("method-dyndir") + '">' + theUILang.autodlDynamicFolder + '</option>' +
		'</select>' +
		'<div id="' + this.id("nothing") + '">' +
			'<p>' + text[1] + '</p>' +
		'</div>' +
		'<div id="' + this.id("rtorrent") + '">' +
			'<table>' +
				'<tbody>' +
					'<tr>' +
						'<td><label for="' + this.id("rtorrent-folder") + '">' + theUILang.autodlRtDestDir + '</label></td>' +
						'<td>' +
							'<input type="text" class="textbox-20" id="' + this.id("rtorrent-folder") + '" title="' + theUILang.autodlTitle49 + '" />' +
							'<input type="button" class="Button watchdir-button" id="' + this.id("rt-getdir-button") + '" value="xyz" />' +
						'</td>' +
					'</tr>' +
				'</tbody>' +
			'</table>'
	if (isWide)
	{
		html += '' +
			'<table>' +
				'<tbody>' +
					'<tr class="rtorrent-tbl1">' +
						'<td><label for="' + this.id("rtorrent-label") + '">' + theUILang.autodlLabel + '</label></td>' +
						'<td><input type="text" class="textbox-7" id="' + this.id("rtorrent-label") + '" title="' + theUILang.autodlTitle51 + '" /></td>' +
						'<td><label for="' + this.id("rtorrent-commands") + '">' + theUILang.autodlRtCmds + '</label></td>' +
						'<td><input type="text" class="textbox-20" id="' + this.id("rtorrent-commands") + '" title="' + theUILang.autodlTitle50 + '" /></td>' +
					'</tr>' +
				'</tbody>' +
			'</table>' +
			'<table>' +
				'<tbody>' +
					'<tr>' +
						'<td><label for="' + this.id("rtorrent-ratio-group") + '">' + theUILang.autodlRatioGroup + '</label></td>' +
						'<td><select id="' + this.id("rtorrent-ratio-group") + '" title="' + theUILang.autodlTitle53 + '" /></td>' +
						'<td><label for="' + this.id("rtorrent-channel") + '">' + theUILang.autodlChannel + '</label></td>' +
						'<td><select id="' + this.id("rtorrent-channel") + '" title="' + theUILang.autodlTitle54 + '" /></td>' +
						'<td><label for="' + this.id("rtorrent-priority") + '">' + theUILang.autodlPriority + '</label></td>' +
						'<td><select id="' + this.id("rtorrent-priority") + '" title="' + theUILang.autodlTitle55 + '" /></td>' +
					'</tr>' +
				'</tbody>' +
			'</table>' +
			'<div>' +
				'<input type="checkbox" id="' + this.id("rtorrent-dont-add-name") + '" title="' + theUILang.autodlTitle56 + '"/>' +
				'<label for="' + this.id("rtorrent-dont-add-name") + '" title="' + theUILang.autodlTitle56 + '">' + theUILang.autodlDontAddName + '</label>' +
			'</div>' +
			'<div>' +
				'<input type="checkbox" id="' + this.id("rtorrent-scheduler") + '" title="' + theUILang.autodlTitle52 + '"/>' +
				'<label for="' + this.id("rtorrent-scheduler") + '" title="' + theUILang.autodlTitle52 + '">' + theUILang.autodlIgnoreScheduler + '</label>' +
			'</div>';
	}
	else
	{
		html += '' +
			'<table>' +
				'<tbody>' +
					'<tr class="rtorrent-tbl1">' +
						'<td><label for="' + this.id("rtorrent-label") + '">' + theUILang.autodlLabel + '</label></td>' +
						'<td><input type="text" class="textbox-7" id="' + this.id("rtorrent-label") + '" title="' + theUILang.autodlTitle51 + '" /></td>' +
						'<td><label for="' + this.id("rtorrent-commands") + '">' + theUILang.autodlRtCmds + '</label></td>' +
						'<td><input type="text" class="textbox-9" id="' + this.id("rtorrent-commands") + '" title="' + theUILang.autodlTitle50 + '" /></td>' +
					'</tr>' +
				'</tbody>' +
			'</table>' +
			'<table>' +
				'<tbody>' +
					'<tr>' +
						'<td><label for="' + this.id("rtorrent-ratio-group") + '">' + theUILang.autodlRatioGroup + '</label></td>' +
						'<td><select id="' + this.id("rtorrent-ratio-group") + '" title="' + theUILang.autodlTitle53 + '" /></td>' +
						'<td><label for="' + this.id("rtorrent-channel") + '">' + theUILang.autodlChannel + '</label></td>' +
						'<td><select id="' + this.id("rtorrent-channel") + '" title="' + theUILang.autodlTitle54 + '" /></td>' +
					'</tr>' +
				'</tbody>' +
			'</table>' +
			'<table>' +
				'<tbody>' +
					'<tr>' +
						'<td><label for="' + this.id("rtorrent-priority") + '">' + theUILang.autodlPriority + '</label></td>' +
						'<td><select id="' + this.id("rtorrent-priority") + '" title="' + theUILang.autodlTitle55 + '" /></td>' +
					'</tr>' +
				'</tbody>' +
			'</table>' +
			'<div>' +
				'<input type="checkbox" id="' + this.id("rtorrent-dont-add-name") + '" title="' + theUILang.autodlTitle56 + '"/>' +
				'<label for="' + this.id("rtorrent-dont-add-name") + '" title="' + theUILang.autodlTitle56 + '">' + theUILang.autodlDontAddName + '</label>' +
			'</div>' +
			'<div>' +
				'<input type="checkbox" id="' + this.id("rtorrent-scheduler") + '" title="' + theUILang.autodlTitle52 + '"/>' +
				'<label for="' + this.id("rtorrent-scheduler") + '" title="' + theUILang.autodlTitle52 + '">' + theUILang.autodlIgnoreScheduler + '</label>' +
			'</div>';
	}
	html += '' +
		'</div>' +
		'<div id="' + this.id("watchdir") + '">' +
			'<table>' +
				'<tbody>' +
					'<tr>' +
						'<td><label for="' + this.id("watchdir-folder") + '">' + theUILang.autodlTorrentClientWatchFolder + '</label></td>' +
						'<td>' +
							'<input type="text" class="textbox-20" id="' + this.id("watchdir-folder") + '" title="' + theUILang.autodlTitle26 + '" />' +
							'<input type="button" class="Button watchdir-button" id="' + this.id("getdir-button") + '" value="xyz" />' +
						'</td>' +
					'</tr>' +
				'</tbody>' +
			'</table>' +
		'</div>' +
		'<div id="' + this.id("webui") + '">' +
			'<p>' + theUILang.autodlOnlyUtorrentWebui + '</p>' +
			'<p>' + theUILang.autodlSettingsInPrefsWebui + '</p>' +
		'</div>' +
		'<div id="' + this.id("ftp") + '">' +
			'<p>' + theUILang.autodlSettingsInPrefsFtp + '</p>' +
			'<table>' +
				'<tbody>' +
					'<tr>' +
						'<td><label for="' + this.id("ftp-path") + '">' + theUILang.autodlFtpPath + '</label></td>' +
						'<td><input type="text" class="textbox-25" id="' + this.id("ftp-path") + '" title="' + theUILang.autodlTitle27 + '" placeholder="' + theUILang.autodlHint27 + '"/></td>' +
					'</tr>' +
				'</tbody>' +
			'</table>' +
		'</div>' +
		'<div id="' + this.id("program") + '">' +
			'<table>' +
				'<tbody>' +
					'<tr>' +
						'<td><label for="' + this.id("program-command") + '">' + theUILang.autodlCommand + '</label></td>' +
						'<td><input type="text" class="textbox-25" id="' + this.id("program-command") + '" title="' + theUILang.autodlTitle28 + '" placeholder="' + theUILang.autodlHint28 + '"/></td>' +
					'</tr>' +
					'<tr>' +
						'<td><label for="' + this.id("program-args") + '">' + theUILang.autodlArguments + '</label></td>' +
						'<td><input type="text" class="textbox-25" id="' + this.id("program-args") + '" title="' + theUILang.autodlTitle29 + '" placeholder="' + theUILang.autodlHint29 + '"/></td>' +
					'</tr>' +
				'</tbody>' +
			'</table>' +
		'</div>' +
		'<div id="' + this.id("dyndir") + '">' +
			'<table>' +
				'<tbody>' +
					'<tr>' +
						'<td><label for="' + this.id("dyndir-dir") + '">' + theUILang.autodlFolder + '</label></td>' +
						'<td><input type="text" class="textbox-25" id="' + this.id("dyndir-dir") + '" title="' + theUILang.autodlTitle30 + '" placeholder="' + theUILang.autodlHint30 + '"/></td>' +
					'</tr>' +
				'</tbody>' +
			'</table>' +
		'</div>';
	$("#" + idParent).html(html);

	var this_ = this;

	if (thePlugins.isInstalled("_getdir"))
	{
		this.getdirButton1 = new theWebUI.rDirBrowser(idDlg, this.id("watchdir-folder"), this.id("getdir-button"));
		this.getdirButton2 = new theWebUI.rDirBrowser(idDlg, this.id("rtorrent-folder"), this.id("rt-getdir-button"));
	}
	else
	{
		$("#" + this.id("getdir-button")).remove();
		$("#" + this.id("rt-getdir-button")).remove();
	}

	this.dropdown = new DropDownTabs(this.id("method"));
	this.dropdown.add(this.id("method-nothing"), this.id("nothing"), "");
	this.dropdown.add(this.id("method-rtorrent"), this.id("rtorrent"), "rtorrent");
	this.dropdown.add(this.id("method-watchdir"), this.id("watchdir"), "watchdir");
	this.dropdown.add(this.id("method-webui"), this.id("webui"), "webui");
	this.dropdown.add(this.id("method-ftp"), this.id("ftp"), "ftp");
	this.dropdown.add(this.id("method-program"), this.id("program"), "exec");
	this.dropdown.add(this.id("method-dyndir"), this.id("dyndir"), "dyndir");
	this.dropdown.onChange = function(oldObj, newObj) { this_._onChange(oldObj, newObj) };

	this.options =
	[
		new DialogOptionText(this.id("watchdir-folder"), "upload-watch-dir", ""),
		new DialogOptionText(this.id("ftp-path"), "upload-ftp-path", ""),
		new DialogOptionText(this.id("program-command"), "upload-command", ""),
		new DialogOptionText(this.id("program-args"), "upload-args", ""),
		new DialogOptionText(this.id("dyndir-dir"), "upload-dyndir", ""),
		new DialogOptionText(this.id("rtorrent-folder"), "rt-dir", ""),
		new DialogOptionText(this.id("rtorrent-commands"), "rt-commands", ""),
		new DialogOptionText(this.id("rtorrent-label"), "rt-label", ""),
		new DialogOptionBool(this.id("rtorrent-scheduler"), "rt-ignore-scheduler", "false"),
		new DialogOptionBool(this.id("rtorrent-dont-add-name"), "rt-dont-add-name", "false")
	];
}

UploadMethod.num = 0;

UploadMethod.prototype.id =
function(id)
{
	return "autodl-upload-" + id + "-" + this.num;
}

function _fixUserPriority(priority)
{
	switch (priority)
	{
	case "0":
	case "dont-download":
		return "dont-download";
	case "1":
	case "low":
		return "low";
	case "2":
	case "normal":
	default:
		return "normal";
	case "3":
	case "high":
		return "high";
	}
}

UploadMethod.prototype.initFields =
function(section)
{
	initDialogOptions(section, this.options);

	this.ratioDropdown.select(getSectionOptionValue(section, "rt-ratio-group", "", "text"));
	this.channelDropdown.select(getSectionOptionValue(section, "rt-channel", "", "text"));
	this.priorityDropdown.select(_fixUserPriority(getSectionOptionValue(section, "rt-priority", "", "text")));

	this.dropdown.select(getSectionOptionValue(section, "upload-type", "", "text"));
}

UploadMethod.prototype.saveOptions =
function(section)
{
	saveDialogOptions(section, this.options);

	section.getOption("rt-ratio-group").setValue(this.ratioDropdown.getSelectedValue());
	section.getOption("rt-channel").setValue(this.channelDropdown.getSelectedValue());

	var priority = this.priorityDropdown.getSelectedValue();
	if (priority === "normal")
		priority = "";	// normal is default
	section.getOption("rt-priority").setValue(priority);

	section.getOption("upload-type").setValue(this.dropdown.getSelectedValue());
}

UploadMethod.prototype._hideGetdirButtons =
function()
{
	if (this.getdirButton1)
		this.getdirButton1.hide();
	if (this.getdirButton2)
		this.getdirButton2.hide();
}

UploadMethod.prototype._onChange =
function(oldObj, newObj)
{
	if (!newObj || newObj.value !== "watchdir")
		this._hideGetdirButtons();
}

UploadMethod.prototype.reenableElements =
function()
{
	enableJqueryElem($('#' + this.id("rtorrent-ratio-group")), theWebUI.ratios != null);
	enableJqueryElem($('#' + this.id("rtorrent-channel")), theWebUI.throttles != null);
}

UploadMethod.prototype.onBeforeShow =
function()
{
	function getRatioName(i)
	{
		if (theWebUI.ratios && theWebUI.ratios[i] && theWebUI.ratios[i].name != "")
			return theWebUI.ratios[i].name;
		return "rat_" + i;
	}
	function getChannelName(i)
	{
		if (theWebUI.throttles && theWebUI.throttles[i] && theWebUI.throttles[i].name != "")
			return theWebUI.throttles[i].name;
		return "thr_" + i;
	}

	$('#' + this.id("rtorrent-ratio-group")).empty();
	this.ratioDropdown = new DropDownBox(this.id("rtorrent-ratio-group"));
	this.ratioDropdown.add("", theUILang.autodlNone);
	for (var i = 0; i < 8; i++)
		this.ratioDropdown.add("rat_" + i, getRatioName(i));

	$('#' + this.id("rtorrent-channel")).empty();
	this.channelDropdown = new DropDownBox(this.id("rtorrent-channel"));
	this.channelDropdown.add("", theUILang.autodlNone);
	for (var i = 0; i < 10; i++)
		this.channelDropdown.add("thr_" + i, getChannelName(i));

	$('#' + this.id("rtorrent-priority")).empty();
	this.priorityDropdown = new DropDownBox(this.id("rtorrent-priority"));
	this.priorityDropdown.add("high", theUILang.autodlPrioHigh);
	this.priorityDropdown.add("normal", theUILang.autodlPrioNormal);
	this.priorityDropdown.add("low", theUILang.autodlPrioLow);
	this.priorityDropdown.add("dont-download", theUILang.autodlPrioDontDownload);

	this.reenableElements();
}

UploadMethod.prototype.onAfterHide =
function()
{
	this._hideGetdirButtons();
}
