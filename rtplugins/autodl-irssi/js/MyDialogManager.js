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

function MyDialogManager(pluginPath)
{
	this.isDownloading = false;
	this.pluginPath = pluginPath;
	this.trackersId = 0;			// Incremented each time we've read the tracker files
	this.lastReadConfigFile = null;	// Date() when we last read autodl.cfg from server
	this.redownloadAll = true;		// true if we should download all files: *.tracker and autodl.cfg
	this.savingConfigFile = false;	// true when we're saving to autodl.cfg

	this.filesDownloader = new AutodlFilesDownloader(this.pluginPath);
	this.trackerInfos = [];
	this.multiSelectDlgBox = new MultiSelect();
	this.configFile = new ConfigFile();
	this.preferences = new Preferences();
	this.trackers = new Trackers();
	this.filters = new Filters();
	this.servers = new Servers();
	this.ircsrvs = new IrcServers();
	this.help = new Help();

	var this_ = this;
	for (var i = 0; i < this.names.length; i++)
	{
		(function(name)
		{
			this_[name].createDialogBox(this_.multiSelectDlgBox, function()
			{
				this_._onOkClicked(name);
			});

			var id = 'autodl-' + name;
			theDialogManager.setHandler(id, "beforeShow", function()
			{
				this_[name].onBeforeShow(this_.configFile, this_.trackerInfos, this_.trackersId);
			});
			theDialogManager.setHandler(id, "afterHide", function()
			{
				this_[name].onAfterHide();
			});
		})(this.names[i]);
	}

	// Download new tracker files once every hour
	setInterval(function() { this_.redownloadAll = true }, 60*60*1000);

	this_._downloadFiles();
}

// Number of seconds to cache the config file.
MyDialogManager.prototype.CONFIG_FILE_CACHE_SECS = 30;

MyDialogManager.prototype.names =
[
	'preferences',
	'filters',
	'trackers',
	'servers',
	'ircsrvs',
	'help'
];

MyDialogManager.prototype._onOkClicked =
function(name)
{
	var updatedConfigFile = this[name].onOkClicked();

	try
	{
		theDialogManager.hide("autodl-" + name);

		if (updatedConfigFile)
			this._uploadConfigFile(this.configFile.toString());
	}
	catch (ex)
	{
		log("MyDialogManager._onOkClicked: " + formatException(ex));
	}
}

MyDialogManager.prototype._uploadConfigFile =
function(fileData)
{
	try
	{
		var this_ = this;

		var boundaryString = "---------------------------32853208516921";
		var boundary = "--" + boundaryString;
		var postData = boundary + "\r\n" +
						"Content-Disposition: form-data; name=\"file\"; filename=\"autodl.cfg\"\r\n" +
						"Content-Type: text/plain\r\n" +
						"\r\n" +
						fileData + "\r\n" +
						boundary + "--" + "\r\n";

		this.savingConfigFile = true;
		$.ajax(
		{
			url: this.pluginPath + "writeconfig.php",
			data: postData,
			type: "POST",
			dataType: "json",
			// We MUST add charset=UTF-8 here ourselves or it will fail to upload (possibly a PHP or Chrome
			// bug). If we don't add it before 'boundary', then Chrome will add it at the end of the string,
			// i.e., after 'boundary' causing PHP to report an error and the upload fail.
			contentType: "multipart/form-data; charset=UTF-8; boundary=" + boundaryString,
			processData: false,
			success: function(data, status)
			{
				this_._onSavedConfigFile(data.error, fileData);
			},
			error: function(xhr, status, ex)
			{
				this_._onSavedConfigFile("Unknown error: " + getAjaxErrorString(status, ex));
			}
		});
	}
	catch (ex)
	{
		this.savingConfigFile = false;
		log("Could not save to autodl.cfg. " + formatException(ex));
	}
}

