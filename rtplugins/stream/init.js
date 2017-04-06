plugin.loadLang();

plugin.config = theWebUI.config;
theWebUI.config = function(data)
{
	plugin.config.call(this,data);
	var oldDblClick = this.getTable("fls").ondblclick;
	this.getTable("fls").ondblclick = function(obj) 
	{
		if(plugin.enabled && (theWebUI.dID!="") && (theWebUI.dID.length==40))
		{
		        if(theWebUI.settings["webui.fls.view"])
		        {
				var arr = obj.id.split('_f_');
		        	theWebUI.streamData(theWebUI.dID,arr[1]);
		        	return(false);
			}
		        else
		        {
				var lnk = this.getAttr(obj.id, "link");
	                	if(lnk==null)
	                	{
        	        		theWebUI.streamData(theWebUI.dID,obj.id.substr(3));
					return(false);
	        		}
			}
		}
		return(oldDblClick.call(this,obj));
	}
}

theWebUI.streamData = function( hash, no )
{
	$("#s_datahash").val(hash);
	$("#s_datano").val(no);
	$("#s_getdata").submit();
}

if(plugin.canChangeMenu())
{
	plugin.createFileMenu = theWebUI.createFileMenu;
	theWebUI.createFileMenu = function( e, id )
	{
		if(plugin.createFileMenu.call(this, e, id))
		{
		        if(plugin.enabled)
		        {
				theContextMenu.add([CMENU_SEP]);
				var fno = null;
				var table = this.getTable("fls");
				if(table.selCount == 1)
				{
		        		var fid = table.getFirstSelected();
					if(this.settings["webui.fls.view"])
					{
						var arr = fid.split('_f_');
						fno = arr[1];
					}
					else
						if(!this.dirs[this.dID].isDirectory(fid))
							fno = fid.substr(3);
					if( 
//						((fno!=null) && (this.files[this.dID][fno].size>=2147483647) && !theWebUI.systemInfo.php.canHandleBigFiles) || 
						(theWebUI.dID.length>40))
						fno = null;
				}
				theContextMenu.add( [theUILang.streamData,  (fno==null) ? null : "theWebUI.streamData('" + theWebUI.dID + "',"+fno+")"] );
			}
			return(true);
		}
		return(false);
	}
}

plugin.onLangLoaded = function()
{
	$(document.body).append($("<iframe name='s_datafrm'/>").css({visibility: "hidden"}).attr( { name: "s_datafrm", id: "s_datafrm" } ).width(0).height(0).load(function()
	{
	        $("#s_datahash").val('');
	        $("#s_datano").val('');
		var d = (this.contentDocument || this.contentWindow.document);
		if(d && (d.location.href != "about:blank"))
			try { eval(d.body.innerHTML); } catch(e) {}
	}));
	$(document.body).append(
		$('<form action="plugins/stream/action.php" id="s_getdata" method="get" target="s_datafrm">'+
			'<input type="hidden" name="hash" id="s_datahash" value="">'+
			'<input type="hidden" name="no" id="s_datano" value="">'+
		'</form>').width(0).height(0));
}

plugin.onRemove = function()
{
	$("#s_datafrm").remove();
	$("#s_getdata").remove();
}
