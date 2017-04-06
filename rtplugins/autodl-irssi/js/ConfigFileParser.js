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

// Parses the autodl.cfg file

function canonicalizeServerName(serverName)
{
	var ary = serverName.match(/^([^:]*)/);
	return ary[1].toLowerCase();
}

function sortObjectById(obj)
{
	var ary = [];
	for (var key in obj)
		ary.push(obj[key]);

	// Output in the same order we read it from the file
	ary.sort(function(a, b) { return stringCompare(a.id, b.id); });

	return ary;
}

function ConfigOption(id, name, value, defaultValue, optionType)
{
	this.type = "option";
	this.id = id;
	this.name = $.trim(name);
	this.value = $.trim(value);
	this.defaultValue = defaultValue;
	this.optionType = optionType;
}

ConfigOption.prototype.clone =
function()
{
	var clone = new ConfigOption(this.id, this.name, this.value, this.defaultValue, this.optionType);
	return clone;
}

ConfigOption.prototype._toString =
function()
{
	if (this.defaultValue != null)
	{
		var value = this.getValue();
		var defaultValue = this._getValue(this.optionType, this.defaultValue, this.defaultValue);
		if (value === defaultValue)
			return null;
	}
	return this.name + " = " + this.value;
}

ConfigOption.prototype.setDefaultValue =
function(defaultValue, optionType)
{
	if (defaultValue != null)
		this.defaultValue = defaultValue;
	if (optionType != null)
		this.optionType = optionType;
}

ConfigOption.prototype.getValue =
function()
{
	return this._getValue(this.optionType, this.value, this.defaultValue);
}

ConfigOption.prototype._getValue =
function(optionType, value, defaultValue)
{
	switch (optionType)
	{
	case "bool":
		return convertStringToBoolean(value);
	case "int":
		return convertStringToInteger(value, convertStringToInteger(defaultValue));
	case "text":
	default:
		return value;
	}
}

ConfigOption.prototype.setValue =
function(value)
{
	this.value = value.toString();
}

function ConfigComment(id, name, line)
{
	this.type = "comment";
	this.id = id;
	this.name = name;
	this.value = line;
}

ConfigComment.prototype.clone =
function()
{
	var clone = new ConfigComment(this.id, this.name, this.value);
	return clone;
}

ConfigComment.prototype._toString =
function()
{
	return this.value;
}

/**
 * @param id	Unique id
 * @param type	The section type, i.e., first part of [type name]
 * @param name	The section name (optional)
 */
function ConfigSection(id, type, name)
{
	this.id = id;
	this.type = $.trim(type);
	this.name = $.trim(name);
	this.nextId = 0;
	this.lines = {};
	this.printEmpty = true;
}

ConfigSection.prototype.dontPrintEmpty =
function()
{
	this.printEmpty = false;
}

ConfigSection.prototype.clone =
function()
{
	var clone = new ConfigSection(this.id, this.type, this.name);
	clone.nextId = this.nextId;
	for (var key in this.lines)
	{
		var line = this.lines[key];
		clone.lines[key] = line.clone();
	}
	return clone;
}

ConfigSection.prototype.addOption =
function(name, value)
{
	var option = new ConfigOption(this.nextId++, name, value);
	this.lines[option.name] = option;
}

ConfigSection.prototype.addComment =
function(line)
{
	var id = this.nextId++;
	var name = " comment " + id;
	var comment = new ConfigComment(id, name, line);
	this.lines[comment.name] = comment;
}

ConfigSection.prototype.getOption =
function(name, defaultValue, type)
{
	var hash = $.trim(name);
	var option = this.lines[hash];
	if (!option)
		this.lines[hash] = option = new ConfigOption(this.nextId++, name, defaultValue);
	option.setDefaultValue(defaultValue, type);
	return option;
}

ConfigSection.prototype._toString =
function()
{
	var out = "";

	out += "[" + this.type;
	if (this.name)
		out += " " + this.name;
	out += "]\n";

	var ary = sortObjectById(this.lines);
	var opts = "";
	for (var i = 0; i < ary.length; i++)
	{
		var s = ary[i]._toString();
		if (s != null)
			opts += s + "\n";
	}
	if (!this.printEmpty && !opts)
		return null;

	return out + opts;
}

function ConfigFile()
{
	this.sections = {};
	this.id = 0;
}

ConfigFile.prototype.parse =
function(contents)
{
	this.sections = {};
	this.filters = [];
	var section, sectionLine, ary;

	function error(msg)
	{
		log("autodl.cfg: line " + (i+1) + ": " + msg);
	}

	var lines = contents.split("\n");
	for (var i = 0; i < lines.length; i++)
	{
		var line = $.trim(lines[i]);
		if (line === "")
			continue;

		if (line.charAt(0) === "#")
		{
			if (section)
				section.addComment(line);
		}
		else if (ary = line.match(/^\[\s*([\w\-]+)\s*(?:([^\]]+))?\s*]$/))
		{
			var type = $.trim(ary[1]).toLowerCase();
			var name = $.trim(ary[2]);

			section = this.getSection(type, name);
		}
		else if (ary = line.match(/^([\w\-]+)\s*=(.*)$/))
		{
			if (!section)
				error("Missing a [section]");
			else
			{
				var option = $.trim(ary[1]).toLowerCase();
				var value = $.trim(ary[2]);
				section.addOption(option, value);
			}
		}
		else
		{
			error("Ignoring line");
		}
	}
}

