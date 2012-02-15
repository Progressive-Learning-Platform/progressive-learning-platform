// Except as noted, this content is licensed under Creative Commons
// Attribution 3.0

// Derived from the golang.org playground javascript

//(function(){

var xmlreq, output, errors, edit, compileTimer;
var changed = false;  // has the user modified the code?
var pc = 0;
var d;
var t; //timeout

function insertTabs(n) {
	if (!window.getSelection) {
		// IE will satisfy this test; no indent for them
		return;
	}
	// find the selection start and end
	var range = window.getSelection().getRangeAt(0);
	var cont  = range.startContainer;
	while (cont.nodeName != "TEXTAREA" && cont.firstChild) {
		cont = cont.firstChild;
	}
	var start = cont.selectionStart;
	var end   = cont.selectionEnd;
	// split the textarea content into two, and insert n tabs
	var v = cont.value;
	var u = v.substr(0, start);
	for (var i=0; i<n; i++) {
		u += "\t";
	}
	u += v.substr(end);
	// set revised content
	cont.value = u;
	// reset caret position after inserted tabs
	cont.selectionStart = start+n;
	cont.selectionEnd = start+n;
}

function keyHandler(e) {
	if (e.keyCode == 9) { // tab
		insertTabs(1);
		e.preventDefault();
		return false;
	}
	if (e.keyCode == 13) { // enter
		if (e.shiftKey) { // +shift
			compile(e.target);
			e.preventDefault();
			return false;
		} else {
			autoindent(e.target);
		}
	}
	return true;
}

function changeHandler(e) {
	changed = true;
}

function autoindent(el) {
	var edit = el;
	var curpos = edit.selectionStart;
	var tabs = 0;
	while (curpos > 0) {
		curpos--;
		if (edit.value[curpos] == "\t") {
			tabs++;
		} else if (tabs > 0 || edit.value[curpos] == "\n") {
			break;
		}
	}
	setTimeout(function() {
		insertTabs(tabs);
	}, 1);
}

function compile(el) {
	clearTimeout(t);
	var prog = el.value;
	$.get("compile.py", {"p":prog}, compileUpdate, "json")
	    .error(function() {
		showResult(false, "Server error.");
	    });

	showResult(true, "Compiling and running...");
	if (window['_gaq']) {
		_gaq.push(['_trackEvent', 'Playground', 'Compile', changed?"Modified":"Unmodified"]);
		changed = false;
		compileTimer = +new Date();
	}
}

function compileUpdate(data) {
	var status = 200;
	if (data.compile_errors != "") {
		showResult(false, data.compile_errors);
                status = 500;
		pc = 100;
	} else {
		pc = 0;
		d = data;
		t = setInterval("pcStep();",1000);
		showResult(true, data.output[0]);
	}

	if (window['_gaq']) {
		var ms = +new Date() - compileTimer;
		_gaq.push(['_trackEvent', 'Playground', 'Compile Result', ''+status, ms]);
	}
}

function showResult(valid, text) {
	// text has been HTML-escaped by the HTTP front end.
	text = "<pre>"+text+"</pre>";
	if (valid) {
		$(output).show().html(text);
		$(errors).hide();
	} else {
		$(errors).show().html(text);
		$(output).hide();
	}
}

function pcStep() {
	pc++;
	if (pc < 100) {
		showResult(true, d.output[pc]);
	} else {
		clearTimeout(t);
	}
}

function bind(el) {
	output = el.find('.output').css('display','block').hide();
	errors = el.find('.errors').css('display','block').hide();
	edit = $('.edit', el).unbind('keydown').bind('keydown', keyHandler);
	edit.unbind('change').bind('change', changeHandler);
	$('.run', el).unbind('click').bind('click', function() {
		compile(edit[0])
	});
	edit.focus();
}

function unsupportedBrowser() {
	var b = window.BrowserDetect
	if (b.browser == "Explorer" && b.version < 7) return true;
	return false;
}


function init() {
	var el = $('#playground');
	if (unsupportedBrowser()) {
		el.html('<p class="unsupported">'+
			'Please upgrade your web browser!<br>'+
			'It\'s worth it!</p>');
		return;
	}
	$('head').append('<link rel="stylesheet" type="text/css" href="style.css">');
	$.ajax({
		url: 'playground.html',
		success: function(html) {
			el.html(html);
			bind(el);
		}
	});
}

google.load("jquery", 1);
google.setOnLoadCallback(init);

//})();

window.BrowserDetect = {
	init: function () {
		this.browser = this.searchString(this.dataBrowser) || "An unknown browser";
		this.version = this.searchVersion(navigator.userAgent)
			|| this.searchVersion(navigator.appVersion)
			|| "an unknown version";
		this.OS = this.searchString(this.dataOS) || "an unknown OS";
	},
	searchString: function (data) {
		for (var i=0;i<data.length;i++)	{
			var dataString = data[i].string;
			var dataProp = data[i].prop;
			this.versionSearchString = data[i].versionSearch || data[i].identity;
			if (dataString) {
				if (dataString.indexOf(data[i].subString) != -1)
					return data[i].identity;
			}
			else if (dataProp)
				return data[i].identity;
		}
	},
	searchVersion: function (dataString) {
		var index = dataString.indexOf(this.versionSearchString);
		if (index == -1) return;
		return parseFloat(dataString.substring(index+this.versionSearchString.length+1));
	},
	dataBrowser: [
		{
			string: navigator.userAgent,
			subString: "Chrome",
			identity: "Chrome"
		},
		{ 	string: navigator.userAgent,
			subString: "OmniWeb",
			versionSearch: "OmniWeb/",
			identity: "OmniWeb"
		},
		{
			string: navigator.vendor,
			subString: "Apple",
			identity: "Safari",
			versionSearch: "Version"
		},
		{
			prop: window.opera,
			identity: "Opera"
		},
		{
			string: navigator.vendor,
			subString: "iCab",
			identity: "iCab"
		},
		{
			string: navigator.vendor,
			subString: "KDE",
			identity: "Konqueror"
		},
		{
			string: navigator.userAgent,
			subString: "Firefox",
			identity: "Firefox"
		},
		{
			string: navigator.vendor,
			subString: "Camino",
			identity: "Camino"
		},
		{		// for newer Netscapes (6+)
			string: navigator.userAgent,
			subString: "Netscape",
			identity: "Netscape"
		},
		{
			string: navigator.userAgent,
			subString: "MSIE",
			identity: "Explorer",
			versionSearch: "MSIE"
		},
		{
			string: navigator.userAgent,
			subString: "Gecko",
			identity: "Mozilla",
			versionSearch: "rv"
		},
		{ 		// for older Netscapes (4-)
			string: navigator.userAgent,
			subString: "Mozilla",
			identity: "Netscape",
			versionSearch: "Mozilla"
		}
	],
	dataOS : [
		{
			string: navigator.platform,
			subString: "Win",
			identity: "Windows"
		},
		{
			string: navigator.platform,
			subString: "Mac",
			identity: "Mac"
		},
		{
			   string: navigator.userAgent,
			   subString: "iPhone",
			   identity: "iPhone/iPod"
	    },
		{
			string: navigator.platform,
			subString: "Linux",
			identity: "Linux"
		}
	]

};
window.BrowserDetect.init();
