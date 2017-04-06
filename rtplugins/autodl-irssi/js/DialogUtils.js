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

// Also works with numbers...
function stringCompare(a, b)
{
	if (a < b)
		return -1;
	if (a > b)
		return 1;
	return 0;
}

function checkFilterStrings(name, filter)
{
	return checkRegexArray(name, regexEscapeWildcardString(filter).split(","));
}

function regexEscapeWildcardString(s)
{
	s = s.replace(/([\^\$\.\+\=\!\:\|\\\/\(\)\[\]\{\}])/g, "\\$1");
	s = s.replace(/([\*\?])/g, ".$1");
	return s;
}

function checkRegexArray(name, filterWordsAry)
{
	for (var i = 0; i < filterWordsAry.length; i++)
	{
		var filterWord = $.trim(filterWordsAry[i]);
		if (!filterWord)
			continue;
		if (name.match(new RegExp("^" + filterWord + "$", "i")))
			return true;
	}

	return false;
}

function convertStringToBoolean(s)
{
	s = s.toLowerCase();
	return !(s === "false" || s === "off" || s === "no" || s === "0");
}

function convertStringToNumber(valueStr, defaultValue, minValue, maxValue)
{
	var value = parseFloat(valueStr);
	if (isNaN(value))
		value = defaultValue;
	if (value === undefined)
		return value;
	if (minValue !== undefined && value < minValue)
		value = minValue;
	if (maxValue !== undefined && value > maxValue)
		value = maxValue;
	return value;
}

function convertStringToInteger(valueStr, defaultValue, minValue, maxValue)
{
	return Math.floor(convertStringToNumber(valueStr, defaultValue, minValue, maxValue));
}

function DialogOptionBool(id, name, defaultValue)
{
	this.type = "bool";
	this.id = id;
	this.name = name;
	this.defaultValue = defaultValue;
}

function DialogOptionInt(id, name, defaultValue)
{
	this.type = "int";
	this.id = id;
	this.name = name;
	this.defaultValue = defaultValue;
}

function DialogOptionText(id, name, defaultValue)
{
	this.type = "text";
	this.id = id;
	this.name = name;
	this.defaultValue = defaultValue;
}

function getSectionOptionValue(section, name, defaultValue, type)
{
	if (!section)
		return (new ConfigOption(0, name, defaultValue, defaultValue, type)).getValue();
	return section.getOption(name, defaultValue, type).getValue();
}

function enableJqueryElem(jq, enable)
{
	if (enable)
		jq.prop("disabled", false);
	else
		jq.prop("disabled", true);
}

function initDialogOptions(section, aryDlgOptions)
{
	for (var i = 0; i < aryDlgOptions.length; i++)
	{
		var dlgOption = aryDlgOptions[i];
		var val = getSectionOptionValue(section, dlgOption.name, dlgOption.defaultValue, dlgOption.type);
		if (dlgOption.type === "bool")
		{
			if (val)
				$("#" + dlgOption.id).prop("checked", true);
			else
				$("#" + dlgOption.id).prop("checked", false);
		}
		else
			$("#" + dlgOption.id).val(val);
	}
}

function saveDialogOptions(section, aryDlgOptions)
{
	if (!section)
		return;
	for (var i = 0; i < aryDlgOptions.length; i++)
	{
		var dlgOption = aryDlgOptions[i];
		var option = section.getOption(dlgOption.name, dlgOption.defaultValue, dlgOption.type);

		var val;
		if (dlgOption.type === "bool")
			val = $("#" + dlgOption.id)[0].checked;
		else
			val = $("#" + dlgOption.id).val();

		if (val == null)
			val = "";
		option.setValue(val.toString());
	}
}

function DropDownBox(id)
{
	this.selectElem = document.getElementById(id);
	this.options = [];
	this.selectedIndex = -1;

	var this_ = this;
	$(this.selectElem).change(function(e) { this_._onChange(); })
						// selectedIndex should be updated when we get a keyup event
					  .keyup(function(e) { this_._onChange(); });
}

DropDownBox.prototype._onChange =
function()
{
	this._setNewSelected(this.selectElem.selectedIndex);
}

DropDownBox.prototype._getIndex =
function(value)
{
	for (var i = 0; i < this.options.length; i++)
	{
		if (this.options[i] === value)
			return i;
	}

	return -1;
}

DropDownBox.prototype.renameSelected =
function(newText)
{
	if (this.selectedIndex < 0)
		return;

	$(this.selectElem.options[this.selectedIndex]).text(newText);
}

DropDownBox.prototype.removeSelected =
function()
{
	if (this.selectedIndex < 0)
		return;

	var newIndex = this.selectedIndex + 1;
	var oldIndex = this.selectedIndex;
	if (this.options[newIndex] == null)
		newIndex = this.selectedIndex - 1;
	if (this.options[newIndex] == null)
		newIndex = -1;

	this._setNewSelected(newIndex);
	var newValue = this.getValue(newIndex);

	this.selectElem.remove(oldIndex);
	this.options.splice(oldIndex, 1);
	this.selectedIndex = this.selectElem.selectedIndex = this._getIndex(newValue);
}

DropDownBox.prototype.getValue =
function(index)
{
	if (index < 0 || index >= this.options.length)
		return null;
	return this.options[index];
}

DropDownBox.prototype._setNewSelected =
function(newIndex)
{
	if (newIndex === this.selectedIndex)
		return;
	var oldSelected = this.selectedIndex;

	// Notify owner
	if (this.onChange)
		this.onChange(this.getValue(oldSelected), this.getValue(newIndex));

	this.selectedIndex = newIndex;
}

DropDownBox.prototype.empty =
function()
{
	$(this.selectElem).empty();
	this.options = [];
	this.selectedIndex = -1;
}

DropDownBox.prototype.add =
function(value, text)
{
	$(this.selectElem).append($('<option />').text(text));
	this.options.push(value);
}

DropDownBox.prototype.select =
function(value)
{
	var index = this._getIndex(value);
	if (index < 0)
		return;

	this._setNewSelected(index);
	this.selectElem.selectedIndex = index;
}

DropDownBox.prototype.getSelectedValue =
function()
{
	return this.getValue(this.selectElem.selectedIndex);
}

// Makes sure a bunch of textboxes contain the same value
function SyncTextBoxes(ids)
{
	var this_ = this;

	this.textboxElems = [];
	for (var i = 0; i < ids.length; i++)
	{
		(function(i, textbox)
		{
			this_.textboxElems.push(textbox);
			$(textbox).keyup(function(e) { this_.setNewValue(i); })
		})(i, document.getElementById(ids[i]));
	}
}

// Updates each textbox with the new value
SyncTextBoxes.prototype.setNewValue =
function(index)
{
	var newValue = $(this.textboxElems[index]).val();
	for (var i = 0; i < this.textboxElems.length; i++)
	{
		if (i === index)
			continue;
		$(this.textboxElems[i]).val(newValue);
	}
}