ConfigFile.prototype.toString =
function()
{
	var out = "";

	var ary = sortObjectById(this.sections);
	for (var i = 0; i < ary.length; i++)
	{
		var section = ary[i];

		var s = section._toString();
		if (s == null)
			continue;

		if (out !== "")
			out += "\n";
		out += s;
	}

	return out;
}

ConfigFile.prototype._filterHash =
function(filterSection)
{
	return $.trim(filterSection.type) + " " + filterSection.id;
}

ConfigFile.prototype._serverHash =
function(serverName)
{
	return " server " + canonicalizeServerName(serverName);
}

ConfigFile.prototype._channelHash =
function(channelName)
{
	return " channel " + this.id++ + " " + channelName.toLowerCase();
}

ConfigFile.prototype.getSection =
function(type, name)
{
	switch (type)
	{
	case "filter":
		var section = new ConfigSection(this.id++, type, name);
		this.filters.push(section);
		var hash = this._filterHash(section);
		this.sections[hash] = section;
		return section;

	case "options":
	case "webui":
	case "ftp":
	case "irc":
		return this._getOrCreateSection(type, name, $.trim(type));

	case "tracker":
		return this._getOrCreateSection(type, name, $.trim(type) + " " + $.trim(name));

	case "server":
		return this._getOrCreateSection(type, name, this._serverHash($.trim(name)));

	case "channel":
		return this._getOrCreateSection(type, name, this._channelHash($.trim(name)));

	default:
		return this._getOrCreateSection(type, name, " unknown " + this.id++ + " " + $.trim(type) + " " + $.trim(name));
	}
}

ConfigFile.prototype._getOrCreateSection =
function(type, name, hash)
{
	var section = this.sections[hash];
	if (!section)
		this.sections[hash] = section = new ConfigSection(this.id++, type, name);
	return section;
}

ConfigFile.prototype.getFilters =
function()
{
	return this.filters;
}

ConfigFile.prototype._removeFilters =
function()
{
	for (var i = 0; i < this.filters.length; i++)
	{
		var filter = this.filters[i];
		var hash = this._filterHash(filter);
		delete this.sections[hash];
	}
	this.filters = [];
}

ConfigFile.prototype._initSectionId =
function(section)
{
	// It's null if caller created it
	if (section.id == null)
		section.id = this.id++;
}

ConfigFile.prototype.setFilters =
function(sections)
{
	this._removeFilters();

	for (var i = 0; i < sections.length; i++)
	{
		var section = sections[i];
		this._initSectionId(section);
		var hash = this._filterHash(section);
		this.sections[hash] = section;
	}
	this.filters = sections;
}

ConfigFile.prototype.getIrcServers =
function()
{
	var servers = {};

	function getServerInfo(serverName, section)
	{
		serverName = canonicalizeServerName(serverName);

		var serverInfo = servers[serverName];
		if (!serverInfo)
		{
			servers[serverName] = serverInfo =
			{
				channels: []
			};
		}

		return serverInfo;
	}

	for (var key in this.sections)
	{
		var section = this.sections[key];
		if (key.match(/^ server /))
		{
			var serverInfo = getServerInfo(section.name);
			serverInfo.serverSection = section;
		}
		else if (key.match(/^ channel /))
		{
			var serverInfo = getServerInfo(section.name);
			serverInfo.channels.push(section);
		}
	}

	// Remove channels with no [server] section
	var rv = [];
	for (var key in servers)
	{
		var serverInfo = servers[key];
		if (serverInfo.serverSection)
			rv.push(serverInfo);
	}

	return rv;
}

ConfigFile.prototype.removeServersChannels =
function()
{
	for (var hash in this.sections)
	{
		if (!hash.match(/^ server /) && !hash.match(/^ channel /))
			continue;
		delete this.sections[hash];
	}
}

ConfigFile.prototype.setServers =
function(serverInfos)
{
	this.removeServersChannels();

	for (var i = 0; i < serverInfos.length; i++)
	{
		var serverInfo = serverInfos[i];

		var serverSection = serverInfo.serverSection;
		var serverName = serverSection.name;
		this._initSectionId(serverSection);
		this.sections[this._serverHash(serverName)] = serverSection;
		
		for (var j = 0; j < serverInfo.channels.length; j++)
		{
			var channelSection = serverInfo.channels[j];
			if (channelSection == null)
				continue;
			channelSection.name = serverName;

			this._initSectionId(channelSection);
			this.sections[this._channelHash(serverName)] = channelSection;
		}
	}
}
