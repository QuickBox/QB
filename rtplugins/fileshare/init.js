
plugin.loadMainCSS();


plugin.attachPageToTabs($('<div>').attr("id","FileShare").addClass('table_tab').get(0), 'File Share');


theWebUI.FS = {

	downlink: '',
	clip: {},

	add: function (button) {

		var file = $('#FS_file').val();
		var duration = $('#FS_duration').val();
		var password = $('#FS_password').val();

		if(!duration.match(/^\d+$/) || (duration < 1)) {alert(theUILang.FSvdur); return false;}
		if(this.islimited(this.maxdur, duration)) {alert(theUILang.FSmaxdur+' '+this.maxdur); return false}
		
		$(button).attr('disabled',true);

		this.query('action=add&file='+encodeURIComponent(file)+'&to='+encodeURIComponent(password)+'&target='+encodeURIComponent(duration),
				function() {	theDialogManager.hide('FS_main');
						log(theUILang.FSshow+': '+theUILang.FSlinkcreate);
		});
	},

	edit: function (button) {

		var duration = $('#FS_duration').val();
		var password = $('#FS_password').val();
		var linkid = $('#FS_lid').val();

		if($.trim(duration) != '') {
			if (!duration.match(/^\d+$/) || (duration < 1)) {alert(theUILang.FSvdur); return false;}	
			if(this.islimited(this.maxdur, duration)) {alert(theUILang.FSmaxdur+' '+this.maxdur); return false}
		}	

		$(button).attr('disabled',true);

		this.query('action=edit&file='+encodeURIComponent(linkid)+'&to='+encodeURIComponent(password)+(duration ? '&target='+encodeURIComponent(duration) : ''));
	},

	del: function () {
		
		var sr = theWebUI.getTable("fsh").rowSel;
		var list = {};

		var x = 0;
		for (i in sr) {
			var id = i.split('_fsh_')[1];
			if (sr[i]) {list[x] = id; x++;}
		}

		this.query('action=del&target='+theWebUI.fManager.encode_string(list));

	},

	islimited: function (max, cur) {return (max > 0) ? ((cur <= max) ? false : true) : false;},

	show: function (what, how) { 

		var file;
		var password;

		var edit = $('#FS_editbut');
		var add = $('#FS_addbut');

		switch(how) {

			case 'edit':
				var table = theWebUI.getTable("fsh");
				file = table.getValueById('_fsh_'+what, 'name');
				password = table.getValueById('_fsh_'+what, 'pass');
				$('#FS_downlink').val(table.getValueById('_fsh_'+what, 'link')).show();
				add.hide();
				edit.show();
				break;
			case 'add':
				file = theWebUI.fManager.homedir+theWebUI.fManager.curpath+what;
				password = '';
				$('#FS_downlink').hide();
				edit.hide();
				add.show();
				break;
		}

		$('#FS_lid').val(what);
		add.attr('disabled',false);
		edit.attr('disabled',false);
		$('#FS_duration').val('');
		$('#FS_password').val(password);
		$('#FS_file').val(file);
		theWebUI.fManager.makeVisbile('FS_main');
	},

	refresh: function() {theWebUI.FS.query('action=list');},

	resize: function (w, h) {

		if(w!==null) {w-=8;}

		if(h!==null) {
			h-=($("#tabbar").height());
			h-=2;
        	}

		var table = theWebUI.getTable("fsh");
		if(table) {table.resize(w,h);}
	},


	rename: function() {
		var table = theWebUI.getTable("fsh");
		if(table.created && plugin.allStuffLoaded) {

			table.renameColumnById('time',theUILang.FSexpire);
			table.renameColumnById('name',theUILang.FSfile);
			table.renameColumnById('pass',theUILang.FSpassword);
			table.renameColumnById('link',theUILang.FSdlink);

		} else { setTimeout(arguments.callee,1000);}

	},

	tableadd: function(data) { 

		var table = theWebUI.getTable("fsh");
		table.clearRows();

		$.each(data.list, function(ndx,item) {
   			 
				table.addRowById({
					name: item.file,
					size: item.size,
					time: item.expire,
					pass: item.password,
					link: theWebUI.FS.downlink+'?uh='+encodeURIComponent(data.uh)+'&s='+ndx
				}, "_fsh_"+ndx, 'Icon_File');
		});
		
		table.refreshRows();
	},

	tablecreate: function () {

		theWebUI.tables.fsh = {
			obj: new dxSTable(),
			columns:
			[
				{ text: '',			width: "210px", id: "name",		type: TYPE_STRING },
				{ text: theUILang.Size,			width: "60px",	id: "size",		type: TYPE_NUMBER },
				{ text: '', 			width: "120px", 	id: "time",		type: TYPE_STRING, 	"align" : ALIGN_CENTER},
				{ text: '',			width: "80px", 	id: "pass",		type: TYPE_STRING },
				{ text: '',			width: "310px",	id: "link",		type: TYPE_STRING }
			],
			container:	"FileShare",
			format:		theWebUI.FS.tableformat,
			onselect:	function(e,id) { theWebUI.FS.tablemenu(e,id); }
		};


	},


	tableformat: function(table,arr) {
		for(var i in arr)
		{
			if(arr[i]==null)
				arr[i] = '';
			else
   			switch(table.getIdByCol(i)) {
      				case 'size' : 
      					arr[i] = theConverter.bytes(arr[i], 2);
      					break;
				case 'time':
					arr[i] = theConverter.date(iv(arr[i])+theWebUI.deltaTime/1000);
					break;
	      		}
	   	}
		return(arr);
	},

	tablemenu: function(e, id) {

		if($type(id) && (e.button == 2)) {

		       theContextMenu.clear();

			var table = theWebUI.getTable("fsh");
			var target = id.split('_fsh_')[1];
			if(table.selCount == 1) {
				var link = theWebUI.getTable("fsh").getValueById('_fsh_'+target, 'link');
				theWebUI.FS.clip.setText(link);
			}

			var target = id.split('_fsh_')[1];
			theContextMenu.add([theUILang.fDelete, function() {askYesNo(theUILang.FSdel, theUILang.FSdelmsg, "theWebUI.FS.del()" );}]);
			theContextMenu.add([theUILang.FSedit, (table.selCount > 1) ? null : function() {theWebUI.FS.show(target, 'edit');}]);
			theContextMenu.add([CMENU_SEP]);
			theContextMenu.add([theUILang.FScopylink,(table.selCount > 1) ? null : function() {}]);

	   		theContextMenu.show();
			
			if(table.selCount == 1) {
				var lie = theContextMenu.get(theUILang.FScopylink)[0];

				if(!theWebUI.FS.clip.ready) {
					theWebUI.FS.clip.glue(lie); 
					theWebUI.FS.clip.addEventListener( 'onComplete', function() {theWebUI.FS.clip.hide();} );
				}
				theWebUI.FS.clip.reposition(lie);
       			theWebUI.FS.clip.show(lie);
			}
			

			return(true);
		}
		return(false);
	},


	query: function(action, complete, err) {


			$.ajax({
  				type: 'POST',
   				url: 'plugins/fileshare/fsh.php',
				timeout: theWebUI.settings["webui.reqtimeout"],
			       async : true,
			       cache: false,
				data: action,
   				dataType: "json",

				error: function(XMLHttpRequest, textStatus, errorThrown) {
					log('FILE SHARE: error - STATUS:'+textStatus+' MSG: '+XMLHttpRequest.responseText);			
				},

				success: function(data, textStatus) {if($type(complete)) {complete();}
										theWebUI.FS.tableadd(data);}
 		});


	}

}




