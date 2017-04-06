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

// Base class
function TabsBase()
{
	this.tabs = [];
	this.selected = undefined;
	this.onChange = function(oldObj, newObj) {};
}

/*
 * @param idTabElem		The id of the tab element
 * @param idContentElem	The id of the content element
 */
TabsBase.prototype.add =
function(idTabElem, idContentElem)
{
	var obj =
	{
		tabElem:		document.getElementById(idTabElem),
		contentElem:	document.getElementById(idContentElem)
	};
	if (!obj.tabElem)
		throw "No tab elem with id " + idTabElem;
	if (!obj.contentElem)
		throw "No content elem with id " + idContentElem;
	this.tabs.push(obj);

	this._makeUnselected(obj);
	if (this.tabs.length === 1)
		this._setNewSelected(obj);

	return obj;
}

TabsBase.prototype.selectByIndex =
function(index)
{
	var obj = this.tabs[index];
	if (!obj)
		return;
	this._setNewSelected(obj);
}

// Set a new selected tab
TabsBase.prototype._setNewSelected =
function(newSelected)
{
	if (newSelected === this.selected)
		return;
	var oldSelected = this.selected;
	this._makeUnselected(oldSelected);
	this._makeSelected(newSelected);
	this.selected = newSelected;
	this.onChange(oldSelected, newSelected);
}

TabsBase.prototype._makeUnselected =
function(obj)
{
	if (!obj)
		return;
	$(obj.contentElem).css("display", "none");
}

TabsBase.prototype._makeSelected =
function(obj)
{
	if (!obj)
		return;
	$(obj.contentElem).css("display", "block");
}

// Controls a tab header
function Tabs()
{
	TabsBase.call(this);
}
Tabs.prototype = new TabsBase();
Tabs.constructor = Tabs;

Tabs.prototype.add =
function(idTabElem, idContentElem)
{
	var obj = TabsBase.prototype.add.call(this, idTabElem, idContentElem);

	var this_ = this;
	$(obj.tabElem).mousedown(function(e)
	{
		this_._setNewSelected(obj);
	});
}

Tabs.prototype._makeUnselected =
function(obj)
{
	if (!obj)
		return;
	TabsBase.prototype._makeUnselected.call(this, obj);
	$(obj.tabElem).parent().removeClass("selected");
}

Tabs.prototype._makeSelected =
function(obj)
{
	if (!obj)
		return;
	TabsBase.prototype._makeSelected.call(this, obj);
	$(obj.tabElem).parent().addClass("selected");
}

// Similar to a tab header except it's using a drop down box
function DropDownTabs(idSelect)
{
	TabsBase.call(this);

	this.selectElem = document.getElementById(idSelect);

	var this_ = this;
	$(this.selectElem).change(function(e) { this_._onChange(); })
						// selectedIndex should be updated when we get a keyup event
					  .keyup(function(e) { this_._onChange(); });
}
DropDownTabs.prototype = new TabsBase();
DropDownTabs.constructor = DropDownTabs;

DropDownTabs.prototype._onChange =
function()
{
	this._setNewSelected(this.tabs[this.selectElem.selectedIndex]);
}

DropDownTabs.prototype.add =
function(idTabElem, idContentElem, value)
{
	var obj = TabsBase.prototype.add.call(this, idTabElem, idContentElem);
	obj.value = value;
}

DropDownTabs.prototype.select =
function(value)
{
	for (var i = 0; i < this.tabs.length; i++)
	{
		if (this.tabs[i].value === value)
		{
			this.selectElem.selectedIndex = i;
			this._setNewSelected(this.tabs[i]);
			return;
		}
	}

	log("DropDownTabs: Could not find value " + value);
}

DropDownTabs.prototype.getSelectedValue =
function()
{
	if (!this.selected)
		return null;
	return this.selected.value;
}
