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

var tvResolutions =
[
	["Portable Device", "PD"],
	["SD", "SDTV", "Standard Def", "Standard Definition"],
	["480i"],
	["480p"],
	["576p"],
	["720p"],
	["810p"],
	["1080i"],
	["1080p"]
];

var tvSources =
[
	["DSR"],
	["PDTV"],
	["HDTV"],
	["HR.PDTV"],
	["HR.HDTV"],
	["DVDRip"],
	["DVDScr", "DVDScrener", "DVD-Screner"],
	["BDr"],
	["BD5"],
	["BD9"],
	["BDRip"],
	["BRRip", "BLURAYRiP"],
	["DVDR", "MDVDR", "DVD", "DVD-R"],
	["HDDVD", "HD-DVD"],
	["HDDVDRip"],
	["BluRay", "Blu-Ray", "MBluRay"],
	["WEB-DL", "WEB", "WEBDL"],
	["WEBRip", "Webrip", "WebRip", "WEBRIP"],
	["TVRip", "TV"],
	["CAM"],
	["HDCAM", "HD-CAM", "HD CAM"],
	["R5"],
	["TELESYNC", "TS"],
	["HDTS", "HD-TS", "HD TS"],
	["TELECINE", "TC"]
];

var tvEncoders =
[
	["XviD", "XvidHD"],
	["DivX"],
	["x264"],
	["x264-Hi10p", "Hi10p", "10-bit"],
	["h.264", "h264"],
	["mpeg2", "mpeg-2"],
	["VC-1", "VC1"],
	["WMV", "WMV-HD"],
	["Remux", "h.264 Remux", "h264 Remux", "VC-1 Remux", "VC1 Remux"]
];

var musicFormats =
[
	["MP3"],
	["FLAC"],
	["Ogg"],
	["AAC"],
	["AC3"],
	["DTS"]
];

var musicBitrates =
[
	["192"],
	["APS (VBR)"],
	["V2 (VBR)"],
	["V1 (VBR)"],
	["256"],
	["APX (VBR)"],
	["V0 (VBR)"],
	["q8.x (VBR)"],
	["320"],
	["Lossless"],
	["24bit Lossless"],
	["Other"]
];

var musicMedia =
[
	["CD"],
	["DVD"],
	["Vinyl"],
	["Soundboard"],
	["SACD"],
	["DAT"],
	["Cassette"],
	["WEB"],
	["Other"]
];

function MenuButton(buttonId, textboxId, strings, onlyAppendValues, sortIt)
{
	this.buttonElem = document.getElementById(buttonId);
	this.textboxElem = document.getElementById(textboxId);
	this.onlyAppendValues = !!onlyAppendValues;

	this.strings = [];
	for (var i = 0; i < strings.length; i++)
		this.strings[i] = strings[i];
	if (sortIt == null || sortIt === true)
	{
		this.strings.sort(function(a, b)
		{
			return stringCompare(a[0].toLowerCase(), b[0].toLowerCase());
		});
	}

	var this_ = this;
	$(this.buttonElem).click(function(e)
	{
		this_._onClick(e);
	});
	$(this.buttonElem).mouseup(function(e)
	{
		// Prevent ruTorrent from immediately closing the popup menu!
		e.stopPropagation();
	});
}

MenuButton.prototype._getChecked =
function()
{
	var checked = [];

	if (this.onlyAppendValues)
		return checked;

	var ary = $(this.textboxElem).val().split(",");
	for (var i = 0; i < ary.length; i++)
	{
		var name = $.trim(ary[i]).toLowerCase();
		for (var j = 0; j < this.strings.length; j++)
		{
			var strings = this.strings[j];
			for (var k = 0; k < strings.length; k++)
			{
				if (checkFilterStrings(strings[k], name))
					checked[j] = true;
			}
		}
	}

	return checked;
}

MenuButton.prototype._isOurMenuVisible =
function()
{
	if (theContextMenu.obj.css("display") !== "block")
		return false;

	var links = theContextMenu.obj.find("a");
	var len = links.length;
	if (len !== this.strings.length || $(links[len-1]).text() !== this.strings[len-1][0])
		return false;

	return true;
}

MenuButton.prototype._onClick =
function(e)
{
	if (this._isOurMenuVisible())
	{
		theContextMenu.hide();
		return;
	}

	theContextMenu.clear();

	var this_ = this;
	this.checked = this._getChecked();
	for (var i = 0; i < this.strings.length; i++)
	{
		(function(i)
		{
			var ary = [];
			if (this_.checked[i])
				ary.push(CMENU_SEL);
			ary.push(this_.strings[i][0], function()
			{
				this_._onClickedMenuitem(i);
			});
			theContextMenu.add(ary);
		})(i);
	}

	var offset = $(this.buttonElem).offset();
	var x = offset.left;
	var y = offset.top + $(this.buttonElem).height();
	theContextMenu.show(x, y);
}

