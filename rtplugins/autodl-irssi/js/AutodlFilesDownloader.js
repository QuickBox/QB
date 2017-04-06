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

function autodl_ajax(obj)
{
	// Don't cache any AJAX requests.
	obj.cache = false;

	$.ajax(obj);
}

function getAjaxErrorString(status, ex)
{
	var msg = "AJAX error";

	if (status)
		msg += ", status: " + status;
	if (ex)
		msg += ", " + formatException(ex);

	return msg;
}

/**
 * @param pluginUrl	URL of plugin directory (plugin.path), ending in a slash.
 */
function AutodlFilesDownloader(pluginUrl)
{
	this.pluginUrl = pluginUrl;
	this.handler = null;
	this.isDownloading = false;
	this.files =
	{
		config: "",
		trackers: []
	};
}

AutodlFilesDownloader.prototype.getConfigFile =
function()
{
	return this.files.config;
}

AutodlFilesDownloader.prototype.setConfigFile =
function(config)
{
	this.files.config = config;
}

AutodlFilesDownloader.prototype.getTrackers =
function()
{
	return this.files.trackers;
}

AutodlFilesDownloader.prototype._notifyHandler =
function(errorMessage)
{
	try
	{
		this.filenames = null;
		this.isDownloading = false;
		var handler = this.handler;
		this.handler = null;
		try
		{
			handler(errorMessage);
		}
		catch (ex)
		{
			log("AutodlFilesDownloader._notifyHandler: handler: " + formatException(ex));
		}
	}
	catch (ex)
	{
		log("AutodlFilesDownloader._notifyHandler: " + formatException(ex));
	}
}

// Downloads the autodl.cfg file and all tracker files
AutodlFilesDownloader.prototype.downloadAllFiles =
function(handler)
{
	if (this.isDownloading)
		throw "AutodlFilesDownloader.downloadAllFiles: Already downloading!";

	try
	{
		this.handler = handler;

		var this_ = this;
		autodl_ajax(
		{
			url: this.pluginUrl + "getfiles.php",
			type: "GET",
			dataType: "json",
			success: function(data, status) { this_._onGetfiles(data); },
			error: function(xhr, status, ex)
			{
				this_._notifyHandler("Could not get files listing: " + getAjaxErrorString(status, ex));
			}
		});
	}
	catch (ex)
	{
		this._notifyHandler("AutodlFilesDownloader.downloadAllFiles: " + formatException(ex));
	}
}

AutodlFilesDownloader.prototype._onGetfiles =
function(data)
{
	try
	{
		if (data.error)
			return this._notifyHandler("Error getting files listing: " + data.error);

		this.files.trackers = [];
		this._downloadFiles(data.files);
	}
	catch (ex)
	{
		this._notifyHandler("AutodlFilesDownloader._onGetfiles: " + formatException(ex));
	}
}

// Downloads only the autodl.cfg file
AutodlFilesDownloader.prototype.downloadConfig =
function(handler)
{
	if (this.isDownloading)
		throw "AutodlFilesDownloader.downloadConfig: Already downloading!";

	try
	{
		this.handler = handler;
		this._downloadFiles(['autodl.cfg']);
	}
	catch (ex)
	{
		this._notifyHandler("AutodlFilesDownloader.downloadAllFiles: " + formatException(ex));
	}
}

AutodlFilesDownloader.prototype._downloadFiles =
function(aryFiles)
{
	try
	{
		this.filenames = aryFiles || [];
		this.filenameIndex = 0;
		this._getNextFile();
	}
	catch (ex)
	{
		this._notifyHandler("AutodlFilesDownloader._downloadFiles: " + formatException(ex));
	}
}

AutodlFilesDownloader.prototype._getNextFile =
function()
{
	try
	{
		if (this.filenameIndex >= this.filenames.length)
			return this._notifyHandler("");
		var filename = this.filenames[this.filenameIndex++];

		var dataType;
		if (filename.match(/\.tracker$/))
			dataType = "xml";
		else
			dataType = "text";

		var this_ = this;
		autodl_ajax(
		{
			url: this.pluginUrl + "getfile.php",
			data: [{name: 'file', value: filename}],
			type: "GET",
			dataType: dataType,
			success: function(data, status) { this_._gotFileData(data, filename); },
			error: function(xhr, status, ex)
			{
				this_._notifyHandler("Could not read file " + filename + ": " + getAjaxErrorString(status, ex));
			}
		});
	}
	catch (ex)
	{
		this._notifyHandler("AutodlFilesDownloader._getNextFile: " + formatException(ex));
	}
}

AutodlFilesDownloader.prototype._gotFileData =
function(data, filename)
{
	try
	{
		if (filename === 'autodl.cfg')
			this.files.config = data;
		else if (filename.match(/\.tracker$/))
			this.files.trackers.push(data);
		else
			log("Got unknown file " + filename);

		this._getNextFile();
	}
	catch (ex)
	{
		this._notifyHandler("AutodlFilesDownloader._gotFileData: " + formatException(ex));
	}
}
