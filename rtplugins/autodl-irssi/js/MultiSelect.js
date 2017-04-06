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

function MultiSelect()
{
	theDialogManager.make("autodl-multiselect", theUILang.autodlSelect,
		'<div id="autodl-multiselect">' +
			'<div id="autodl-multiselect-title" />' +
			'<div id="autodl-multiselect-content">' +
				'<select multiple="multiple" id="autodl-multiselect-select" />' +
			'</div>' +
			'<div class="aright buttons-list dialog-buttons">' +
				'<input type="button" id="autodl-multiselect-ok-button" value="' + theUILang.ok + '" class="OK Button" />' +
				'<input type="button" value="' + theUILang.Cancel + '" class="Cancel Button" />' +
			'</div>' +
		'</div>', true
	);

	var this_ = this;

	this.selectElem = document.getElementById("autodl-multiselect-select");

	theDialogManager.setHandler("autodl-multiselect", "beforeShow", function()
	{
		this_.onBeforeShow();
	});
	theDialogManager.setHandler("autodl-multiselect", "afterHide", function()
	{
		this_.onAfterHide();
	});

	$("#autodl-multiselect-ok-button").click(function(e)
	{
		this_._onClickOk();
	});
}

MultiSelect.prototype.show =
function(data)
{
	this.data = data;

	theDialogManager.show("autodl-multiselect");
}

MultiSelect.prototype.onBeforeShow =
function()
{
	$("#autodl-multiselect-header").text(this.data.title);
	$("#autodl-multiselect-title").text(this.data.text);
	this._initializeListbox();
}

MultiSelect.prototype.onAfterHide =
function()
{
	this.data = null;
}

MultiSelect.prototype._initializeListbox =
function()
{
	var this_ = this;
	$(this.selectElem).empty();

	this.data.listboxData.sort(function(a, b)
	{
		return stringCompare(a.displayName.toLowerCase(), b.displayName.toLowerCase());
	});

	function isSelected(validNames)
	{
		for (var i = 0; i < validNames.length; i++)
		{
			if (validNames[i].length === 0)
				continue;
			if (checkFilterStrings(validNames[i], this_.data.selectedText))
				return true;
		}
		return false;
	}

	for (var i = 0; i < this.data.listboxData.length; i++)
	{
		var option = $('<option />').text(this.data.listboxData[i].displayName);
		if (isSelected(this.data.listboxData[i].validNames))
			option.attr("selected", "selected");
		$(this.selectElem).append(option);
	}
}

MultiSelect.prototype.getSelectedString =
function()
{
	var rv = "";

	var this_ = this;
	$("option", this.selectElem).each(function(index, elem)
	{
		if (!this.selected)
			return;
		var validNames = this_.data.listboxData[index].validNames;
		for (var i = 0; i < validNames.length; i++)
		{
			if (validNames[i].length === 0)
				continue;
			if (rv)
				rv += ", ";
			rv += validNames[i];
			break;
		}
	});

	return rv;
}

MultiSelect.prototype._onClickOk =
function()
{
	var data = this.data;	// Save it because onAfterHide() removes it
	data.selectedText = this.getSelectedString();
	theDialogManager.hide("autodl-multiselect");

	data.okClicked();
}