plugin.config = theWebUI.config;
theWebUI.config = function(data) {

		theWebUI.FS.tablecreate();
		theWebUI.FS.rename();
		plugin.config.call(this,data);
}


plugin.resizeBottom = theWebUI.resizeBottom;
theWebUI.resizeBottom = function( w, h ) {

		theWebUI.FS.resize(w, h);
		plugin.resizeBottom.call(this,w,h);
}


plugin.onShow = theTabs.onShow;
theTabs.onShow = function(id) {

	if(id == "FileShare") {
			$('#FS_refresh').show();
			theWebUI.getTable('fsh').refreshRows();
			theWebUI.resize();
	} else {$('#FS_refresh').hide(); plugin.onShow.call(this,id);}
}


plugin.flmMenu = theWebUI.fManager.flmSelect;
theWebUI.fManager.flmSelect = function( e, id ) {

		plugin.flmMenu.call(this, e, id);
		if(plugin.enabled) {
			var el = theContextMenu.get( theUILang.fMediaI );
			if(el) {
				var item = id.split('_flm_')[1];
				theContextMenu.add( el, [theUILang.FSshare, (!theWebUI.fManager.isDir(item) && !(theWebUI.getTable("flm").selCount > 1) && !theWebUI.FS.islimited(theWebUI.FS.maxlinks, theWebUI.getTable("fsh").rows)) ? function() {theWebUI.FS.show(item, 'add');} : null]);
			}
		}
		
}



