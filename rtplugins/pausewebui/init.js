plugin.loadMainCSS();
plugin.loadLang(true);

debug = false;

paused = false;


window.onfocus = function()
{
	if(debug)
		log("Pausewebui: focus");
	if(!paused){
		if(debug)
			log("Pausewebui: not paused on focus");
		theWebUI.update();
	}
}

window.onblur = function()
{
	if(debug)
		log("Pausewebui: blur");
	stop();	
}

function stop(){
	if(debug)
		log("Pausewebui: stopping");
	theWebUI.timer.stop();
	if(theWebUI.updTimer){
		if(debug)
			log("Pausewebui: clearing timer");
		window.clearTimeout(theWebUI.updTimer);	
	}
}

theWebUI.togglePause = function(){
	if(debug)
		log("Pausewebui: toggeling");
	if(paused)
	{
		if(debug)
			log("Pausewebui: paused -> resuming");
		$("#webuiUpdateToggle").removeClass("resume");
		$("#webuiUpdateToggle").addClass("pause");
		$("#webuiUpdateToggle").attr("title", theUILang.pausewebuiPause);

		theWebUI.update();
		paused = false;
	}
	else
	{
		if(debug)
			log("Pausewebui: !paused -> pausing");
		$("#webuiUpdateToggle").removeClass("pause");
		$("#webuiUpdateToggle").addClass("resume");
		$("#webuiUpdateToggle").attr("title", theUILang.pausewebuiResume);

		stop();
		
		paused = true;
	}
}

theWebUI.forceUpdate = function(){
	if(debug)
		log("Pausewebui: forcing");
	stop();
	theWebUI.update();
	if(paused){
		stop();
	}
}

originalSetInterval = theWebUI.setInterval

theWebUI.setInterval = function(force){
	if(debug)
		log("Pausewebui: trying to set interval");
	if(!paused){
		if(debug)
			log("Pausewebui: setting interval");
		originalSetInterval.call(this, force);
	}
}

plugin.onLangLoaded = function()
{
	this.addButtonToToolbar("webuiUpdateToggle", theUILang.pausewebuiPause, "theWebUI.togglePause()", "help");
	this.addButtonToToolbar("webuiRefresh", theUILang.pausewebuiRefresh, "theWebUI.forceUpdate()", "help");
	$("#webuiUpdateToggle").addClass("pause");
	this.addSeparatorToToolbar("help");
}

plugin.onRemove = function()
{
	theWebUI.update();
	this.removeSeparatorFromToolbar("webuiUpdateToggle");
	this.removeButtonFromToolbar("webuiUpdateToggle");
	this.removeButtonFromToolbar("webuiRefresh");
}

	