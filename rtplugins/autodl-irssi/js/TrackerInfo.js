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

// Parses a .tracker file and keeps the info we need

function getChildNodes(node, func)
{
	var rv = [];

	for (var i = 0; i < node.childNodes.length; i++)
	{
		var child = node.childNodes[i];
		if (func(child))
			rv.push(child);
	}

	return rv;
}

function getChildElementsByTagName(node, name)
{
	return getChildNodes(node, function(child)
	{
		return child.nodeType === 1 && child.nodeName === name;
	});
}

function getChildElements(node)
{
	return getChildNodes(node, function(child)
	{
		return child.nodeType === 1;
	});
}

function getTheChildElement(elem, childElemName)
{
    var ary = getChildElementsByTagName(elem, childElemName);
	if (ary.length !== 1)
	    throw "Could not find one and only one child element named '" + childElemName + "'";
    return ary[0];
}

function appendTextElement(doc, elem, childElemName, textValue)
{
	var child = doc.createElement(childElemName);
	child.appendChild(doc.createTextNode(textValue));
	elem.appendChild(child);
}

function readTextNode(elem, childElemName, defaultValue)
{
	var ary = getChildElementsByTagName(elem, childElemName);
	if (ary.length === 0)
		return defaultValue;

	var elem = ary[0];
	var child = elem.firstChild;
	if (!child || child.nodeType !== 3)
		return defaultValue;

	// FireFox has a 4K limit on text nodes! It will split them up into multiple text
	// nodes, so we must combine them all.
	var s = "";
	for (var i = 0; i < elem.childNodes.length; i++)
	{
		var childNode = elem.childNodes[i];
		var nodeType = childNode.nodeType;
		if (nodeType === 3 || nodeType === 4)
			s += childNode.nodeValue;
	}

	// Trim the string, including newlines
	s = s.replace(/^\s+/m, "");
	s = s.replace(/\s+$/m, "");
	return s;
}

function readTextNodeBoolean(elem, childElemName, defaultValue)
{
	var strVal = readTextNode(elem, childElemName);
	if (strVal === undefined)
		return defaultValue;
	return convertStringToBoolean(strVal);
}

function readTextNodeNumber(elem, childElemName, defaultValue, minValue, maxValue)
{
	var strVal = readTextNode(elem, childElemName);
	if (strVal === undefined)
		return defaultValue;
	return convertStringToNumber(strVal, defaultValue, minValue, maxValue);
}

function readTextNodeInteger(elem, childElemName, defaultValue, minValue, maxValue)
{
	var strVal = readTextNode(elem, childElemName);
	if (strVal === undefined)
		return defaultValue;
	return convertStringToInteger(strVal, defaultValue, minValue, maxValue);
}

function readAttribute(elem, attrName, defaultValue)
{
	var attr = elem.attributes.getNamedItem(attrName);
	return attr === null ? defaultValue : attr.nodeValue;
}

function readAttributeBoolean(elem, attrName, defaultValue)
{
	var strVal = readAttribute(elem, attrName)
	if (strVal === undefined)
		return defaultValue;
	return convertStringToBoolean(strVal);
}

function readAttributeNumber(elem, attrName, defaultValue, minValue, maxValue)
{
	var strVal = readAttribute(elem, attrName)
	if (strVal === undefined)
		return defaultValue;
	return convertStringToNumber(strVal, defaultValue, minValue, maxValue);
}

function readAttributeInteger(elem, attrName, defaultValue, minValue, maxValue)
{
	var strVal = readAttribute(elem, attrName)
	if (strVal === undefined)
		return defaultValue;
	return convertStringToInteger(strVal, defaultValue, minValue, maxValue);
}

function TrackerInfo(xmlDoc)
{
	this._parse(xmlDoc);
}

TrackerInfo.prototype._parse =
function(doc)
{
	var trackerInfoElem = getTheChildElement(doc, "trackerinfo");

	this.type = readAttribute(trackerInfoElem, "type");
	this.shortName = readAttribute(trackerInfoElem, "shortName");
	this.longName = readAttribute(trackerInfoElem, "longName");
	this.siteName = readAttribute(trackerInfoElem, "siteName");
	if (this.type == null || this.shortName == null || this.longName == null || this.siteName == null)
		throw "Invalid tracker file";

	this.settings = this._parseSettings(trackerInfoElem);
	this.servers = this._parseServers(trackerInfoElem);
}