plugin.onLangLoaded = function() {

	injectScript('plugins/fileshare/settings.js.php', function() {theWebUI.FS.refresh();});
	injectScript('plugins/fileshare/clip/clip.js', function() {
								ZeroClipboard.setMoviePath('plugins/fileshare/clip/ZeroClipboard.swf');
								theWebUI.FS.clip = new ZeroClipboard.Client();
							});

	if(this.enabled) {
		plugin.renameTab('FileShare', theUILang.FSshow);
		$('#tab_lcont').append('<input type="button" id="FS_refresh" class="Button" value="'+theUILang.fRefresh+'" style="display: none;">');


		var add = '<div class="cont fxcaret"><fieldset><legend>Options:</legend>'+
				'<table border="0" cellspacing="0" cellpadding="0" >'+
  				'<tr><td>'+theUILang.FSfile+':</td>'+
    					'<td align="right"><input type="text" name="FS_file" class="TextboxLarge" readonly="readonly" id="FS_file" value=""/></td>'+
  				'</tr><tr><td>'+theUILang.FSdhours+':</td>'+
   				' <td align="right"><label><input type="text" name="FS_duration" id="FS_duration" class="Textbox" /></label></td>'+
 				' </tr><tr><td>'+theUILang.FSpassword+':</td><td align="right"><label><input name="FS_password" type="text" id="FS_password" class="Textbox" />'+
  				'</label><input name="FS_lid" type="hidden" id="FS_lid" value="" /></td></tr></table>'+
				'<textarea name="FS_downlink" rows="2" style="width: 100%" readonly="readonly" id="FS_downlink"></textarea></fieldset></div>'+
				'<div class="aright buttons-list">'+
					'<input type="button" id="FS_editbut" class="Button" value="'+theUILang.FSedit+'" class="Button" />'+
					'<input type="button" id="FS_addbut" class="Button" value="'+theUILang.FSadd+'" class="Button" />'+
					'<input type="button" class="Cancel Button" value="'+theUILang.fDiagClose+'"/>'+
				'</div>';
				

		theDialogManager.make('FS_main', theUILang.FSshow, add, false);

		$('#FS_addbut').click(function() {theWebUI.FS.add(this);});
		$('#FS_editbut').click(function() {theWebUI.FS.edit(this);});
		$('#FS_refresh').click(function() {theWebUI.FS.refresh();});



	}
};


plugin.onRemove = function() {
	this.removePageFromTabs('FileShare');
	$('[id^="FS_"]').remove();
}

plugin.loadLang(true);