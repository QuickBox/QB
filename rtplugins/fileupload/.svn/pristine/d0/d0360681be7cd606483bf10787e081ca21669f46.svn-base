plugin.loadMainCSS();

	
theWebUI.fileup.add_options = function () {
	
		var content = $("<div>").attr("id",'fileUP_options');

		for (var i in this.services) {
				
			var service = this.services[i];
				
			if(service.hasOwnProperty('login')) {
		
				var title = this.ucfirst(i);
				var table = $('<table width="100%" border="0" cellspacing="0" cellpadding="0">').attr('id', 'fileUP_'+i);
		
				for (var x in service) {
					var field;
					switch(x) {
						case 'password':
							field = $('<input type="'+x+'" />');
							break;
						case 'account':
							continue;
							break;
						case 'email':
							continue;
							break;
						default:
							field = $('<input type="text" />');
					}
					
					
		            var row = $('<tr>').append('<td>'+theUILang.fileUP[x]+'</td>').
										append($('<td>').addClass("alr").
										append(field.addClass("TextboxLarge").attr('id', 'fileUP_'+i+'_'+x).val(service[x])));
		            table.append(row);
				}
				content.append($('<fieldset>').append('<legend>'+title+'</legend>').append(table));
		
			}
		}
		 
		plugin.attachPageToOptions(content.get(0),theUILang.fileUP.title);

		
	}
	
	
theWebUI.fileup.ucfirst =  function (string) {
    return string.charAt(0).toUpperCase() + string.slice(1);
}

theWebUI.fileup.create_dialog = function() {
		
		var content = '<div class="cont fxcaret">'+
						'<fieldset><legend>'+theUILang.fileUP.file_info+'</legend>'+
						'<div id="fileUP_filelog" style="width:550px; height:95px;"><pre></pre></div>'+
						'</fieldset>'+
						'<fieldset><legend>'+theUILang.fileUP.up_status+'</legend>'+
						'<div id="fileUP_uplog" style="width:550px; height:110px;"><pre></pre></div>'+
						'</fieldset>'+
						'<div id="fileUP_upoptions"><fieldset><legend>'+theUILang.fileUP.up_options+'</legend>'+
						'  <table width="100%" border="0" cellspacing="0" cellpadding="0" >'+
						'  <tr>'+
						'    <td>'+
						'		<div id="fileUP_description">'+theUILang.fileUP.description+'<br />'+
						'		<input type="text" name="textfield" class="TextboxLarge" />'+
						'		</div>'+
						'	</td>'+
						'   <td>'+
						'		<div id="fileUP_recipient">'+theUILang.fileUP.recipient+'<br />'+
						'		<input type="text" name="textfield" class="TextboxLarge" />'+
						'		</div>'+
						'	</td>'+
						'  </tr>'+
						'  </table>'+
						'</fieldset></div>'+
						'</div>'+
						'<div class="aright buttons-list">'+
						'<input type="button" class="fileUP_Stop Button" value="'+theUILang.fDiagStop+'" class="Button" disabled=true />'+
						'<input type="button" class="fileUP_Start Button" value="'+theUILang.fDiagStart+'" class="Button" />'+
						'<input type="button" class="Cancel Button" value="'+theUILang.fDiagClose+'"/>'+
						'</div>';
		theDialogManager.make('fileUP_Console', theUILang.fileUP.title, content, false);
		
		$('.fileUP_Start').click(function() {
			theWebUI.fileup.start_up(this);
			
		});
		
		$('.fileUP_Stop').click(function() {
			theWebUI.fileup.stop_up(this);
			
		});

}


theWebUI.fileup.has_login = function(service) {
	
	var checks = ['login', 'password'];
	
	for (var i = 0; i < checks.length; i++) {
		

			if(	this.services[service].hasOwnProperty('account') 
				&& this.services[service].hasOwnProperty(checks[i]) 
				&& ($.trim(this.services[service][checks[i]]) == '')) 
				{return false;}

	}
	return true;
	
}


