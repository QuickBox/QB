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

function Help()
{
}

Help.prototype.createDialogBox =
function(multiSelectDlgBox, okHandler)
{
	theDialogManager.make("autodl-help", theUILang.autodlHelp,
		'<div id="autodl-help">' +
			'<div align="center">' +
				'<h2><u>Wiki</u></h2>' +
				'<p><a href="https://github.com/autodl-community/autodl-irssi/wiki" target="_blank">autodl-irssi</a></p>' +
				'<p><a href="https://github.com/autodl-community/autodl-rutorrent/wiki" target="_blank">autodl-rutorrent</a></p>' +
				'<h2><u>Discord</u></h2>' +
				'<p><a href="http://discord.autodl.community" target="_blank">Chat on Discord</a></p>' +
				'<h2><u>IRC</u></h2>' +
				'<p>#autodl-community on irc.p2p-network.net</p>' +
				'<p><a href="http://widget.mibbit.com/?settings=acc887926984c5c6e62412b13d9fbfbc&server=irc.p2p-network.net&channel=%23autodl-community&nick=autodler%3F%3F" target="_blank">Chat with Mibbit</a></p>' +
			'</div>' +
		'</div>'
	);
}

Help.prototype.onBeforeShow =
function(configFile, trackerInfos, trackersId)
{
}

Help.prototype.onAfterHide =
function()
{
}

Help.prototype.onOkClicked =
function()
{
	return false;
}