MyDialogManager.prototype._onSavedConfigFile =
function(errorMessage, configFileData)
{
	this.savingConfigFile = false;
	if (errorMessage)
	{
		this.redownloadAll = true;
		log(theUILang.autodlErrorSaving + errorMessage);
		return;
	}

	// We just wrote the file, so we have a copy of it
	this.lastReadConfigFile = new Date();
	this.filesDownloader.setConfigFile(configFileData);
}

MyDialogManager.prototype._isDialogVisible =
function(name)
{
	return $.inArray('autodl-' + name, theDialogManager.visible) >= 0;
}

MyDialogManager.prototype._isOneOfOurDialogsVisible =
function()
{
	for (var i = 0; i < this.names.length; i++)
	{
		if (this._isDialogVisible(this.names[i]))
			return true;
	}
	return false;
}

MyDialogManager.prototype.toggleDialog =
function(name)
{
	if (this._isDialogVisible(name))
	{
		theDialogManager.hide('autodl-' + name);
		return;
	}

	this.dialogName = name;
	this._downloadFiles();
}

MyDialogManager.prototype._downloadFiles =
function()
{
	if (this.savingConfigFile)
	{
		this.dialogName = null;
		alert(theUILang.autodlWaitSaving);
	}
	else if (this.redownloadAll)
		this._downloadAllFiles();
	else
		this._downloadConfigFile();
}

// Downloads the autodl.cfg file and all *.tracker files
MyDialogManager.prototype._downloadAllFiles =
function()
{
	if (this.isDownloading)
		return;
	this.isDownloading = true;
	var this_ = this;
	this.filesDownloader.downloadAllFiles(function(errorMessage)
	{
		this_._onDownloadedFiles(errorMessage, true);
	});
}

MyDialogManager.prototype.clearConfigFileCache =
function()
{
	this.lastReadConfigFile = null;
}

// Downloads the autodl.cfg file
MyDialogManager.prototype._downloadConfigFile =
function()
{
	if (this.isDownloading)
		return;

	if (this.lastReadConfigFile != null &&
		((new Date()) - this.lastReadConfigFile) <= this.CONFIG_FILE_CACHE_SECS*1000)
	{
		this._useCachedConfigFile();
		return;
	}

	this.isDownloading = true;
	var this_ = this;
	this.filesDownloader.downloadConfig(function(errorMessage)
	{
		this_._onDownloadedFiles(errorMessage, false);
	});
}

MyDialogManager.prototype._onDownloadedFiles =
function(errorMessage, downloadedAllFiles)
{
	try
	{
		this.isDownloading = false;
		var dialogName = this.dialogName;
		this.dialogName = null;

		if (errorMessage)
		{
			this.redownloadAll = true;
			log(theUILang.autodlError + errorMessage);
			return;
		}

		this.configFile.parse(this.filesDownloader.getConfigFile());
		this.lastReadConfigFile = new Date();

		if (downloadedAllFiles)
			this._parseXmlDocuments(this.filesDownloader.getTrackers());

		this.redownloadAll = false;
		this._showDialog(dialogName);
	}
	catch (ex)
	{
		this.redownloadAll = true;
		log("MyDialogManager._onDownloadedFiles: " + formatException(ex));
	}
}

MyDialogManager.prototype._useCachedConfigFile =
function()
{
	try
	{
		var dialogName = this.dialogName;
		this.dialogName = null;
		this.configFile.parse(this.filesDownloader.getConfigFile());
		this._showDialog(dialogName);
	}
	catch (ex)
	{
		log("MyDialogManager._useCachedConfigFile: " + formatException(ex));
	}
}

MyDialogManager.prototype._showDialog =
function(dialogName)
{
	if (dialogName)
		theDialogManager.show('autodl-' + dialogName);
}

MyDialogManager.prototype._parseXmlDocuments =
function(aryDocs)
{
	var trackerInfos = [];

	for (var i = 0; i < aryDocs.length; i++)
	{
		var xmlDoc = aryDocs[i];
		trackerInfos.push(new TrackerInfo(xmlDoc));
	}

	this.trackerInfos = trackerInfos;
	this.trackersId++;
}