theWebUI.fileup.save = function() {
		this.query('action=save&fls='+theWebUI.fManager.encode_string(this.services));
}

theWebUI.fileup.start_up = function (button) {
	
	var recipient = $.trim($('#fileUP_recipient').find('input').val());
	if((this.selectedservice == 'megaupload') && (recipient !='' ) && !this.valid_mail(recipient)) {alert(theUILang.fileUP.invalid); return false;}

	var f = function (data) {
		if(data.tmpdir) {
			this.running = true; 
			theWebUI.fileup.session = data.tmpdir;
			theWebUI.fileup.timer = setInterval(function () {theWebUI.fileup.status(); }, 1000 );
		}

	}
	$(button).attr('disabled', true);
	$('.fileUP_Stop').attr('disabled', false);
	
	$('#fileUP_Console .buttons-list').css( "background", "transparent url(./plugins/create/images/ajax-loader.gif) no-repeat 15px 2px" );


	var description = $.trim($('#fileUP_description').find('input').val());
	var extra = (description !='' ) ? '&mode='+description : '';
	extra += (recipient !='' ) ? '&to='+recipient: '';

	this.query('action=upload&target='+this.selectedservice+extra+'&file='+encodeURIComponent(this.selectedfile), f);

}

theWebUI.fileup.stop_up = function (button) {
	
	var f = function (data) {
		var fl = $('#fileUP_filelog pre').append("<strong>"+theUILang.fileUP.up_stop+"</strong>\n<hr />\n");
	}
	$(button).attr('disabled', true);
	var s = this.session;
	theWebUI.fileup.reset();
	this.query('action=stop&target='+s, f);

}


theWebUI.fileup.status = function () {
	
	var f = function (data) {
			var cw = $('#fileUP_uplog');
			var fl = $('#fileUP_filelog');
			var fl_con = fl.find('pre');

			cw.find('pre').html(data.status);
			cw[0].scrollTop = cw[0].scrollHeight;

			if(data.errcode > 0){
				theWebUI.fileup.reset();
				fl_con.append("<strong>"+theUILang.fileUP.failed+"</strong>\n<hr />\n")
			}
			
			if(data.done)  {
					theWebUI.fileup.reset();
					fl_con.append('<strong>'+theUILang.fileUP.fileurl+' </strong><a href="'+data.dllink+'" target="_blank">'+data.dllink+"</a>\n");
					if($type(data.dellink)) {fl_con.append('<strong>'+theUILang.fileUP.deleteurl+' </strong><a href="'+data.dellink+'" target="_blank">'+data.dellink+"</a>\n");}
					fl_con.append("<hr />\n")
					cw.find('pre').append("\nDone!");
			}
			
			fl[0].scrollTop = fl[0].scrollHeight;


	}
	
	
	this.query('action=status&target='+encodeURIComponent(this.session), f);	
}

theWebUI.fileup.selectedfile = "";
theWebUI.fileup.selectedservice = "";
theWebUI.fileup.running = false;
theWebUI.fileup.timer = 0;
theWebUI.fileup.session = '';

theWebUI.fileup.prepare = function(provider) {
	
		if(!this.running) {
			var opt = $('#fileUP_upoptions');
			var desc = $('#fileUP_description').hide();
			var email = $('#fileUP_recipient').hide();

			$('#fileUP_recipient').find('input').val('');
			$('#fileUP_description').find('input').val('');


if(	this.services[provider].hasOwnProperty('email') ) {

	opt.show();
	email.show();

}
		


			$('#fileUP_filelog pre').append('<strong>'+theUILang.fileUP.file+' </strong>'+this.selectedfile+"\n");
			this.selectedservice = provider;
			$('.fileUP_Start').attr('disabled', false);
			$('#fileUP_uplog pre').empty().html(theUILang.fileUP.preup);
		}

		theWebUI.fManager.makeVisbile('fileUP_Console');
}