TrackerInfo.prototype._parseSettings =
function(trackerInfoElem)
{
	var settings = getTheChildElement(trackerInfoElem, "settings");
	var children = getChildElements(settings);
	if (children.length === 0)
		throw "No settings found";

	var rv = [];

	var this_ = this;
	function addIt(setting, elemName)
	{
		this_._initializeSetting(setting, elemName);
		rv.push(setting);
	}

	addIt(
	{
		name: "enabled",
		type: "bool",
		defaultValue: "true",
		text: theUILang.autodlEnabled,
		tooltiptext: theUILang.autodlEnableAnnouncer,
		isDownloadVar: false
	});

	for (var i = 0; i < children.length; i++)
	{
		var elem = children[i];

		var setting =
		{
			name:			readAttribute(elem, "name"),
			type:			readAttribute(elem, "type"),
			defaultValue:	readAttribute(elem, "defaultValue", ""),
			text:			readAttribute(elem, "text"),
			placeholder:	readAttribute(elem, "placeholder"),
			tooltiptext:	readAttribute(elem, "tooltiptext"),
			pasteGroup:		readAttribute(elem, "pasteGroup"),
			pasteRegex:		readAttribute(elem, "pasteRegex"),
			minValue:		readAttribute(elem, "minValue"),
			maxValue:		readAttribute(elem, "maxValue"),
			isDownloadVar:	readAttributeBoolean(elem, "isDownloadVar", true)
		};

		addIt(setting, elem.nodeName);
	}

	addIt(
	{
		name: "upload-delay-secs",
		type: "integer",
		defaultValue: "0",
		text: theUILang.autodlUploadDelay,
		tooltiptext: theUILang.autodlUploadDelay2,
		isDownloadVar: false,
		minValue: "0"
	});
	addIt(
	{
		name: "force-ssl",
		type: "bool",
		defaultValue: "false",
		text: theUILang.autodlForceSsl,
		tooltiptext: theUILang.autodlForceSsl2,
		isDownloadVar: false
	});

	return rv;
}

TrackerInfo.prototype._initializeSetting =
function(setting, elemName)
{
	function setProp(name, value)
	{
		if (setting[name] === undefined)
			setting[name] = value;
	}	

	if (elemName === "gazelle_description")
	{
		setProp("type", "description");
		setProp("text", theUILang.autodlPaste1 + this.longName + theUILang.autodlPaste2);
	}
	else if (elemName === "gazelle_authkey")
	{
		setProp("type", "textbox");
		setProp("name", "authkey");
		setProp("text", "authkey");
		setProp("placeholder", this.longName + " " + setting.name);
		setProp("tooltiptext", theUILang.autodlAuthkey1 + this.longName + theUILang.autodlAuthkey2);
		setProp("pasteGroup", "authkey,torrent_pass");
		setProp("pasteRegex", "[\\?&]authkey=([\\da-zA-Z]{32})");
	}
	else if (elemName === "gazelle_torrent_pass")
	{
		setProp("type", "textbox");
		setProp("name", "torrent_pass");
		setProp("text", "torrent_pass");
		setProp("placeholder", this.longName + " " + setting.name);
		setProp("tooltiptext", theUILang.autodlTorrentPass1 + this.longName + theUILang.autodlTorrentPass2);
		setProp("pasteGroup", "authkey,torrent_pass");
		setProp("pasteRegex", "[\\?&]torrent_pass=([\\da-zA-Z]{32})");
	}
	else if (elemName === "description")
	{
		setProp("type", "description");
	}
	else if (elemName === "authkey")
	{
		setProp("type", "textbox");
		setProp("name", "authkey");
		setProp("text", "authkey");
		setProp("placeholder", this.longName + " " + setting.name);
		setProp("tooltiptext", theUILang.autodlAuthkey3 + this.longName + theUILang.autodlAuthkey4);
		setProp("pasteGroup", "authkey");
		setProp("pasteRegex", "[\\?&]authkey=([\\da-fA-F]{32})");
	}
	else if (elemName === "passkey")
	{
		setProp("type", "textbox");
		setProp("name", "passkey");
		setProp("text", "passkey");
		setProp("placeholder", this.longName + " passkey");
		setProp("tooltiptext", theUILang.autodlPasskey1 + this.longName + theUILang.autodlPasskey2);
		setProp("pasteGroup", "passkey");
		setProp("pasteRegex", "[\\?&]passkey=([\\da-fA-F]{32})");
	}
	else if (elemName === "cookie_description")
	{
		setProp("type", "description");
		setProp("text", theUILang.autodlCookie1);
	}
	else if (elemName === "cookie")
	{
		setProp("type", "textbox");
		setProp("name", "cookie");
		setProp("text", "Cookie");
		setProp("placeholder", this.longName + " " + setting.name);
		setProp("tooltiptext", theUILang.autodlCookie2 + this.longName + theUILang.autodlCookie3);
	}
	else if (elemName === "integer")
	{
		setProp("type", "integer");
		setProp("minValue", "-999999999");
	}
	else if (elemName === "delta")
	{
		setProp("type", "integer");
		setProp("name", "delta");
		setProp("text", theUILang.autodlIdDelta);
		setProp("minValue", "-999999999");
	}
	else if (elemName === "textbox")
	{
		setProp("type", "textbox");
		setProp("placeholder", this.longName + " " + setting.name);
	}

	if (setting.pasteRegex)
		setting.pasteRegex = new RegExp(setting.pasteRegex);
}

TrackerInfo.prototype._parseServers =
function(trackerInfoElem)
{
	var servers = getTheChildElement(trackerInfoElem, "servers");
	var children = getChildElementsByTagName(servers, "server");

	var rv = [];

	function fixList(s)
	{
		return s.split(/\s*,\s*/).join(", ");
	}

	for (var i = 0; i < children.length; i++)
	{
		var elem = children[i];

		var server =
		{
			network:		readAttribute(elem, "network", ""),
			serverNames:	fixList(readAttribute(elem, "serverNames", "")),
			channelNames:	fixList(readAttribute(elem, "channelNames", "")),
			announcerNames:	fixList(readAttribute(elem, "announcerNames", ""))
		};
		rv.push(server);
	}

	return rv;
}
