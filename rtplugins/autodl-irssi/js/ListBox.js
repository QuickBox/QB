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

/*
 * @param idParent	Parent of all listitems, i.e., the id of this listbox
 *
 * When a listitem is selected, ListBox.onSelected(oldData, newData) is called if it's been
 * initialized to a user function.
 */
function ListBox(idParent)
{
	this.lbElem = document.getElementById(idParent);
	$(this.lbElem).addClass("autodl-listbox");
	this.rows = [];
	this.selectedIndex = -1;
}

ListBox.prototype.removeAll =
function()
{
	this.rows = [];
	this.selectedIndex = -1;
	$(this.lbElem).empty();
}

ListBox.prototype.getSelectedIndex =
function()
{
	return this.selectedIndex;
}

ListBox.prototype.append =
function(elem, data)
{
	var obj =
	{
		elem: $('<div class="autodl-listbox-item" />').append(elem)[0],
		data: data
	};
	this.rows.push(obj);
	$(this.lbElem).append(obj.elem);

	var this_ = this;
	$(obj.elem).click(function(e)
	{
		this_.select(this_._findIndex(obj));
	});
}

// Returns the index or -1 if not found
ListBox.prototype._findIndex =
function(obj)
{
	for (var i = 0; i < this.rows.length; i++)
	{
		if (this.rows[i] === obj)
			return i;
	}

	return -1;
}

// Returns the index or -1 if not found
ListBox.prototype._findIndexData =
function(data)
{
	for (var i = 0; i < this.rows.length; i++)
	{
		if (this.rows[i].data === data)
			return i;
	}

	return -1;
}

ListBox.prototype.select =
function(index)
{
	this._newSelected(index);
}

ListBox.prototype.selectData =
function(data)
{
	this._newSelected(this._findIndexData(data));
}

ListBox.prototype._getData =
function(index)
{
	var obj = this.rows[index];
	if (obj)
		return obj.data;
	return;
}

ListBox.prototype._newSelected =
function(index)
{
	var oldObj = this.rows[this.selectedIndex];
	var newObj = this.rows[index];
	this.selectedIndex = this._fixIndex(index);
	this._newSelectedObj(oldObj, newObj);
}

ListBox.prototype._newSelectedObj =
function(oldObj, newObj)
{
	if (oldObj === newObj)
		return;

	this._makeUnselected(oldObj);
	this._makeSelected(newObj);
	if (this.onSelected)
		this.onSelected((oldObj || {}).data, (newObj || {}).data);
}

ListBox.prototype._makeUnselected =
function(obj)
{
	if (!obj)
		return;
	$(obj.elem).removeClass("autodl-listbox-item-selected");
}

ListBox.prototype._makeSelected =
function(obj)
{
	if (!obj)
		return;
	$(obj.elem).addClass("autodl-listbox-item-selected");
}

ListBox.prototype.getSelectedData =
function()
{
	return (this.rows[this.selectedIndex] || {}).data;
}

ListBox.prototype.removeSelected =
function()
{
	this.remove(this.selectedIndex);
}

ListBox.prototype.removeData =
function(data)
{
	this.remove(this._findIndexData(data));
}

ListBox.prototype._fixIndex =
function(index)
{
	if (!this.rows[index])
		return -1;
	return index;
}

ListBox.prototype._removeIndex =
function(index)
{
	var childElem = $(this.lbElem).children("div")[index];
	$(childElem).remove();
	this.rows.splice(index, 1);
}

ListBox.prototype.remove =
function(index)
{
	if (!this.rows[index])
		return;

	if (this.selectedIndex !== index)
	{
		this._removeIndex(index);
		if (this.selectedIndex > index);
			this.selectedIndex--;
		return;
	}

	var oldObj = this.rows[this.selectedIndex];
	var newObj = this.rows[this.selectedIndex + 1];
	if (!newObj)
		newObj = this.rows[this.selectedIndex - 1];
	this._removeIndex(index);
	this.selectedIndex = this._findIndex(newObj);
	this._newSelectedObj(oldObj, newObj);
}