theWebUI.fileup.reset = function() {
	$('#fileUP_Console .buttons-list').css( "background", "none" );
	$('.fileUP_Stop').attr('disabled', true);
	clearInterval(this.timer);
	this.running = false;
	this.session = '';
	
	
}

theWebUI.fileup.query = function(action, complete) {


			$.ajax({
  				type: 'POST',
   				url: 'plugins/fileupload/fup.php',
				timeout: theWebUI.settings["webui.reqtimeout"],
			    async : true,
			    cache: false,
				data: action,
   				dataType: "json",

				error: function(XMLHttpRequest, textStatus, errorThrown) {
					log('FILE UPLOAD: error - STATUS:'+textStatus+' MSG: '+XMLHttpRequest.responseText);
					theWebUI.fileup.reset();			
				},

				success: function(data, textStatus) {
								if($type(complete)) {complete(data);}
						}
 		});


}

theWebUI.fileup.valid_mail = function(str) {
		console.log(str);
		return str.match(/^([\w-]+(?:\.[\w-]+)*)\@((?:[\w-]+\.)*\w[\w-]{0,66})\.([a-z]{2,6}(?:\.[a-z]{2})?)$|(\[?(\d{1,3}\.){3}\d{1,3}\]?)$/i);
}


plugin.addAndShowSettings = theWebUI.addAndShowSettings;
theWebUI.addAndShowSettings = function(arg) {
		if(plugin.enabled) {
			for (var i in theWebUI.fileup.services) {
				
				var obj = theWebUI.fileup.services[i];
				var fields = $('#fileUP_'+i).find('input, select');
				
				if(obj && (fields.length > 0)) {
					for (var x in obj) {$('#fileUP_'+i+'_'+x).val(obj[x]);}
				}
			}
		}
	plugin.addAndShowSettings.call(theWebUI,arg);
}



plugin.setSettings = theWebUI.setSettings;
theWebUI.setSettings = function() {


		if(plugin.enabled) {
			var needsave = false;

			for (var i in theWebUI.fileup.services) {
				
				var obj = theWebUI.fileup.services[i];
				var fields = $('#fileUP_'+i).find('input, select');
				
				if(obj && (fields.length > 0)) {
					for (var x in obj) {
						var ele = $('#fileUP_'+i+'_'+x).val();
						if(ele && (ele != obj[x])) {
							if(x == 'email' && !theWebUI.fileup.valid_mail(ele)) {continue;}
							theWebUI.fileup.services[i][x] = ele; 
							needsave = true;
						}
						
					}
				}
			}

			if(needsave) {theWebUI.fileup.save();}
		}

		plugin.setSettings.call(this);
}


plugin.flmMenu = theWebUI.fManager.flmSelect;
theWebUI.fManager.flmSelect = function( e, id ) {

		plugin.flmMenu.call(this, e, id);
		if(plugin.enabled) {
			var el = theContextMenu.get( theUILang.fMediaI);
			if(el) {
				var item = id.split('_flm_')[1];
				var services = []
				var flup = theWebUI.fileup;
				
				for(var i in flup.services) {
					var ff = flup.has_login(i) ? (function(val) { flup.selectedfile = theWebUI.fManager.homedir+theWebUI.fManager.curpath+item; var f = function() {flup.prepare(val);}; return f; })(i) : null;
					services.push([i, ff]);
				}
				
				theContextMenu.add( el, (!theWebUI.fManager.isDir(item) && !(theWebUI.getTable("flm").selCount > 1)) ? [CMENU_CHILD, theUILang.fileUP.upto, services] : null);
			}
		}
		
}

plugin.onLangLoaded = function() {
	
		theWebUI.fileup.add_options();
		theWebUI.fileup.create_dialog();
}

plugin.loadLang(true);