// Called when the user clicked one of the menu items
MenuButton.prototype._onClickedMenuitem =
function(index)
{
	this.checked[index] = !this.checked[index];

	var s;
	if (this.onlyAppendValues)
	{
		s = $(this.textboxElem).val();
		if (s)
			s += ", ";
		s += this.strings[index][0];
	}
	else
	{
		s = "";
		for (var i in this.checked)
		{
			if (!this.checked[i])
				continue;
			if (s)
				s += ", ";
			s += this.strings[i][0];
		}
	}
	$(this.textboxElem).val(s);
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

function Filters()
{
}

// Class for the ANY / ALL drop down box (eg. the one used by tags and except-tags)
function AnyAllDropDown(id, optionName)
{
	this.dropDownBox = new DropDownBox(id);
	this.dropDownBox.add("any", theUILang.autodlAny);
	this.dropDownBox.add("all", theUILang.autodlAll);
	this.optionName = optionName;
}

AnyAllDropDown.prototype.initialize =
function(section)
{
	var value = getSectionOptionValue(section, this.optionName, "true", "bool");
	this.dropDownBox.select(value ? "any" : "all");
}

AnyAllDropDown.prototype.save =
function(section)
{
	var value = this.dropDownBox.getSelectedValue() === "any";
	section.getOption(this.optionName).setValue(value);
}

Filters.prototype.createDialogBox =
function(multiSelectDlgBox, okHandler)
{
	theDialogManager.make("autodl-filters", theUILang.autodlFilters,
		'<div id="autodl-filters">' +
			'<div id="autodl-filters-left">' +
				'<div id="autodl-filters-list" />' +
				'<div id="autodl-filters-list-buttons" align="center">' +
					'<input type="button" class="Button" id="autodl-filters-new-button" value="' + theUILang.autodlNew + '" />' +
					'<input type="button" class="Button" id="autodl-filters-remove-button" value="' + theUILang.autodlRemove + '" />' +
				'</div>' +
			'</div>' +
			'<div id="autodl-filters-right">' +
				'<div id="autodl-filters-tabs">' +
					'<ul class="tabbar">' +
						'<li><a id="autodl-filters-tab-general">' + theUILang.autodlGeneral + '</a></li>' +
						'<li><a id="autodl-filters-tab-p2p">' + theUILang.autodlP2P + '</a></li>' +
						'<li><a id="autodl-filters-tab-tv-movie">' + theUILang.autodlTvMovies + '</a></li>' +
						'<li><a id="autodl-filters-tab-music">' + theUILang.autodlMusic + '</a></li>' +
						'<li><a id="autodl-filters-tab-advanced">' + theUILang.autodlAdvanced + '</a></li>' +
						'<li><a id="autodl-filters-tab-upload">' + theUILang.autodlUploadMethod + '</a></li>' +
						'<li><a id="autodl-filters-tab-wol">' + theUILang.autodlWol + '</a></li>' +
					'</ul>' +
				'</div>' +
				'<div id="autodl-filters-contents">' +
					'<div id="autodl-filters-contents-general">' +
						'<table>' +
							'<tbody>' +
								'<tr>' +
									'<td><label for="autodl-filters-name">' + theUILang.autodlDisplayName + '</label></td>' +
									'<td><input type="text" class="textbox-23" id="autodl-filters-name" title="' + theUILang.autodlTitle1 + '" placeholder="' + theUILang.autodlHint1 + '"/></td>' +
								'</tr>' +
								'<tr>' +
									'<td><label for="autodl-filters-priority">' + theUILang.autodlFiltersPriority + '</label></td>' +
									'<td><input type="text" class="textbox-3" id="autodl-filters-priority" title="' + theUILang.autodlTitle66 + '"/></td>' +
								'</tr>' +
							'</tbody>' +
						'</table>' +
						'<hr>' +
						'<table>' +
							'<tbody>' +
								'<tr>' +
									'<td><input type="button" id="autodl-filters-match-sites-button" class="Button" value="' + theUILang.autodlMatchSites + '" /></td>' +
									'<td><input type="text" class="textbox-23" id="autodl-filters-match-sites" title="' + theUILang.autodlTitle4 + '" placeholder="' + theUILang.autodlHint4 + '"/></td>' +
								'</tr>' +
								'<tr>' +
									'<td><input type="button" id="autodl-filters-except-sites-button" class="Button" value="' + theUILang.autodlExceptSites + '" /></td>' +
									'<td><input type="text" class="textbox-23" id="autodl-filters-except-sites" title="' + theUILang.autodlTitle24 + '" placeholder="' + theUILang.autodlHint23 + '"/></td>' +
								'</tr>' +
								'<tr>' +
									'<td><label for="autodl-filters-min-size">' + theUILang.autodlMinimumSize + '</label></td>' +
									'<td><input type="text" class="textbox-16" id="autodl-filters-min-size" title="' + theUILang.autodlTitle5 + '" placeholder="' + theUILang.autodlHint5 + '"/></td>' +
								'</tr>' +
								'<tr>' +
									'<td><label for="autodl-filters-max-size">' + theUILang.autodlMaximumSize + '</label></td>' +
									'<td><input type="text" class="textbox-16" id="autodl-filters-max-size" title="' + theUILang.autodlTitle6 + '" placeholder="' + theUILang.autodlHint6 + '"/></td>' +
								'</tr>' +
							'</tbody>' +
						'</table>' +
						'<table>' +
							'<tbody>' +
								'<tr>' +
									'<td>' +
										'<label for="autodl-filters-upload-delay-secs">' + theUILang.autodlUploadDelay + '</label>' +
										'<input type="text" class="textbox-5" id="autodl-filters-upload-delay-secs" title="' + theUILang.autodlUploadDelay2 + '" />' +
									'</td>' +
								'</tr>' +
								'<tr>' +
									'<td>' +
										'<label for="autodl-filters-max-downloads">' + theUILang.autodlMaxDownloads + '</label>' +
										'<input type="text" class="textbox-5" id="autodl-filters-max-downloads" title="' + theUILang.autodlMaxDownloadsTitle + '" />' +
										'<label for="autodl-filters-max-downloads-per">' + theUILang.autodlMaxDownloads2 + '</label>' +
										'<select id="autodl-filters-max-downloads-per" />' +
										'<label for="autodl-filters-max-downloads-per">' + theUILang.autodlMaxDownloads3 + '</label>' +
									'</td>' +
								'</tr>' +
								'<tr>' +
									'<td>' +
										'<input type="checkbox" id="autodl-filters-download-duplicates" title="' + theUILang.autodlTitle33 + '"/>' +
										'<label for="autodl-filters-download-duplicates" title="' + theUILang.autodlTitle33 + '">' + theUILang.autodlDownloadDupes + '</label>' +
									'</td>' +
								'</tr>' +
							'</tbody>' +
						'</table>' +
					'</div>' +
				'<div id="autodl-filters-contents">' +
					'<div id="autodl-filters-contents-p2p">' +
						'<table>' +
							'<tbody>' +
								'<tr>' +
									'<td><label for="autodl-filters-match-releases">' + theUILang.autodlMatchReleases + '</label></td>' +
									'<td><input type="text" class="textbox-23" id="autodl-filters-match-releases" title="' + theUILang.autodlTitle2 + '" placeholder="' + theUILang.autodlHint2 + '"/></td>' +
								'</tr>' +
								'<tr>' +
									'<td><label for="autodl-filters-except-releases">' + theUILang.autodlExceptReleases + '</label></td>' +
									'<td><input type="text" class="textbox-23" id="autodl-filters-except-releases" title="' + theUILang.autodlTitle3 + '" placeholder="' + theUILang.autodlHint3 + '"/></td>' +
								'</tr>' +
								'<tr>' +
									'<td>' +
										'<input type="checkbox" id="autodl-filters-use-regex" title="' + theUILang.autodlTitle63 + '"/>' +
										'<label for="autodl-filters-use-regex" title="' + theUILang.autodlTitle63 + '">' + theUILang.autodlFiltersUseRegex + '</label>' +
									'</td>' +
								'</tr>' +
							'</tbody>' +
						'</table>' +
						'<hr>' +
						'<table>' +
							'<tbody>' +
								'<tr>' +
									'<td><label for="autodl-filters-match-release-groups">' + theUILang.autodlMatchReleaseGroups + '</label></td>' +
									'<td><input type="text" class="textbox-23" id="autodl-filters-match-release-groups" title="' + theUILang.autodlTitle65 + '" placeholder="' + theUILang.autodlHint38 + '"/></td>' +
								'</tr>' +
								'<tr>' +
									'<td><label for="autodl-filters-except-release-groups">' + theUILang.autodlExceptReleaseGroups + '</label></td>' +
									'<td><input type="text" class="textbox-23" id="autodl-filters-except-release-groups" title="' + theUILang.autodlTitle65 + '" placeholder="' + theUILang.autodlHint38 + '"/></td>' +
								'</tr>' +
								'<tr>' +
									'<td><label for="autodl-filters-max-pretime">' + theUILang.autodlMaxPreTime + '</label></td>' +
									'<td><input type="text" class="textbox-23" id="autodl-filters-max-pretime" title="' + theUILang.autodlTitle25 + '" placeholder="' + theUILang.autodlHint24 + '"/></td>' +
								'</tr>' +
							'</tbody>' +
						'</table>' +
						'<table>' +
							'<tr>' +
								'<td>' +
									'<label for="autodl-filters-scene">' + theUILang.autodlScene + '</label>' +
									'<select id="autodl-filters-scene" />' +
									'<label for="autodl-filters-origins">' + theUILang.autodlOrigins + '</label>' +
									'<input type="text" class="textbox-16" id="autodl-filters-origins" title="' + theUILang.autodlTitle60 + '"placeholder="' + theUILang.autodlHint35 + '"/>' +
								'</td>' +
							'</tr>' +
							'<tr>' +
								'<td>' +
									'<label for="autodl-filters-freeleech">' + theUILang.autodlFreeleech + '</label>' +
									'<select id="autodl-filters-freeleech" />' +
									'<label for="autodl-filters-feeleech-percents">' + theUILang.autodlFreeleechPercents + '</label>' +
									'<input type="text" class="textbox-16" id="autodl-filters-freeleech-percents" title="' + theUILang.autodlTitle58 + '" placeholder="' + theUILang.autodlHint33 + '"/>' +
								'</td>' +
							'</tr>' +
						'</table>' +
					'</div>' +
					'<div id="autodl-filters-contents-tv-movie">' +
						'<table>' +
							'<tbody>' +
								'<tr>' +
									'<td><label for="autodl-filters-shows">' + theUILang.autodlTvShowMovie + '</label></td>' +
									'<td><input type="text" class="textbox-23" id="autodl-filters-shows" title="' + theUILang.autodlTitle7 + '" placeholder="' + theUILang.autodlHint7 + '"/></td>' +
								'</tr>' +
							'</tbody>' +
						'</table>' +
						'<hr>' +
						'<table>' +
							'<tbody>' +
								'<tr>' +
									'<td><label for="autodl-filters-seasons">' + theUILang.autodlSeasons + '</label></td>' +
									'<td><input type="text" class="textbox-23" id="autodl-filters-seasons" title="' + theUILang.autodlTitle8 + '" placeholder="' + theUILang.autodlHint8 + '"/></td>' +
								'</tr>' +
								'<tr>' +
									'<td><label for="autodl-filters-episodes">' + theUILang.autodlEpisodes + '</label></td>' +
									'<td><input type="text" class="textbox-23" id="autodl-filters-episodes" title="' + theUILang.autodlTitle9 + '" placeholder="' + theUILang.autodlHint9 + '"/></td>' +
								'</tr>' +
								'<tr>' +
									'<td>' +
										'<input type="checkbox" id="autodl-filters-smart-episode" title="' + theUILang.autodlTitle67 + '"/>' +
										'<label for="autodl-filters-smart-episode" title="' + theUILang.autodlTitle67 + '">' + theUILang.autodlSmartEpisode + '</label>' +
									'</td>' +
								'</tr>' +
							'</tbody>' +
						'</table>' +
						'<hr>' +
						'<table>' +
							'<tbody>' +
								'<tr>' +
									'<td><input type="button" id="autodl-filters-resolutions-button" class="Button" value="' + theUILang.autodlResolutions + '" /></td>' +
									'<td><input type="text" class="textbox-23" id="autodl-filters-resolutions" title="' + theUILang.autodlTitle10 + '" placeholder="' + theUILang.autodlHint10 + '"/></td>' +
								'</tr>' +
								'<tr>' +
									'<td><input type="button" id="autodl-filters-encoders-button" class="Button" value="' + theUILang.autodlEncoders + '" /></td>' +
									'<td><input type="text" class="textbox-23" id="autodl-filters-encoders" title="' + theUILang.autodlTitle11 + '" placeholder="' + theUILang.autodlHint11 + '"/></td>' +
								'</tr>' +
								'<tr>' +
									'<td><input type="button" id="autodl-filters-sources-button" class="Button" value="' + theUILang.autodlSources + '" /></td>' +
									'<td><input type="text" class="textbox-23" id="autodl-filters-sources" title="' + theUILang.autodlTitle12 + '" placeholder="' + theUILang.autodlHint12 + '"/></td>' +
								'</tr>' +
								'<tr>' +
									'<td><label for="autodl-filters-containers">' + theUILang.autodlContainers + '</label></td>' +
									'<td><input type="text" class="textbox-23" id="autodl-filters-containers" title="' + theUILang.autodlTitle62 + '" placeholder="' + theUILang.autodlHint36 + '"/></td>' +
								'</tr>' +
								'<tr>' +
									'<td><label for="autodl-filters-years1">' + theUILang.autodlYears + '</label></td>' +
									'<td><input type="text" class="textbox-23" id="autodl-filters-years1" title="' + theUILang.autodlTitle13 + '" placeholder="' + theUILang.autodlHint13 + '"/></td>' +
								'</tr>' +
							'</tbody>' +
						'</table>' +
					'</div>' +
					'<div id="autodl-filters-contents-music">' +
						'<table>' +
							'<tbody>' +
								'<tr>' +
									'<td><label for="autodl-filters-years2">' + theUILang.autodlYears + '</label></td>' +
									'<td><input type="text" class="textbox-23" id="autodl-filters-years2" title="' + theUILang.autodlTitle13 + '" placeholder="' + theUILang.autodlHint13 + '"/></td>' +
								'</tr>' +
								'<tr>' +
									'<td><label for="autodl-filters-artists">' + theUILang.autodlArtists + '</label></td>' +
									'<td><input type="text" class="textbox-23" id="autodl-filters-artists" title="' + theUILang.autodlTitle14 + '" placeholder="' + theUILang.autodlHint14 + '"/></td>' +
								'</tr>' +
								'<tr>' +
									'<td><label for="autodl-filters-albums">' + theUILang.autodlAlbums + '</label></td>' +
									'<td><input type="text" class="textbox-23" id="autodl-filters-albums" title="' + theUILang.autodlTitle15 + '" placeholder="' + theUILang.autodlHint15 + '"/></td>' +
								'</tr>' +
								'<tr>' +
									'<td><input type="button" id="autodl-filters-formats-button" class="Button" value="' + theUILang.autodlFormats + '" /></td>' +
									'<td><input type="text" class="textbox-23" id="autodl-filters-formats" title="' + theUILang.autodlTitle16 + '" placeholder="' + theUILang.autodlHint16 + '"/></td>' +
								'</tr>' +
								'<tr>' +
									'<td><input type="button" id="autodl-filters-bitrates-button" class="Button" value="' + theUILang.autodlBitrates + '" /></td>' +
									'<td><input type="text" class="textbox-23" id="autodl-filters-bitrates" title="' + theUILang.autodlTitle17 + '" placeholder="' + theUILang.autodlHint17 + '"/></td>' +
								'</tr>' +
								'<tr>' +
									'<td><input type="button" id="autodl-filters-media-button" class="Button" value="' + theUILang.autodlMedia + '" /></td>' +
									'<td><input type="text" class="textbox-23" id="autodl-filters-media" title="' + theUILang.autodlTitle18 + '" placeholder="' + theUILang.autodlHint18 + '"/></td>' +
								'</tr>' +
								'<tr>' +
									'<td><label for="autodl-filters-match-release-types">' + theUILang.autodlMatchReleaseTypes + '</label></td>' +
									'<td><input type="text" class="textbox-23" id="autodl-filters-match-release-types" title="' + theUILang.autodlTitle64 + '" placeholder="' + theUILang.autodlHint37 + '"/></td>' +
								'</tr>' +
								'<tr>' +
									'<td><label for="autodl-filters-except-release-types">' + theUILang.autodlExceptReleaseTypes + '</label></td>' +
									'<td><input type="text" class="textbox-23" id="autodl-filters-except-release-types" title="' + theUILang.autodlTitle64 + '" placeholder="' + theUILang.autodlHint37 + '"/></td>' +
								'</tr>' +
							'</tbody>' +
						'</table>' +
						'<table>' +
							'<tr>' +
								'<td>' +
									'<label for="autodl-filters-cue">Cue</label>' +
									'<select id="autodl-filters-cue" />' +
								'</td>' +
							'</tr>' +
							'<tr>' +
								'<td>' +
									'<label for="autodl-filters-log">Log</label>' +
									'<select id="autodl-filters-log" />' +
									'<label for="autodl-filters-log-scores">' + theUILang.autodlLogScores + '</label>' +
									'<input type="text" class="textbox-16" id="autodl-filters-log-scores" title="' + theUILang.autodlTitle59 + '" placeholder="' + theUILang.autodlHint34 + '"/>' +
								'</td>' +
							'</tr>' +
						'</table>' +
					'</div>' +
					'<div id="autodl-filters-contents-advanced">' +
						'<table>' +
							'<tbody>' +
								'<tr>' +
									'<td><label for="autodl-filters-match-categories">' + theUILang.autodlMatchCategories + '</label></td>' +
									'<td><input type="text" class="textbox-20" id="autodl-filters-match-categories" title="' + theUILang.autodlTitle20 + '" placeholder="' + theUILang.autodlHint19 + '"/></td>' +
								'</tr>' +
								'<tr>' +
									'<td><label for="autodl-filters-except-categories">' + theUILang.autodlExceptCategories + '</label></td>' +
									'<td><input type="text" class="textbox-20" id="autodl-filters-except-categories" title="' + theUILang.autodlTitle21 + '" placeholder="' + theUILang.autodlHint20 + '"/></td>' +
								'</tr>' +
								'<tr>' +
									'<td><label for="autodl-filters-match-uploaders">' + theUILang.autodlMatchUploaders + '</label></td>' +
									'<td><input type="text" class="textbox-20" id="autodl-filters-match-uploaders" title="' + theUILang.autodlTitle22 + '" placeholder="' + theUILang.autodlHint21 + '"/></td>' +
								'</tr>' +
								'<tr>' +
									'<td><label for="autodl-filters-except-uploaders">' + theUILang.autodlExceptUploaders + '</label></td>' +
									'<td><input type="text" class="textbox-20" id="autodl-filters-except-uploaders" title="' + theUILang.autodlTitle23 + '" placeholder="' + theUILang.autodlHint22 + '"/></td>' +
								'</tr>' +
								'<tr>' +
									'<td><label for="autodl-filters-tags">' + theUILang.autodlTags + '</label></td>' +
									'<td>' +
										'<input type="text" class="textbox-16" id="autodl-filters-tags" title="' + theUILang.autodlTitle19 + '" placeholder="' + theUILang.autodlHint25 + '"/>' +
										'<select id="autodl-filters-tags-any" />' +
									'</td>' +
								'</tr>' +
								'<tr>' +
									'<td><label for="autodl-filters-except-tags">' + theUILang.autodlExceptTags + '</label></td>' +
									'<td>' +
										'<input type="text" class="textbox-16" id="autodl-filters-except-tags" title="' + theUILang.autodlTitle57 + '" placeholder="' + theUILang.autodlHint25 + '"/>' +
										'<select id="autodl-filters-except-tags-any" />' +
									'</td>' +
								'</tr>' +
							'</tbody>' +
						'</table>' +
					'</div>' +
					'<div id="autodl-filters-contents-upload"/>' +
					'<div id="autodl-filters-contents-wol">' +
						'<p>' +
							theUILang.autodlWolDesc +
						'</p>' +
						'<table>' +
							'<tbody>' +
								'<tr>' +
									'<td><label for="autodl-filters-wol-mac">' + theUILang.autodlWolMac + '</label></td>' +
									'<td><input type="text" class="textbox-23" id="autodl-filters-wol-mac" title="' + theUILang.autodlWolMacTitle + '" placeholder="' + theUILang.autodlWolMacEmpty + '"/></td>' +
								'</tr>' +
								'<tr>' +
									'<td><label for="autodl-filters-wol-ip">' + theUILang.autodlWolIp + '</label></td>' +
									'<td><input type="text" class="textbox-23" id="autodl-filters-wol-ip" title="' + theUILang.autodlWolIpTitle + '" placeholder="' + theUILang.autodlWolIpEmpty + '"/></td>' +
								'</tr>' +
								'<tr>' +
									'<td><label for="autodl-filters-wol-port">' + theUILang.autodlWolPort + '</label></td>' +
									'<td><input type="text" class="textbox-23" id="autodl-filters-wol-port" title="' + theUILang.autodlWolPortTitle + '" placeholder="' + theUILang.autodlWolPortEmpty + '"/></td>' +
								'</tr>' +
							'</tbody>' +
						'</table>' +
					'</div>' +
				'</div>' +
			'</div>' +
			'<div class="aright buttons-list dialog-buttons">' +
				'<input type="button" id="autodl-filters-ok-button" value="' + theUILang.ok + '" class="OK Button" />' +
				'<input type="button" id="autodl-filters-cancel-button" value="' + theUILang.Cancel + '" class="Cancel Button" />' +
			'</div>' +
		'</div>'
	);

	this.options =
	[
		new DialogOptionText("autodl-filters-match-releases", "match-releases", ""),
		new DialogOptionText("autodl-filters-except-releases", "except-releases", ""),
		new DialogOptionText("autodl-filters-match-release-groups", "match-release-groups", ""),
		new DialogOptionText("autodl-filters-except-release-groups", "except-release-groups", ""),
		new DialogOptionText("autodl-filters-match-categories", "match-categories", ""),
		new DialogOptionText("autodl-filters-except-categories", "except-categories", ""),
		new DialogOptionText("autodl-filters-match-sites", "match-sites", ""),
		new DialogOptionText("autodl-filters-except-sites", "except-sites", ""),
		new DialogOptionText("autodl-filters-min-size", "min-size", ""),
		new DialogOptionText("autodl-filters-max-size", "max-size", ""),
		new DialogOptionText("autodl-filters-shows", "shows", ""),
		new DialogOptionText("autodl-filters-artists", "shows", ""),
		new DialogOptionText("autodl-filters-seasons", "seasons", ""),
		new DialogOptionText("autodl-filters-episodes", "episodes", ""),
		new DialogOptionBool("autodl-filters-smart-episode", "smart-episode", "false"),
		new DialogOptionText("autodl-filters-resolutions", "resolutions", ""),
		new DialogOptionText("autodl-filters-sources", "sources", ""),
		new DialogOptionText("autodl-filters-years1", "years", ""),
		new DialogOptionText("autodl-filters-years2", "years", ""),
		new DialogOptionText("autodl-filters-encoders", "encoders", ""),
		new DialogOptionText("autodl-filters-albums", "albums", ""),
		new DialogOptionText("autodl-filters-formats", "formats", ""),
		new DialogOptionText("autodl-filters-bitrates", "bitrates", ""),
		new DialogOptionText("autodl-filters-media", "media", ""),
		new DialogOptionText("autodl-filters-match-release-types", "match-release-types", ""),
		new DialogOptionText("autodl-filters-except-release-types", "except-release-types", ""),
		new DialogOptionText("autodl-filters-tags", "tags", ""),
		new DialogOptionText("autodl-filters-except-tags", "except-tags", ""),
		new DialogOptionText("autodl-filters-match-uploaders", "match-uploaders", ""),
		new DialogOptionText("autodl-filters-except-uploaders", "except-uploaders", ""),
		new DialogOptionText("autodl-filters-max-pretime", "max-pretime", ""),
		new DialogOptionText("autodl-filters-max-downloads", "max-downloads", ""),
		new DialogOptionText("autodl-filters-wol-mac", "wol-mac-address", ""),
		new DialogOptionText("autodl-filters-wol-ip", "wol-ip-address", ""),
		new DialogOptionInt("autodl-filters-wol-port", "wol-port", "9"),
		new DialogOptionText("autodl-filters-freeleech-percents", "freeleech-percents", ""),
		new DialogOptionText("autodl-filters-log-scores", "log-scores", ""),
		new DialogOptionText("autodl-filters-origins", "origins", ""),
		new DialogOptionText("autodl-filters-containers", "containers", ""),
		new DialogOptionText("autodl-filters-upload-delay-secs", "upload-delay-secs", ""),
		new DialogOptionBool("autodl-filters-download-duplicates", "download-duplicates", "false"),
		new DialogOptionBool("autodl-filters-use-regex", "use-regex", "false"),
		new DialogOptionInt("autodl-filters-priority", "priority", "0")
	];

	var this_ = this;

	$("#autodl-filters-new-button").click(function(e)
	{
		this_._onClickNew();
	});
	$("#autodl-filters-remove-button").click(function(e)
	{
		this_._onClickRemove();
	});
	$("#autodl-filters-name").keyup(function(e)
	{
		this_._onFilterNameModified();
	});

	this.syncName1 = new SyncTextBoxes(["autodl-filters-shows", "autodl-filters-artists"]);
	this.syncYears = new SyncTextBoxes(["autodl-filters-years1", "autodl-filters-years2"]);

	this.resolutionsButton = new MenuButton("autodl-filters-resolutions-button", "autodl-filters-resolutions", tvResolutions, undefined, false);
	this.encodersButton = new MenuButton("autodl-filters-encoders-button", "autodl-filters-encoders", tvEncoders);
	this.sourcesButton = new MenuButton("autodl-filters-sources-button", "autodl-filters-sources", tvSources);
	this.formatsButton = new MenuButton("autodl-filters-formats-button", "autodl-filters-formats", musicFormats);
	this.bitratesButton = new MenuButton("autodl-filters-bitrates-button", "autodl-filters-bitrates", musicBitrates, true);
	this.mediaButton = new MenuButton("autodl-filters-media-button", "autodl-filters-media", musicMedia);

	this.matchSitesButton = new SitesButton("autodl-filters-match-sites-button", "autodl-filters-match-sites", multiSelectDlgBox);
	this.exceptSitesButton = new SitesButton("autodl-filters-except-sites-button", "autodl-filters-except-sites", multiSelectDlgBox);

	this.tabs = new Tabs();
	this.tabs.add("autodl-filters-tab-general", "autodl-filters-contents-general");
	this.tabs.add("autodl-filters-tab-p2p", "autodl-filters-contents-p2p");
	this.tabs.add("autodl-filters-tab-tv-movie", "autodl-filters-contents-tv-movie");
	this.tabs.add("autodl-filters-tab-music", "autodl-filters-contents-music");
	this.tabs.add("autodl-filters-tab-advanced", "autodl-filters-contents-advanced");
	this.tabs.add("autodl-filters-tab-upload", "autodl-filters-contents-upload");
	this.tabs.add("autodl-filters-tab-wol", "autodl-filters-contents-wol");

	var names = ["scene", "log", "cue", "freeleech"];
	for (var i = 0; i < names.length; i++)
	{
		var dropdown = new DropDownBox('autodl-filters-' + names[i]);
		this[names[i] + "DropDownBox"] = dropdown;
		dropdown.add("", theUILang.autodlDontCare);
		dropdown.add("true", theUILang.autodlYes);
		dropdown.add("false", theUILang.autodlNo);
	}

	this.tagsDropdownBox = new AnyAllDropDown("autodl-filters-tags-any", "tags-any");
	this.exceptTagsDropdownBox = new AnyAllDropDown("autodl-filters-except-tags-any", "except-tags-any");

	this.maxDlsDropdown = new DropDownBox("autodl-filters-max-downloads-per");
	this.maxDlsDropdown.add("", theUILang.autodlMaxDlsNone);
	this.maxDlsDropdown.add("forever", theUILang.autodlMaxDlsForever);
	this.maxDlsDropdown.add("hour", theUILang.autodlMaxDlsHour);
	this.maxDlsDropdown.add("day", theUILang.autodlMaxDlsDay);
	this.maxDlsDropdown.add("week", theUILang.autodlMaxDlsWeek);
	this.maxDlsDropdown.add("month", theUILang.autodlMaxDlsMonth);

	this.filterListBox = new ListBox("autodl-filters-list");
	this.filterListBox.onSelected = function(oldObj, newObj) { this_._onFilterSelected(oldObj, newObj); }

	this.uploadMethod = new UploadMethod("autodl-filters", "autodl-filters-contents-upload");

	$("#autodl-filters-ok-button").click(function(e) { okHandler() });
}

Filters.prototype.onBeforeShow =
function(configFile, trackerInfos, trackersId)
{
	this.configFile = configFile;

	this.uploadMethod.onBeforeShow();
	this.matchSitesButton._setTrackerInfos(trackerInfos);
	this.exceptSitesButton._setTrackerInfos(trackerInfos);
	this.initFilters();
}

Filters.prototype.onAfterHide =
function()
{
	this.uploadMethod.onAfterHide();
	this.matchSitesButton._setTrackerInfos(null);
	this.exceptSitesButton._setTrackerInfos(null);
	this.filterListBox.removeAll();
	delete this.filterObjs;
	this.configFile = null;
}

Filters.prototype.onOkClicked =
function()
{
	var obj = this.filterListBox.getSelectedData();
	this._saveFilterObj(obj);
	this.lastSelectedFilterName = obj ? obj.section.name : null;

	var filters = [];
	for (var i = 0; i < this.filterObjs.length; i++)
		filters.push(this.filterObjs[i].section);
	this.configFile.setFilters(filters);

	return true;	// autodl.cfg updated
}

Filters.prototype.initFilters =
function()
{
	this.nextId = 0;
	this.filterObjs = [];
	var ary = this.configFile.getFilters();
	ary.sort(function(a, b)
	{
		return stringCompare(a.name.toLowerCase(), b.name.toLowerCase());
	});
	for (var i = 0; i < ary.length; i++)
		this._addFilterSection(ary[i].clone());

	if (this.filterObjs.length === 0)
		this._onFilterSelected();
	else
	{
		var index = this._findIndexByName(this.lastSelectedFilterName);
		index = index < 0 ? 0 : index;
		this.filterListBox.select(index);
	}
}

Filters.prototype._fixFilterName =
function(name)
{
	return name || theUILang.autodlNoName;
}

Filters.prototype._findIndexByName =
function(name)
{
	if (name == null)
		return -1;

	for (var i = 0; i < this.filterObjs.length; i++)
	{
		if (this.filterObjs[i].section.name === name)
			return i;
	}

	return -1;
}

Filters.prototype._addFilterSection =
function(section)
{
	var obj =
	{
		section: section,
		idNum: this.nextId
	};
	obj.checkboxElem = $('<input type="checkbox" />')[0];
	obj.labelElem = $('<label />').text(this._fixFilterName(section.name))[0];

	if (section.getOption("enabled", "true", "bool").getValue())
		$(obj.checkboxElem).attr("checked", "checked");

	this.filterListBox.append($(obj.checkboxElem).add(obj.labelElem), obj);

	this.filterObjs.push(obj);
	this.nextId++;
	return obj;
}

Filters.prototype._saveFilterObj =
function(obj)
{
	if (obj)
	{
		var section = obj.section;

		this.uploadMethod.saveOptions(section);
		saveDialogOptions(section, this.options);
		section.getOption("scene").setValue(this.sceneDropDownBox.getSelectedValue());
		section.getOption("log").setValue(this.logDropDownBox.getSelectedValue());
		section.getOption("cue").setValue(this.cueDropDownBox.getSelectedValue());
		section.getOption("freeleech").setValue(this.freeleechDropDownBox.getSelectedValue());

		section.name = $("#autodl-filters-name").val();

		var enabled = obj.checkboxElem.checked;
		section.getOption("enabled").setValue(enabled.toString());

		section.getOption("max-downloads-per").setValue(this.maxDlsDropdown.getSelectedValue());

		this.tagsDropdownBox.save(section);
		this.exceptTagsDropdownBox.save(section);
	}
}

Filters.prototype._onFilterSelected =
function(oldObj, newObj)
{
	this._saveFilterObj(oldObj);

	this.lastSelectedFilterName = newObj ? newObj.section.name : null;

	var section = (newObj || {}).section;
	initDialogOptions(section, this.options);
	this.uploadMethod.initFields(section);
	$("#autodl-filters-name").val(section ? section.name : "");

	function setDropDown(dropdown, name)
	{
		var val = getSectionOptionValue(section, name, "", "text");
		if (val !== "")
			val = convertStringToBoolean(val, "");
		dropdown.select(val.toString());
	}
	setDropDown(this.sceneDropDownBox, "scene");
	setDropDown(this.logDropDownBox, "log");
	setDropDown(this.cueDropDownBox, "cue");
	setDropDown(this.freeleechDropDownBox, "freeleech");

	this.maxDlsDropdown.select(getSectionOptionValue(section, "max-downloads-per", "", "text"));

	this.tagsDropdownBox.initialize(section);
	this.exceptTagsDropdownBox.initialize(section);

	var enable = newObj != null;
	var elems = $("#autodl-filters-remove-button").
				add($("input, select", $("#autodl-filters-right")[0]).
				not("#autodl-filters-ok-button, #autodl-filters-cancel-button"));
	enableJqueryElem(elems, enable);
	if (enable)
		this.uploadMethod.reenableElements();
}

Filters.prototype._onClickNew =
function()
{
	var obj = this._addFilterSection(new ConfigSection(null, "filter", ""));
	this.filterListBox.selectData(obj);
	this.tabs.selectByIndex(0);
	$("#autodl-filters-name").focus();
}

Filters.prototype._onClickRemove =
function()
{
	if (confirm(theUILang.autodlDeleteFilter))
	{
		var selectedIndex = this.filterListBox.getSelectedIndex();
		this.filterObjs.splice(selectedIndex, 1);
		this.filterListBox.removeSelected();
	}
}

Filters.prototype._onFilterNameModified =
function()
{
	var obj = this.filterListBox.getSelectedData();
	if (!obj)
		return;
	var newText = this._fixFilterName($("#autodl-filters-name").val())
	$(obj.labelElem).text(newText);
}
