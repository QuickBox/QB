

plugin.attachPageToTabs($('<div>').attr("id","FileManager").addClass('table_tab').html('<table width="100%" border="0" id="fMan_navpath" cellspacing="0" cellpadding="0"><tr>'+
 '   <td width="19"><input type="button" name="fMan_navbut" id="fMan_navbut" value="" /></td>'+
 '   <td><select name="fMan_pathsel" id="fMan_pathsel" style="width:100%;"></select></td>'+
' </tr></table><div id="fManager_data" class="table_tab"></div>').get(0), 'File Manager');



plugin.loadMainCSS();


theWebUI.fManager = {


	paths: [],
	curpath: '/',
	workpath: '/',
	settings: {	timef: '%d-%M-%y %h:%m:%s',
			permf: 1,
			histpath: 5,
			stripdirs: true,
			showhidden: true,
			cleanlog: false,
			arcnscheme: 'new',
			scrows: 12,
			sccols: 4,
			scwidth: 300
	},
	pathlists: 5,
	permf: 0,
	tformat: '%d-%M-%y %h:%m:%s',
	inaction: false,
	actionlist:{},
	actionstats: 0,
	actiontoken:0,
	actiontimeout: 0,
	actionlp: 0,
	activediag: '',
	homedir: '',

	action: {
			
			requrl: 'plugins/filemanager/flm.php',

			request: function (data2send, onComplete, onTimeout, onError) {


							$.ajax({
  								type: 'POST',
   								url: theWebUI.fManager.action.requrl,
								timeout: theWebUI.settings["webui.reqtimeout"],
			        				async : true,
			        				cache: false,
								data: 'dir='+encodeURIComponent(theWebUI.fManager.workpath)+'&'+data2send,
   								dataType: "json",

								error: function(XMLHttpRequest, textStatus, errorThrown) {

										theWebUI.fManager.inaction = false;
										var diag = theWebUI.fManager.activediag.split('fMan_');

										if(textStatus=="timeout") {

											if($type(onTimeout) == "function") {
												onTimeout();
											} else {
												log(theUILang.fManager+': '+diag[1]+' - '+theUILang.fErrMsg[11]);
											}

										} else 
										if(($type(onError) == "function")) {
			        							var status = "Status unavailable";
			        							var response = "Responce unavailable";
											try { 
												status = XMLHttpRequest.status; 
												response = XMLHttpRequest.responseText; 
											} catch(e) {};

											onError(status+" "+textStatus+" "+errorThrown,response);
										} else { 
											log(theUILang.fManager+' ERROR: '+diag[1]+' - '+XMLHttpRequest.status+" "+XMLHttpRequest.responseText+" "+errorThrown); 
										}
										theWebUI.fManager.cleanactions();
								},

								success: function(data, textStatus) {
									
	            								if($type(onComplete) == 'function') {onComplete(data, status); } else
										if(!theWebUI.fManager.isErr(data.errcode, theWebUI.fManager.activediag) && $type(data.tmpdir)) { 
											theWebUI.fManager.actiontoken = data.tmpdir;
											theWebUI.fManager.actiontimeout = setTimeout(theWebUI.fManager.action.stats, 1000);
										} else { theWebUI.fManager.cleanactions();}
								}
 							});
			},

			getlisting: function (dir) {
			
					$('#fManager_data table').addClass('disabled_table');
					theWebUI.fManager.inaction = true;

					this.request('action=list&dir='+encodeURIComponent(dir), 
								function (data) {
											theWebUI.fManager.inaction = false;
											theWebUI.fManager.parseReply(data, dir);
											
								},
								function () {log(theUILang.fErrMsg[9]);},
								function () {log(theUILang.fErrMsg[10]+' - '+dir);}
					);

			},


			stats: function (diag) {

					theWebUI.fManager.action.request('action=getlog&target='+encodeURIComponent(theWebUI.fManager.actiontoken)+'&to='+theWebUI.fManager.actionlp, 
								function (data) { 
									theWebUI.fManager.actionstats = data.status;
									theWebUI.fManager.actionlp = data.lp;
									theWebUI.fManager.cmdlog(data.lines);

									if(!theWebUI.fManager.isErr(data.errcode) && (data.status < 1)) {
										theWebUI.fManager.actiontimeout = setTimeout(theWebUI.fManager.action.stats, 1000);
									} else { 
										theWebUI.fManager.cleanactions();
										if (theWebUI.fManager.curpath == theWebUI.fManager.workpath) { theWebUI.fManager.Refresh(); }
									}
								}
					);

			}


	},

	actionCheck: function (diag) {

		if((this.actiontimeout > 0) && (this.activediag != diag)) {return null;}

		if(!this.dialogs[diag].hasOwnProperty('funct')) {return null;}

 		var args = "";

		var i = (arguments.length > 1) ? 1 : 0;

  		for (i = i; i < arguments.length; i++) {
			
			var rg;

			switch($type(arguments[i])) { 
				case 'string':
					rg = '"'+this.addslashes(arguments[i])+'"';
			

					break;
				default:
					rg = arguments[i];
			}

			args +=  rg + ($type(arguments[i+1]) ? ',' : '');

		}

			return 'theWebUI.fManager.'+this.dialogs[diag].funct+'('+args+')';
		

	},


	addslashes: function (str) {
	// http://phpjs.org/functions/addslashes:303
   		 return (str + '').replace(/[\\"\/]/g, '\\$&').replace(/\u0000/g, '\\0');
	},


	actStart: function (diag) {


			this.makeVisbile('fMan_Console');
			var loader = './images/ajax-loader.gif';
			if(thePlugins.isInstalled('create')) {
				loader = './plugins/create/images/ajax-loader.gif';
			}
			$('#fMan_Console .buttons-list').css( "background", "transparent url("+loader+") no-repeat 15px 2px" );
			$(".fMan_Stop").attr('disabled',false);
			this.activediag = diag;
			if (this.settings.cleanlog) {$('#fMan_ClearConsole').click();} else { this.cmdlog("-------\n");}

			this.cmdlog(theUILang.fStarts[diag]+"\n");

			theDialogManager.hide('fMan_'+diag);
	},


	actStop: function() {
			this.loaderHide();
			this.action.request('action=kill&target='+encodeURIComponent(theWebUI.fManager.actiontoken));
			this.cleanactions();
	},


	Archive: function(name, ext) {



		if(!(theWebUI.fManager.actiontoken.length > 1)) {

			this.doSel('CArchive');

			$('#fMan_CArchivebpath').val(this.homedir+this.curpath+this.recname(name)+'.'+this.archives.types[ext]);

			var type = $('#fMan_archtype').empty();

			$.each(this.archives.types, function(index, value) { 

				var opt = '<option value="'+index+'">'+value.toUpperCase()+'</option>';
				type.append((index == ext) ? $(opt).attr('selected', 'selected').get(0) : opt);
			});

			type.change();
		}

		this.makeVisbile('fMan_CArchive');
	},


	basename: function(str) {
			
			var isdir = this.isDir(str);
			var path = this.trimslashes(str);

			var bname = path.split('/').pop();

			return ((isdir) ? bname+'/' : bname);
	},

	buildList: function (diag) {

			var checks = $('#'+diag+' .checklist input:checked');
			if (checks.size() == 0) {alert("Nothing is not a option"); return false;}
			
			checks.each(function(index, val) {
				theWebUI.fManager.actionlist[index] = theWebUI.fManager.addslashes(decodeURIComponent(val.value));

			});

			return true;
	},


	checkInputs: function (diag, forcedir) {


			forcedir = $type(forcedir) ? true : false;

			var path = $.trim($('#fMan_'+diag+'bpath').val());
				
			if(!path) {theDialogManager.hide('fMan_'+diag); return false;}
			if(path.length < this.homedir.length) {alert(theUILang.fDiagNoPath); return false}

			path = path.split(this.homedir);
			path = this.trimslashes(path[1]);

			if ((path == this.trimslashes(this.curpath)) && !forcedir) { alert(theUILang.fDiagNoPath); return false;}

			var funky = this.trimslashes(this.curpath) ? this.trimslashes(path.split(this.trimslashes(this.curpath)+'/')[1]).split('/').shift() : path.split('/').shift();
			if (	this.isChecked('fMan_'+diag, this.basename(path)) || 
				this.fileExists(funky)) {alert(theUILang.fDiagNoPath); return false;}

			return '/'+path;
	},

	cleanactions: function () {

		$(".fMan_Stop").attr('disabled',true);
		clearTimeout(theWebUI.fManager.actiontimeout);
		this.loaderHide();
		theWebUI.fManager.activediag = '';
		theWebUI.fManager.actionlist = {};
		theWebUI.fManager.actionstats = 0;
		theWebUI.fManager.actiontoken = 0;
		theWebUI.fManager.actiontimeout = 0;
		theWebUI.fManager.actionlp = 0;
	},

	cleanlog: function() {

		$('#fMan_ConsoleLog pre').empty();
	},

	cmdlog: function (text) {

		var console = $('#fMan_ConsoleLog');

		if(browser.isIE) {console.innerHTML = "<pre>"+console.html()+text+"</pre>";
		} else {console.children('pre').append(text);}

		console[0].scrollTop = console[0].scrollHeight;
	},



	changedir: function (target) {
	
		var dir;

		if(target == this.getLastPath(this.curpath)) {
			dir = this.getLastPath(this.curpath);
		} else if (target == '/') {dir = target;} else {
			dir = this.curpath+target;
		}

		this.action.getlisting(dir);			
	},


	Copy: function(diag) {
		$('#fMan_'+diag+'bpath').val(this.homedir+this.curpath);
		this.doSel(diag);

	},

	createDir: function (dirname) {

			$('#fMan-NewDirPath').text(theWebUI.fManager.curpath);
			this.makeVisbile('fMan_mkdir');
	},


	createScreenshots: function (target) {

		if(!(theWebUI.fManager.actiontoken.length > 1)) {

				$('#fMan_Screenshotslist').html(theWebUI.fManager.curpath+'<strong>'+target+'</strong>');
				$('#fMan_Screenshotsbpath').val(this.homedir+this.curpath+'screens_'+this.recname(target)+'.png');
				$('#fMan_Screenshots .fMan_Start').attr('disabled',false); 
		}

		this.makeVisbile('fMan_Screenshots');
	},


	createT: function (target) {

			$('#path_edit').val(this.homedir+this.curpath+target);
			if($('#tcreate').css('display') == 'none') {theWebUI.showCreate();}
	},


	createTable: function() {

		theWebUI.tables.flm = {
			obj: new dxSTable(),
			columns:
			[
				{ text: theUILang.Name,			width: "210px", id: "name",		type: TYPE_STRING },
				{ text: theUILang.Size,			width: "60px",	id: "size",		type: TYPE_NUMBER },
				{ text: '', 			width: "120px", 	id: "time",		type: TYPE_STRING, 	"align" : ALIGN_CENTER},
				{ text: '',			width: "80px", 	id: "type",		type: TYPE_STRING },
				{ text: '',			width: "80px",	id: "perm",		type: TYPE_NUMBER }
			],
			container:	"fManager_data",
			format:		theWebUI.fManager.format,
			onselect:	function(e,id) { 
								if (theWebUI.fManager.inaction) {return false;} 
								theWebUI.fManager.flmSelect(e,id); },
			ondblclick:	function(obj) 
			{ 
				if (theWebUI.fManager.inaction) {return false;} 
				var target = obj.id.slice(5,obj.id.length);

				if(theWebUI.fManager.isDir(target)) {theWebUI.fManager.changedir(target);} 
				else {theWebUI.fManager.getFile(target);}

				return(false);
			}
		};	
		
	},


	doSel: function(diag) {

		diag = 'fMan_'+diag

		var forcedirs = (diag == 'fMan_CreateSFV') ? true : false;

		if(!(theWebUI.fManager.actiontoken.length > 1)) {
			this.generateSelection($('#'+diag+'list'), forcedirs);
			$('#'+diag+' .fMan_Start').attr('disabled',false); 
		}

		this.makeVisbile(diag);
	},

	doArchive: function(button, diag) {


		var archive = this.checkInputs(diag);
		if(archive === false) {return false;}
		if(this.fileExists(this.basename(archive+'/'))) {alert(theUILang.fDiagArchempty); return false;}

		if(!this.buildList('fMan_'+diag)) {return false;}

		var type = $("#fMan_archtype").val();
		var vsize = ((this.archives.types[type] != 'zip') && $("#fMan_multiv").is(':checked') && $("#fMan_vsize").val().match(/^\d+$/)) ? $("#fMan_vsize").val() : 0;
		var compression = $('#fMan_archcompr').val();
		var password = $('#fMan_apassword').val();

		var self = this;

		var options = {	format: self.settings.arcnscheme,
					type: type,
					compression: compression,
					vsize: vsize, 
					password: password};
	
		$(button).attr('disabled',true);
		this.actStart(diag);


		this.action.request('action=archive&target='+encodeURIComponent(archive)+'&mode='+this.encode_string(options)+'&fls='+this.encode_string(theWebUI.fManager.actionlist));

	},

	doExtract: function(button, diag) {

			var path = this.checkInputs(diag, true);
			if(path === false) {return false;}

			var archive = $('#fMang_Archfile').text();

			$(button).attr('disabled',true);

			this.actStart(diag);

			this.action.request('action=extract&target='+encodeURIComponent(archive)+'&to='+encodeURIComponent(path));
	},



	doSFVcheck: function(button, diag) {

			var sfvfile = $('#fMang_ChSFVfile').text();
			$(button).attr('disabled',true);

			this.actStart(diag);

			this.action.request('action=sfvch&target='+encodeURIComponent(sfvfile));
	},


	doSFVcreate: function(button, diag) {

			var file = this.checkInputs(diag);
			if(file === false) {return false;}
			if(this.fileExists(this.basename(file+'/'))) {alert(theUILang.fDiagSFVempty); return false;}
			
			if(!this.buildList('fMan_'+diag)) {return false;}

			$(button).attr('disabled',true);
			this.actStart(diag);

			this.action.request('action=sfvcr&target='+encodeURIComponent(file)+'&fls='+this.encode_string(theWebUI.fManager.actionlist));
	},


	doDelete: function(button, diag) {

			if(!this.buildList('fMan_'+diag)) {return false;}

			$(button).attr('disabled',true);

			this.actStart(diag);

			this.action.request('action=rm&fls='+this.encode_string(theWebUI.fManager.actionlist));

	},


	doMove: function(button, diag) {

			var path = this.checkInputs(diag);

			if(path === false) {return false;}
			if(!this.buildList('fMan_'+diag)) {return false;}

			$(button).attr('disabled',true);

			this.actStart(diag);
			this.action.request('action=mv&to='+encodeURIComponent(path)+'&fls='+this.encode_string(theWebUI.fManager.actionlist));

	},



	doCopy: function(button, diag) {
				
			var path = this.checkInputs(diag);

			if(path === false) {return false;}
			if(!this.buildList('fMan_'+diag)) {return false;}

			$(button).attr('disabled',true);

			this.actStart(diag);
			this.action.request('action=cp&to='+encodeURIComponent(path)+'&fls='+this.encode_string(theWebUI.fManager.actionlist));
	},


	doNewDir: function() {

			var ndn = $.trim($('#fMan-ndirname').val()); 

			if(!ndn.length) {theDialogManager.hide('fMan_mkdir'); return false;}
			if(!this.validname(ndn)) {alert(theUILang.fDiagInvalidname); return false;}

			if(this.fileExists(ndn) || this.fileExists(ndn+'/')) {
					alert(theUILang.fDiagAexist);
					return false;
			}

			theWebUI.fManager.workpath = $('#fMan-NewDirPath').text();

			this.action.request('action=mkd&target='+encodeURIComponent(ndn),
								function (data) { 
									if((theWebUI.fManager.curpath == theWebUI.fManager.workpath) && !theWebUI.fManager.isErr(data.errcode, ndn)) {
										theWebUI.fManager.Refresh();
									}
									theDialogManager.hide('fMan_mkdir');	
								},
								function () {log(theUILang.fErrMsg[9]);},
								function () {log(theUILang.fErrMsg[4]+' - '+ndn);}
			);

	},


	doRename: function () {


			var nn = $.trim($('#fMan-RenameTo').val());
			var on = this.basename($('#fMan-RenameWhat').text());

			if(!nn.length || (on == nn)) {theDialogManager.hide('fMan_Rename'); return false; }
			if(!theWebUI.fManager.validname(nn)) {alert(theUILang.fDiagInvalidname); return false;}

			if(theWebUI.fManager.fileExists(nn) || theWebUI.fManager.fileExists(nn+'/')) {alert(theUILang.fDiagAexist); return false;}


			this.action.request('action=rename&target='+encodeURIComponent(on)+'&to='+encodeURIComponent(nn), 
								function (data) { 
									if((theWebUI.fManager.curpath == theWebUI.fManager.workpath) && !theWebUI.fManager.isErr(data.errcode, on)) {
										theWebUI.fManager.Refresh();
									}
									theDialogManager.hide('fMan_Rename');	
								},
								function () {log(theUILang.fErrMsg[11]);},
								function () {log(theUILang.fErrMsg[12]+' - Rename: '+on);}
			);

	},

	doScreenshots: function (button, diag) {


			var screen_file = this.checkInputs(diag);
			if(screen_file === false) {return false;}

			var video = $('#fMan_Screenshotslist').text();

			$(button).attr('disabled',true);

			this.actStart(diag);

			this.action.request('action=scrn&target='+encodeURIComponent(video)+'&to='+encodeURIComponent(screen_file));

	},

	encode_string: function(str) {

		return encodeURIComponent(this.json_encode(str));

	},

	extract: function(what, here) {
		if(!(theWebUI.fManager.actiontoken.length > 1)) {
				$('#fMang_Archfile').html(theWebUI.fManager.curpath+'<strong>'+what+'</strong>');
				$('#fMan_Extractbpath').val(this.homedir+this.curpath+(here ? '' : this.recname(what)));
				$('#fMan_Extract .fMan_Start').attr('disabled',false); 
		}

		this.makeVisbile('fMan_Extract');
	},


	fileExists: function(what) {


				var table = theWebUI.getTable("flm");
				var exists = false;

				try {if(table.getValueById('_flm_'+what, 'name')) {
					throw true;
					} else { 
					throw false; }
 				 }
				catch(dx) {
					if(dx === true) {exists = dx;}
 				 }

				return exists;
	},

	formatDate: function  (timestamp) {

		if(timestamp) {

			var d = new Date(timestamp*1000);

     			var times = {
                			s: d.getSeconds(),
                			m: d.getMinutes(),
               			h: d.getHours(),

                 			d: d.getDate(),
                 			M: d.getMonth(),
                 			y: d.getFullYear()
 			};

    			for (i in times) {
       			if(i == 'M') { times[i]++;}
         			if (times[i] < 10) {times[i] = "0"+times[i];}
    			}

			var ndt = this.settings.timef.replace(/%([dMyhms])/g, function (m0, m1) { return times[m1]; });
			return ndt;
		} else {return '';}
	},

	formatPerm: function (octal) {

			var pmap = ['-', '-xx', '-w-', '-wx', 'r--', 'r-x', 'rw-', 'rwx'];
			var arr = octal.split('');
			
			var out = '';

			for(i = 0; i < arr.length; i++) {
                            out += pmap[arr[i]];
			}
			return out;

	},
 

	format: function(table,arr){
		for(var i in arr)
		{
			if(arr[i]==null) {
				arr[i] = '';
			} else {
   				switch(table.getIdByCol(i)){
				case 'name':
					if(theWebUI.fManager.settings.stripdirs && theWebUI.fManager.isDir(arr[i])) {arr[i] = theWebUI.fManager.trimslashes(arr[i]);}
					break;
      				case 'size' : 
      					if(arr[i] != '') {arr[i] = theConverter.bytes(arr[i], 2);}
      					break;
      				case 'type' : 
      					if(theWebUI.fManager.isDir(arr[i])) { arr[i] = '';} 
					else {arr[i] = theWebUI.fManager.getExt(arr[i]);}
      					break;
				case 'time' :
					arr[i] = theWebUI.fManager.formatDate(arr[i]);
					break;
				case 'perm':
					if (theWebUI.fManager.settings.permf > 1) {arr[i] = theWebUI.fManager.formatPerm(arr[i]);}
					break;
	      			}
			}
	   	}
		return(arr);
	},


	fPath: function() {


		var cpath = $('#fMan_pathsel');


		cpath.children('option').each(function(index, val) {
			if(val.value == theWebUI.fManager.curpath) {$(this).attr('selected', 'selected');}
		});

		for (var i = 0; i < this.paths.length; i++) {
			if(this.paths[i] == this.curpath) {return false;}	
		}


		if(this.paths.length > this.pathlists) {this.paths.pop();}
		this.paths.unshift(this.curpath);

		cpath.empty();

		for (var i = 0; i < this.paths.length; i++) {

			cpath.append('<option>'+this.paths[i]+'</option>');
			if((this.paths[i] != '/') && (i == (this.paths.length - 1))) {cpath.append('<option value="/">/</option>');}
		}
		
		
	},

	generateSelection: function(holder, forcefiles) {

		forcefiles = $type(forcefiles) ? forcefiles : $type(forcefiles);

		var container = holder.children('ul');
		container.empty();
	
		var sr = theWebUI.getTable("flm").rowSel;
		var topdir = this.getLastPath(this.curpath);

		for (i in sr) {
			var name = i.split('_flm_');
			name = name[1];

			if (sr[i] && (name != topdir) && (!forcefiles || !this.isDir(name))) {
				container.append('<li><label><input type="checkbox" value="'+encodeURIComponent(name)+'" checked="checked" />'+name+'</label></li>');
			}
		}
	},


	getExt: function(element) {

		if(!$type(element)) {return '';}
		var fname = element.split('.');		
		return (((element.charAt(1) == '.') && (fname.length == 2)) || (fname.pop() == element)) ? 'none' : element.split('.').pop();
	},


	getLastPath: function (path) {

		var last = '/';
		path = this.trimslashes(path);

		if (path) {
   			var ar = path.split('/');
   			ar.pop();
    			last += ar.join('/');
			if(ar.length > 0) {last += '/';}
		}
			
		return(last);
	},


	getFile: function (id) {

		$("#fManager_dir").val(theWebUI.fManager.curpath);
		$("#fManager_getfile").val(id);
		$("#fManager_getdata").submit();

	},


	getICO: function (element) {

			if(this.isDir(element)) { return('Icon_Dir'); } 

			var iko;

			element = this.getExt(element).toLowerCase();

			if(element.match(/^r[0-9]+$/)) { return('Icon_partRar'); }

			switch(element){
      			
      				case 'mp3' : 
 					iko = 'Icon_Mp3';
      					break;
				case 'avi':
				case 'mp4':
				case 'wmv':
				case 'mkv':
				case 'divx':
				case 'mov':
				case 'flv':
				case 'mpeg':
      					iko = 'Icon_Vid';
     					break;
				case 'bmp':
				case 'jpg':
				case 'jpeg':
				case 'png':
				case 'gif':
      					iko = 'Icon_Img';
      					break;
				case 'nfo':
      					iko = 'Icon_Nfo';
      					break;
				case 'sfv':
      					iko = 'Icon_Sfv';
      					break;
				case 'rar':
      					iko = 'Icon_Rar';
      					break;
				case 'zip':
      					iko = 'Icon_Zip';
      					break;
				case 'tar':
				case 'gz':
				case 'bz2':
					iko = 'Icon_gnuARCH';
      					break;
				case 'torrent':
      					iko = 'Icon_Torrent';
      					break;
				default: 
					iko = 'Icon_File';			
	      		}

			return(iko);
	},


	isChecked: function(diag, what) {

			var ret = false;

			$('#'+diag+' .checklist input:checked').each(function(index, val) {
				if((what == decodeURIComponent(val.value)) || (what+'/' == decodeURIComponent(val.value))) {ret = true; return false;}
			});

			return ret;
	},

	isDir: function(element) {return	 (element.charAt(element.length-1) == '/') },
	
	isErr: function(errcode, extra) {


		if(!$type(extra)) {extra = '';}

		if(errcode > 0) {
			log('FILE MANAGER: '+theUILang.fErrMsg[errcode]+" : "+extra); 
			return true;
		}

		return false;
	},

	json_encode: function (obj) {
	switch($type(obj))
	{
		case "number":
			return(String(obj));
		case "boolean":
			return(obj ? "1" : "0");
		case "string":
			return('"'+obj+'"');
		case "array":
		{
		        var s = '';
		        $.each(obj,function(key,item)
		        {
		                if(s.length)
                			s+=",";
		        	s += theWebUI.fManager.json_encode(item);
		        });
			return("["+s+"]");
		}
		case "object":
		{
		        var s = '';
		        $.each(obj,function(key,item)
		        {
		                if(s.length)
                			s+=",";
		        	s += ('"'+key+'":'+theWebUI.fManager.json_encode(item));
		        });
			return("{"+s+"}");
		}
	}
	return("null");
	},

	loaderHide: function () {

		$('#fMan_Console .buttons-list').css( "background", "none" );
	},

	makeVisbile: function (what) {

		if($('#'+what).css('overflow', 'visible').css('display') == 'none') {theDialogManager.toggle(what);}

	},

	
	mediainfo: function(what) {



			this.cleanlog();
			this.cmdlog("Fetching...");

			var self = this;

			this.makeVisbile('fMan_Console');
			var loader = './images/ajax-loader.gif';
			if(thePlugins.isInstalled('create')) {
				loader = './plugins/create/images/ajax-loader.gif';
			}
			$('#fMan_Console .buttons-list').css( "background", "transparent url("+loader+") no-repeat 15px 2px" );
			$(".fMan_Stop").attr('disabled',true);


			this.action.request('action=minfo&target='+encodeURIComponent(what), 
								function (data) { if(theWebUI.fManager.isErr(data.errcode, what)) {
											self.cmdlog('Failed fetching data');
											return false;
											}
									self.cleanlog();
									self.cmdlog(data.minfo);
								}
			);

			this.loaderHide();


	},

	optSet: function () {

		if(theWebUI.configured) {	
			var needsave = false;

			for (var set in theWebUI.fManager.settings) { 
				if($type(theWebUI.settings["webui.fManager."+set])) { 
					theWebUI.fManager.settings[set] = theWebUI.settings["webui.fManager."+set]
				} else {
					theWebUI.settings["webui.fManager."+set] = theWebUI.fManager.settings[set]; 
					needsave = true;
				}
			}

			if(needsave) {theWebUI.save();}

		} else { setTimeout(arguments.callee,500);}
	},


	parseReply: function(reply, dir) {

		$('#fManager_data table').removeClass('disabled_table');

		if(this.isErr(reply.errcode, dir)) {return false;}

		this.curpath = dir;
		this.fPath();

		this.TableData(reply.listing);
		
	},


	Refresh: function() {this.action.getlisting(this.curpath);},


	rename: function (what) {

		var type = (this.isDir(what)) ? 'Directory:' : 'File:';
		what = this.trimslashes(what)

		$('#fMan-RenameType strong').text(type);
		$('#fMan-RenameWhat').html(theWebUI.fManager.curpath+'<strong>'+what+'</strong>');
		$('#fMan-RenameTo').val(what);

		this.makeVisbile('fMan_Rename');

	},

	renameStuff: function() {

				var table = theWebUI.getTable("flm");

				if(table.created && plugin.allStuffLoaded) {

					table.renameColumnById('time',theUILang.fTime);
					table.renameColumnById('type',theUILang.fType);
					table.renameColumnById('perm',theUILang.fPerm);

					log('FILE MANAGER ignited');

				} else { setTimeout(arguments.callee,500);}

	},


	resize: function (w, h) {

		if(w!==null) {w-=8;}

		if(h!==null) {
			h-=($("#tabbar").height());
			h-=($("#fMan_navpath").height())
			h-=2;
        	}

		var table = theWebUI.getTable("flm");
		if(table) {table.resize(w,h);}
	},


	recname: function (what) {


			if(this.isDir(what)) { return this.trimslashes(what);}

			var ext = this.getExt(what);
			var recf = what.split(ext);

			if(recf.length > 1) {
   				recf.pop();
    				recf = recf.join(ext).split('.');
   				if(recf[recf.length-1] == '') {
        				recf.pop();
    				}
    				return(recf.join('.'));
			}

			return(recf.join(''));

	},

	sfvCreate: function(what) {

		$('#fMan_CreateSFVbpath').val(this.homedir+this.curpath+this.recname(what)+'.sfv');
		theWebUI.fManager.doSel('CreateSFV');

	},

	sfvCheck: function(what) {

		if(!(theWebUI.fManager.actiontoken.length > 1)) {
			$('#fMang_ChSFVfile').html(theWebUI.fManager.curpath+'<strong>'+what+'</strong>');
			$('#fMan_CheckSFV .fMan_Start').attr('disabled',false); 
		}

		this.makeVisbile('fMan_CheckSFV');
	},


	trimslashes: function(str) {

				if(!$type(str)) {return '';}

				var ar = str.split('/');
				var rar = [];

				for(i = 0; i < ar.length; i++) {
    					if(ar[i]) {rar.push(ar[i]);}
				}

				return(rar.join('/'));
	},


	TableData: function(data) { 

		var table = theWebUI.getTable("flm");
		table.clearRows();

		if(this.curpath != '/') {
				table.addRowById({name: '../', size: '', time: '', type: '/', perm: ''}, "_flm_"+this.getLastPath(this.curpath), 'Icon_UpD');
		} else { if(data.length < 1) { data = {0: {name: '/', size: '', time: '', perm: ''}}; } }


		$.each(data, function(ndx,file) {

				if(theWebUI.fManager.isDir(file.name)) { var ftype = 0;} else {var ftype = 1;}
   			 
				table.addRowById({
					name: file.name,
					size: file.size,
					time: file.time,
					type: ftype+file.name,
					perm: file.perm
				}, "_flm_"+file.name, theWebUI.fManager.getICO(file.name));
				if(!theWebUI.fManager.settings.showhidden && (file.name.charAt(0) == '.')) {table.hideRow("_flm_"+file.name);}
		});
		
		table.refreshRows();
	},

	TableRegenerate: function() { 
			var td = theWebUI.getTable("flm").rowdata;
			var old = {};

			var x = 0;

			for (i in td) {
				if(td[i].icon == 'Icon_UpD') {continue;}
				old[x] = {name: td[i].data[0], size: td[i].data[1], time: td[i].data[2], type: td[i].data[3], perm: td[i].data[4]}
				x++;
			}

			this.TableData(old);
	},


	validname: function (what) {return (what.split('/').length > 1) ? false : true;},

	viewNFO: function (what, mode) {


			this.makeVisbile('fMan_Nfo');
			$("#fMan_nfoformat option[value='"+mode+"']").attr('selected', 'selected');

			var cont = $('#nfo_content pre');
			cont.empty();
			cont.text('			Loading...');
			
			$("#fMan_nfofile").val(what);


			this.action.request('action=nfo&mode='+mode+'&target='+encodeURIComponent(what), 
								function (data) { 
									
									if(theWebUI.fManager.isErr(data.errcode, what)) {
										cont.text('Failed fetching .nfo data');
										return false;
									}

									if(browser.isIE) {document.getElementById("nfo_content").innerHTML = "<pre>"+data.nfo+"</pre>";} 
									else {cont.html(data.nfo);}

								}
			);


	}




};
	


theWebUI.fManager.flmSelect = function(e, id) {

		if($type(id) && (e.button == 2)) {

		       theContextMenu.clear();

			var table = theWebUI.getTable("flm");
			var flm = theWebUI.fManager;

			var target = id.split('_flm_')[1];
			var targetIsDir = flm.isDir(target);
		
			theContextMenu.add([theUILang.fOpen, (table.selCount > 1) ? null : (targetIsDir ? function () {flm.changedir(target);} : function () {flm.getFile(target);})]);

			if(target != flm.getLastPath(flm.curpath)) {

				flm.workpath = flm.curpath;

				var fext = flm.getExt(target);

				if(fext == 'nfo') {
					theContextMenu.add([CMENU_SEP]);
					theContextMenu.add([theUILang.fView, function() {flm.viewNFO(target, 1);}]); 
					theContextMenu.add([CMENU_SEP]);
				} 

				theContextMenu.add([theUILang.fCopy, flm.actionCheck('Copy')]);
				theContextMenu.add([theUILang.fMove, flm.actionCheck('Move')]);
				theContextMenu.add([theUILang.fDelete, flm.actionCheck('Delete')]);

				theContextMenu.add([theUILang.fRename, !(table.selCount > 1) ? flm.actionCheck('Rename', target) : null]);

				theContextMenu.add([CMENU_SEP]);


				if(fext.match(/^(zip|rar|tar|gz|bz2)$/i) && !(table.selCount > 1)) {
					theContextMenu.add([theUILang.fExtracta, flm.actionCheck('Extract', target, false)]);
					theContextMenu.add([theUILang.fExtracth, flm.actionCheck('Extract', target, true)]);
					theContextMenu.add([CMENU_SEP]);
				}


				var create_sub = [];

				create_sub.push([theUILang.fcNewTor, thePlugins.isInstalled('create') && !(table.selCount > 1) ? function () {flm.createT(target);} : null]);
				create_sub.push([CMENU_SEP]);
				create_sub.push([theUILang.fcNewDir, "theWebUI.fManager.createDir()"]);
				create_sub.push([theUILang.fcNewArchive, flm.actionCheck('CArchive', target, 0)]);
				create_sub.push([CMENU_SEP]);
				create_sub.push([theUILang.fcSFV, !targetIsDir ? flm.actionCheck('CreateSFV', target) : null]);

				create_sub.push([theUILang.fcScreens, (thePlugins.isInstalled('screenshots') && !targetIsDir && flm.getExt(target).match(new RegExp("^("+thePlugins.get('screenshots').extensions.join('|')+")$", "i")) && !(this.actiontimeout > 0)) ? flm.actionCheck('Screenshots', target) : null]);

				theContextMenu.add([CMENU_CHILD, theUILang.fcreate, create_sub]);

				theContextMenu.add([theUILang.fcheckSFV, (fext == 'sfv') ? flm.actionCheck('CheckSFV', target) : null]);
				theContextMenu.add([theUILang.fMediaI, (thePlugins.isInstalled('mediainfo') && !targetIsDir && !(this.actiontimeout > 0)) ? function() {flm.mediainfo(target); } : null]);

			} else { theContextMenu.add([theUILang.fcNewDir, "theWebUI.fManager.createDir()"]); }

			theContextMenu.add([CMENU_SEP]);
			theContextMenu.add([theUILang.fRefresh, "theWebUI.fManager.Refresh()"]);

	   		theContextMenu.show();
			return(true);
		}
		return(false);
}







theWebUI.fManager.createDialogs = function () {


theWebUI.fManager.dialogs = {


	optPan: {
		title: 'fManager',
		content: '<fieldset>'+
		'  <legend>Display Settings</legend>'+
		'  <table width="100%" border="0" cellspacing="0" cellpadding="0">'+
		'  <tr>'+
		'    <td>Paths history number of values:</td>'+
		'    <td><input type="text" name="textfield" class="Textbox num1" id="fMan_Opthistpath" value="" /></td>'+
		'  </tr>'+
		'  <tr>'+
		'    <td>Strip trailing slashes from directory names:</td>'+
		'    <td><input type="checkbox" name="fMan_Optstripdirs" id="fMan_Optstripdirs" value="true" /></td>'+
		'  </tr>'+
		'  <tr>'+
		'    <td>Show hidden files:</td>'+
		'    <td><input type="checkbox" name="fMan_Optshowhidden" id="fMan_Optshowhidden" value="true" /></td>'+
		'  </tr>'+
		'  <tr>'+
		'    <td>Clean console log automatically:</td>'+
		'    <td><input type="checkbox" name="fMan_Optcleanlog" id="fMan_Optcleanlog" value="true" /></td>'+
		'  </tr>'+
		'  <tr>'+
		'    <td>Permissions format:</td>'+
		'    <td><select name="fMan_Optpermf" id="fMan_Optpermf">'+
		'      <option value="1">Octal (0755)</option>'+
		'      <option value="2">Symbolic (-rw)</option>'+
		'    </select></td>'+
		'  </tr>'+
		'  <tr>'+
		'    <td>Date - Time format:</td>'+
		'    <td><input type="text" name="fMan_Opttimef" class="TextboxLarge" style="width: 160px;" id="fMan_Opttimef" value=""/></td>'+
		'  </tr>'+
		'<tr>'+
		'	<td>'+
		'<table border="0" cellspacing="0" cellpadding="0">'+
		'  <tr>'+
		'    <td><strong>%s</strong> - seconds</td>'+
		'    <td><strong>%m</strong> - minutes</td>'+
		'  </tr>'+
		'  <tr>'+
		'    <td><strong>%h</strong> - hours</td>'+
		'    <td><strong>%d</strong> - day</td>'+
		'  </tr>'+
		'  <tr>'+
		'    <td><strong>%M</strong> - month</td>'+
		'    <td><strong>%y </strong>- year</td>'+
		'  </tr>'+
		'</table>'+
		'	</td>'+
		'<td>'+
		'<table width="100%" border="0" align="right" cellpadding="0" cellspacing="0">'+
		'  <tr>'+
		'    <td>Format: <strong>%d</strong>.<strong>%M</strong>.<strong>%y</strong> <strong>%h</strong>:<strong>%m</strong>:<strong>%s</strong></td>'+
		'  </tr>'+
		'  <tr>'+
		'    <td>Equals to: <strong>03</strong>.<strong>12</strong>.<strong>2011</strong> <strong>22</strong>:<strong>55</strong>:<strong>47</strong></td>'+
		'  </tr>'+
		'</table>'+
		'</td>'+
		'</tr>'+
		'  </table>'+
		'</fieldset><fieldset>'+
		'  <legend>Archive Settings</legend>'+
		'  <table width="100%" border="0" cellspacing="0" cellpadding="0"><tr>'+
		'    <td>Multi-volume format:</td>'+
		'    <td><select name="fMan_Optarcnscheme" id="fMan_Optarcnscheme">'+
		'      <option value="new" selected="selected">NEW - .part1.rar</option>'+
		'      <option value="old">OLD - .r01</option>'+
		'    </select></td>'+
		'  </tr>'+
		'  </table>'+
		'</fieldset><fieldset>'+
		'  <legend>Screenshot Settings</legend>'+
		'  <table width="100%" border="0" cellspacing="0" cellpadding="0"><tr>'+
		'    <td>Screens rows:</td>'+
		'    <td><input type="text" name="textfield" class="Textbox num1" id="fMan_Optscrows" value="" /></td>'+
		'  </tr>'+
		'<tr>'+
		'    <td>Screens columns:</td>'+
		'    <td><input type="text" name="textfield" class="Textbox num1" id="fMan_Optsccols" value="" /></td>'+
		'  </tr>'+
		'<tr>'+
		'    <td>Thumbnail width:</td>'+
		'    <td><input type="text" name="textfield" class="Textbox num1" id="fMan_Optscwidth" value="" /></td>'+
		'  </tr>'+
		'  </table>'+
		'</fieldset>'
	},

	CArchive: {
		title: 'fDiagCArchive',
		modal: true,
		funct: "Archive",
		content: 
			'<fieldset><legend>'+theUILang.fDiagCArchiveSel+'</legend>'+
				'<div id="fMan_CArchivelist" class="checklist"><ul></ul></div>'+
			'</fieldset><fieldset><legend>Options:</legend>'+
				'<label>'+theUILang.fDiagArchive+'<input type="text" id="fMan_CArchivebpath" name="fMan_CArchivebpath" class="TextboxLarge" style="width:320px;" autocomplete="off"/></label>'+
				'<input type="button" value="..." id="fMan_CArchivebbut" class="Button aright"><br/>'+
 				' <label style="float: left;">'+theUILang.fDiagCArchType+
 				'  <select name="fMan_archtype" id="fMan_archtype">'+
   				'</select>'+
				'</label>'+
 				' <label style="float: left; margin-left: 10px;">'+theUILang.fDiagCompression+
  				'  <select name="fMan_archcompr" id="fMan_archcompr">'+
    				'</select>'+
  				'</label>'+
				'<label style="float: right;">'+theUILang.fDiagCArchVsize+'<input name="fMan_vsize" class="fMan_CArchiveRAR Textbox num1" type="text" value="" id="fMan_vsize" disabled="true" /></label>'+
				'<label style="float: right;"><input name="fMan_multiv" type="checkbox" value="1" class="fMan_CArchiveRAR" id="fMan_multiv" style="margin-right: 5px; margin-top:8px;"/></label>'+
				'<label style="clear:both; float: left;">Password: <input name="fMan_apassword" class="TextboxLarge fMan_CArchiveRAR" type="text" value="" id="fMan_apassword" /></label>'+

			'</fieldset>'+
			'<div style="clear:both;"></div>'

	},


	CheckSFV: {
		title: 'fDiagSFVCheck',
		modal: false,
		funct: 'sfvCheck',
		content: 
			'<fieldset><legend>'+theUILang.fDiagSFVCheckf+'</legend>'+
				'<div id="fMang_ChSFVfile" style="width:440px;"></div>'+
			'</fieldset>'
	},


	Console: {
		title: 'fDiagConsole',
		modal: false,
		content: 
			'<fieldset><legend>Command log:</legend>'+
				'<div id="fMan_ConsoleLog" style="width:500px; height:310px;"><pre></pre></div>'+
			'</fieldset>'
	},


	Copy: {
		title: 'fDiagCopy',
		modal: true,
		funct: 'Copy',
		content: '<fieldset><legend>'+theUILang.fDiagCopySel+'</legend></fieldset>'
	},


	CreateSFV: {
		title: 'fDiagSFVCreate',
		modal: true,
		funct: 'sfvCreate',
		content: '<fieldset><legend>'+theUILang.fDiagSFVCreateSel+'</legend></fieldset>'
	},


	Delete: {
		title: 'fDiagDelete',
		modal: true,
		funct: 'doSel',
		content: '<fieldset><legend>'+theUILang.fDiagDeleteSel+'</legend></fieldset>'
	},


	Extract: {
		title: 'fDiagExtract',
		modal: true,
		funct: "extract",
		content: 
			'<fieldset><legend>'+theUILang.fDiagArchive+'</legend>'+
				'<div id="fMang_Archfile" style="width:460px;"></div>'+
			'</fieldset>'

	},

	mkdir: {
		title: 'fDiagmkdir',
		modal: false,
		content: 	
			'<div><strong>Path of creation: </strong></div>'+
			'<div id="fMan-NewDirPath" style="padding-top:3px; padding-bottom:4px; width:200px;"></div>'+
			'<fieldset><legend>'+theUILang.fDiagndirname+'</legend>'+
				'<input type="text" name="fMan-ndirname" id="fMan-ndirname" style="width:200px;" />'+						
			'</fieldset>'
	},


	Move: {
		title: 'fDiagMove',
		modal: true,
		funct: 'doSel',
		content: '<fieldset><legend>'+theUILang.fDiagMoveSel+'</legend></fieldset>'
	},

	Nfo: {
		title: 'fDiagNFO',
		modal: false,
		content: 
			'<table width="100%" border="0" cellspacing="0" cellpadding="0"><tr><td width="8%"><strong>Format:</strong></td>'+
				'<td width="92%"><select id="fMan_nfoformat" name="fMan_nfoformat">'+
						'<option value="1" selected="selected">DOS</option>'+
						'<option value="0">WIN</option>'+
				'</select><input name="fMan_nfofile" type="hidden" id="fMan_nfofile" value="" /></td></tr>'+
			'</table>'+
			'<div id="nfo_content" style="width: 549px;"><pre>Loading...</pre></div>'
	},

	Rename: {
		title: 'fDiagRename',
		modal: false,
		funct: 'rename',
		content: 
			'<div id="fMan-RenameType"><strong></strong></div>'+
			'<div id="fMan-RenameWhat" style="padding-top:3px; padding-bottom:4px; width:200px;"></div>'+
			'<fieldset><legend>'+theUILang.fDiagRenameTo+'</legend>'+
				'<input type="text" name="fMan-RenameTo" id="fMan-RenameTo" style="width:200px;" />'+
			'</fieldset>'
	},

	Screenshots: {
		title: 'fcScreens',
		modal: true,
		funct: 'createScreenshots',
		content: '<fieldset><legend>Video file:</legend>'+	
			'</fieldset>'
	}
}


		plugin.attachPageToOptions($("<div>").attr("id",'fMan_optPan').html(this.dialogs.optPan.content).get(0),theUILang[this.dialogs.optPan.title]);
		delete this.dialogs.optPan;


		var buttons = '<div class="aright buttons-list">'+
					'<input type="button" class="fMan_Start Button" value="'+theUILang.fDiagStart+'" class="Button" />'+
					'<input type="button" class="Cancel Button" value="'+theUILang.fDiagClose+'"/>'+
				'</div>';
		var consbut = '<div class="aright buttons-list">'+
					'<input type="button" id="fMan_ClearConsole" class="Button" value="Clear" class="Button" />'+
					'<input type="button" class="fMan_Stop Button" value="'+theUILang.fDiagStop+'" class="Button" disabled="true"/>'+
					'<input type="button" class="Cancel Button" value="'+theUILang.fDiagClose+'"/>'+
				'</div>';


		var browsediags = {
			CreateSFV: theUILang.fDiagSFVHashfile,
			Move: theUILang.fDiagMoveTo, 
			Copy: theUILang.fDiagCopyTo,
			Extract: theUILang.fDiagTo,
			Screenshots: 'Screens image:'
		}
		
		var pathbrowse;
		
		for (i in this.dialogs) {

			var dcontent = this.dialogs[i].content;

			if($type(browsediags[i])) { 

				if ((i != 'Extract') || (i != 'Screenshots')) {dcontent = $(this.dialogs[i].content).append('<div id="fMan_'+i+'list" class="checklist"><ul></ul></div>');}

				pathbrowse = $('<fieldset>').html($('<legend>').text(browsediags[i])).
						append($('<input type="text" style="width:350px;" autocomplete="off" />').attr('id', 'fMan_'+i+'bpath').addClass('TextboxLarge')).
						append($('<input type="button" value="..." style="float: right;" />').attr('id', 'fMan_'+i+'bbut').addClass('Button aright'));
			} else 
			if ( i == 'Delete') {dcontent = $(this.dialogs[i].content).append('<div id="fMan_'+i+'list" class="checklist"><ul></ul></div>'); pathbrowse = ''}
			else {pathbrowse = '';}


			var fcontent = $('<div>').html($('<div>').addClass('cont fxcaret').html(dcontent).append(pathbrowse)).append((i != 'Nfo') ? ((i == 'Console') ? consbut : buttons) : '').get(0);
			theDialogManager.make('fMan_'+i, theUILang[this.dialogs[i].title], fcontent, this.dialogs[i].modal);
		}

/* 	
Dialogs button binds bellow:
*/


		$('.fMan_Start').click(function() {

			var diagid = $(this).parents('.dlg-window:first').attr('id');
			diagid = diagid.split('fMan_');

			switch(diagid[1]) {
				case 'CArchive':
					theWebUI.fManager.doArchive(this, diagid[1]);
					break;
				case 'CheckSFV':
					theWebUI.fManager.doSFVcheck(this, diagid[1]);
					break;
				case 'CreateSFV':
					theWebUI.fManager.doSFVcreate(this, diagid[1]);
					break;
				case 'Copy':
					theWebUI.fManager.doCopy(this, diagid[1]);
					break;
				case 'Delete':
					theWebUI.fManager.doDelete(this, diagid[1]);
					break;
				case 'Extract':
					theWebUI.fManager.doExtract(this, diagid[1]);
					break;
				case 'mkdir':
					theWebUI.fManager.doNewDir();
					break;	
				case 'Move':
					theWebUI.fManager.doMove(this, diagid[1]);
					break;	
				case 'Rename':
					theWebUI.fManager.doRename();
					break;	
				case 'Screenshots':
					theWebUI.fManager.doScreenshots(this, diagid[1]);
					break;	
			}


		});

		$('.fMan_Stop').click(function() {

			theWebUI.fManager.cmdlog(theUILang.fStops[theWebUI.fManager.activediag]+"\n");
			theWebUI.fManager.actStop();   

		});


		if(thePlugins.isInstalled("_getdir")) {

			browsediags.CArchive = 'arch';
			var closehandle = function (diagid) {theDialogManager.setHandler('fMan_'+diagid,'afterHide', function () {plugin["bp"+diagid].hide();}); }

			for (sex in browsediags) {
				plugin['bp'+sex] = new theWebUI.rDirBrowser( 'fMan_'+sex, 'fMan_'+sex+'bpath', 'fMan_'+sex+'bbut', null, false );
				closehandle(sex);
			}

		} else {for (sex in browsediags) {$('fMan_'+sex+'bbut').remove(); }}



		$('#fMan_pathsel').change(function() {
				var path = $(this).val();
				if(path == theWebUI.fManager.curpath) { return false; }

				theWebUI.fManager.action.getlisting(path);
		});


		$("#fMan_multiv").change(function() {

			var dis = $(this).is(':checked');
			$("#fMan_vsize").attr("disabled", !dis);
		});

		$("#fMan_archtype").change(function() {

				var type = $(this).val();
				var comp = $("#fMan_archcompr");

				var ext;

				switch(theWebUI.fManager.archives.types[type]) {
					case 'gzip':
						ext = 'tar.gz';
						break;
					case 'bzip2':
						ext = 'tar.bz2';
						break;
					default:
						ext = theWebUI.fManager.archives.types[type];
				}

				$('#fMan_CArchivebpath').val(theWebUI.fManager.recname(theWebUI.fManager.recname($('#fMan_CArchivebpath').val()))+'.'+ext);
				$("#fMan_vsize").attr("disabled", (!$("#fMan_multiv").attr("disabled", (type != 0)).is(':checked') || (type != 0)));
				$('#fMan_apassword').attr("disabled", (type != 0))
				comp.empty();

				for(var i = 0; i < theWebUI.fManager.archives.compress[type].length; i++) {
					  comp.append('<option value="'+i+'">'+theUILang.fManArComp[type][i]+'</option>');
				}

		});


		$("#fMan_nfoformat").change(function() {

				var mode = $(this).val();
				var nfofile = $("#fMan_nfofile").val();

				theWebUI.fManager.viewNFO(nfofile, mode)
		});

		$('#fMan_ClearConsole').click(function() {theWebUI.fManager.cleanlog();});
		$('#fMan_navbut').click(function() {theWebUI.fManager.Refresh();});





		if(!thePlugins.isInstalled('data')) {


			$(document.body).append($("<iframe name='datafrm'/>").css({visibility: "hidden"}).attr( { name: "datafrm", id: "datafrm" } ).width(0).height(0).load(function()
			 {
			 var d = (this.contentDocument || this.contentWindow.document);
			 if(d.location.href != "about:blank")
				try { eval(d.body.innerHTML); } catch(e) {}
		         }));
		}



		$(document.body).append(
			$('<form action="'+theWebUI.fManager.action.requrl+'" id="fManager_getdata" method="post" target="datafrm">'+
				'<input type="hidden" name="dir" id="fManager_dir" value="">'+
				'<input type="hidden" name="target" id="fManager_getfile" value="">'+
				'<input type="hidden" name="action" value="dl">'+
			'</form>').width(0).height(0));

}



plugin.setSettings = theWebUI.setSettings;
theWebUI.setSettings = function() {


			if(plugin.enabled) {
				var needsave = false;

				$('#fMan_optPan').find('input,select').each(function(index, ele) { 
					var inid = $(ele).attr('id').split('fMan_Opt');
					var inval;
		
					if($(ele).attr('type') == 'checkbox') {inval = $(ele).is(':checked') ? true : false; }
					else { inval = $(ele).val(); }

					if(inval != theWebUI.settings["webui.fManager."+inid[1]]) {
						theWebUI.settings["webui.fManager."+inid[1]] = theWebUI.fManager.settings[inid[1]] = inval;
						needsave = true;
					}
				});

				if(needsave) {	theWebUI.save();
							theWebUI.fManager.TableRegenerate(); }
			 }

			plugin.setSettings.call(this);

}



plugin.addAndShowSettings = theWebUI.addAndShowSettings;
theWebUI.addAndShowSettings = function(arg) {
		if(plugin.enabled) {

			$('#fMan_optPan').find('input, select').each(function(index, ele) { 
				var inid = ele.id.split('fMan_Opt');

				if($(ele).attr('type') == 'checkbox') { 
					if(theWebUI.settings["webui.fManager."+inid[1]]) {$(ele).attr('checked', 'checked'); }
				} else if ($(ele).is("select")) { 
					$(ele).children('option[value="'+theWebUI.settings["webui.fManager."+inid[1]]+'"]').attr('selected', 'selected'); 
				} else {
					$(ele).val(theWebUI.settings["webui.fManager."+inid[1]]);
				}
			});
		}
		
		plugin.addAndShowSettings.call(theWebUI,arg);
}




plugin.config = theWebUI.config;
theWebUI.config = function(data) {


		theWebUI.fManager.optSet();
		theWebUI.fManager.createTable();
		theWebUI.fManager.renameStuff();


		var table = this.getTable("flm");
		table.oldFilesSortAlphaNumeric = table.sortAlphaNumeric;
		table.sortAlphaNumeric = function(x, y) 
		{
	
			if(x.key.split('_flm_')[1] == theWebUI.fManager.getLastPath(theWebUI.fManager.curpath)) {return(this.reverse ? 1 : -1);}
			return(this.oldFilesSortAlphaNumeric(x,y));
		}
		table.oldFilesSortNumeric = table.sortNumeric;
		table.sortNumeric = function(x, y)
		{
			if(x.key.split('_flm_')[1] == theWebUI.fManager.getLastPath(theWebUI.fManager.curpath)) {return(this.reverse ? 1 : -1);}
			return(this.oldFilesSortNumeric(x,y));
		}	

		plugin.config.call(this,data);
}


plugin.resizeBottom = theWebUI.resizeBottom;
theWebUI.resizeBottom = function( w, h ) {

				theWebUI.fManager.resize(w, h);
				plugin.resizeBottom.call(this,w,h);
}


plugin.onShow = theTabs.onShow;
theTabs.onShow = function(id) {

	if(id == "FileManager") {
			theWebUI.getTable('flm').refreshRows();
			theWebUI.resize();
			$('#fMan_showconsole').show();
	} else {$('#fMan_showconsole').hide(); plugin.onShow.call(this,id);}

}


plugin.onLangLoaded = function() {

	plugin.renameTab('FileManager', theUILang.fManager);

	$('#tab_lcont').append('<input type="button" id="fMan_showconsole" class="Button" value="Console" style="display: none;">');
	$('#fMan_showconsole').click(function() {theWebUI.fManager.makeVisbile('fMan_Console');});

	injectScript('plugins/filemanager/settings.js.php');

	theWebUI.fManager.createDialogs();
	theWebUI.fManager.Refresh();

};

plugin.onRemove = function()
{
	theWebUI.fManager.cleanactions();
	this.removePageFromTabs('FileManager');
	$('#fMan_showconsole').remove();
	$('[id^="fMan_"]').remove();
}

plugin.loadLang(true);