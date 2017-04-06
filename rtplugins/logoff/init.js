plugin.loadMainCSS();
plugin.loadLang();

plugin.onLangLoaded = function()
{
    var before = (theWebUI.systemInfo.rTorrent.started ? "add" : "settings");
    this.addButtonToToolbar("logoff", theUILang.logoff + " (" + plugin.me + ")", "theDialogManager.show('logoffDlg')", before);
    this.addSeparatorToToolbar(before);
    var multi = false;

    if (plugin.allowSwitch) {
        var options = "";
        for (i in plugin.users)
            options += "<option value=\"" + plugin.users[i] + "\">" + plugin.users[i] + "</option>";

        var switchUser = "";
        if (options != "") {
            multi = true;
            switchUser = ""+
            "<div>"+
                "<label for=\"login.username\">" + theUILang.logoffUsername + ":</label> "+
                "<select id=\"login.username\">"+
                    options+
                "</select>"+
            "</div>"+
            "<div>"+
                "<label for=\"login.password\">" + theUILang.logoffPassword + ":</label> "+
                "<input type=\"password\" id=\"login.password\" class=\"Textbox\" /> <span id=\"logoffPassEmpty\"></span>"+
            "</div>";
        }
    }

    theDialogManager.make("logoffDlg", theUILang.logoff,
        "<div id=\"logoffDlg-content\">"+
            (multi ? theUILang.logoffSwitchPrompt + switchUser : theUILang.logoffPrompt)+
        "</div>"+
        "<div id=\"logoffDlg-buttons\" class=\"aright buttons-list\">"+
            (multi ? "<input type=\"button\" class=\"Button\" value=\"" + theUILang.logoffSwitch + "\" id=\"logoffSwitch\">" : "")+
            "<input type=\"button\" class=\"Button\" value=\"" + theUILang.logoff + "\" id=\"logoffComplete\">"+
            "<input type=\"button\" class=\"Button\" value=\"" + theUILang.Cancel + "\" id=\"logoffCancel\" onclick=\"theDialogManager.hide('logoffDlg');\">"+
        "</div>",
    true);

    if (multi) {
        $($$("login.password")).keydown(function(event)
        {
            if ((event.which && event.which == 13) || (event.keyCode && event.keyCode == 13))
                $("#logoffSwitch").trigger("click");
        });

        $("#logoffSwitch").click(function()
        {
            if ($($$("login.password")).val() == "") {
                $("#logoffPassEmpty").html(theUILang.logoffEmpty);
                return(false);
            }
            $("#logoffPassEmpty").html("");
            $("#logoffSwitch").attr("disabled", "disabled");

            try {
                if (browser.isFirefox) {
                    var xmlhttp = new XMLHttpRequest();
                    xmlhttp.open("POST", document.location.href, true, $($$("login.username")).val(), $($$("login.password")).val());
                    xmlhttp.onreadystatechange = function() { if (this.readyState == 4) theWebUI.reload(); };
                    xmlhttp.send(null);
                } else {
                    var xmlhttp = new XMLHttpRequest();
                    xmlhttp.open("POST", this.action, true, $($$("login.username")).val(), $($$("login.password")).val());
                    xmlhttp.onreadystatechange = function() { if (this.readyState == 4) { document.location = this.action; theWebUI.reload(); } };
                    xmlhttp.send(null);
                }
            } catch (e) { alert("Error: " + e); }
        });
    }

    $("#logoffComplete").click(function()
    {
        $("#logoffComplete").attr("disabled", "disabled");
        try {
            if (browser.isIE7up) {
                document.execCommand("ClearAuthenticationCache");
                document.location = plugin.logoffURL;
            } else if (browser.isFirefox) {
                var xmlhttp = new XMLHttpRequest();
                xmlhttp.open("POST", document.location.href, true, "logoff", "logoff");
                xmlhttp.onreadystatechange = function() { if (this.readyState == 4) document.location = plugin.logoffURL; };
                xmlhttp.send(null);
                if (browser.isFirefox3x)
                    xmlhttp.abort();
            } else {
                var xmlhttp = new XMLHttpRequest();
                xmlhttp.open("POST", this.action, true, "logoff", "logoff");
                xmlhttp.onreadystatechange = function() { if (this.readyState == 4) document.location = plugin.logoffURL; };
                xmlhttp.send(null);
                window.logoffxmlhttp = xmlhttp;
                window.setTimeout("logoffxmlhttp.abort()", plugin.abortMs);
            }
        } catch (e) { alert("Error: " + e); }
    });
}

plugin.onRemove = function()
{
    theDialogManager.hide("logoffDlg");
    this.removeSeparatorFromToolbar(theWebUI.systemInfo.rTorrent.started ? "add" : "settings");
    this.removeButtonFromToolbar("logoff");
}
