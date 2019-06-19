
var NAME = 'MyABCs';
var VER_MAJ = '0';
var VER_MIN = '4';
var VER_REL = '4';
var FAVICON = 'data/favicon.png';
var BGIMAGE = 'data/airplane-mask.png';
var PROJECT = 'https://sourceforge.net/p/myabcs';
var DOWNLOADS = PROJECT.concat("/files");
var GITHUB = 'https://github.com/AntumDeluge/myabcs';
var AUTHOR_HOME = 'http://antumdeluge.wordpress.com/';
var CHANGELOG = 'https://sourceforge.net/p/myabcs/code/ci/master/tree/changelog.txt';

var changesList = '&nbsp;- Changing to a <a href="https://opensource.org/licenses/MIT">MIT license</a>.<br>\
&nbsp;- <a href="http://libsdl.org">SDL2</a> will be used for rendering audio.<br>\
&nbsp;- <a href="https://en.wikipedia.org/wiki/Scalable_Vector_Graphics">Scalable Vector Graphics (SVG)</a> images will be used as default over <a href="https://en.wikipedia.org/wiki/Portable_Network_Graphics">PNG</a>.<br>\
&nbsp;- <a href="https://sourceforge.net/p/wxsvg">wxSVG</a> will be used for rendering SVG images.<br>\
&nbsp;- Will be updated for compatibility with <a href="http://wxwidgets.org/">wxWidgets</a> 3.x (support for 2.x will be dropped).<br>\
&nbsp;- <a href="https://cmake.org/">CMake</a> will be used for building.';

setAppInfo = function() {
	document.getElementsByClassName('appinfo')[0].innerHTML = NAME.concat(' ', VER_MAJ, '.', VER_MIN, '.', VER_REL);
}

addMenuItem = function(id, url, text) {
	var menuCell, menuText;

	menuCell = document.getElementById(id);
	menuCell.setAttribute('class', 'menucell');
	menuText = document.createElement('h5');
	menuCell.appendChild(menuText);
	menuLink = document.createElement('a');
	menuLink.setAttribute('href', url);
	menuLink.innerHTML = text;
	menuText.appendChild(menuLink);
}

setNewsFeed = function() {
	var newsTable = document.getElementsByClassName("newsfeed")[0];
	var textArea = newsTable.querySelector('td');

	var changesHeader = document.createElement('h5');
	changesHeader.innerHTML = 'Upcoming Changes:<br>'.concat(changesList);

	textArea.appendChild(changesHeader);
	textArea.appendChild(changes);
}

window.onload = function() {
	document.title = NAME;

	let head = document.querySelector('head');
	let favicon = document.createElement('link');
	favicon.setAttribute('rel', 'shortcut icon');
	favicon.setAttribute('href', FAVICON);
	head.appendChild(favicon);

	var body = document.querySelector('body');
	body.setAttribute('background', BGIMAGE);

	setAppInfo();

	addMenuItem('menu.project', PROJECT, 'Project Page');
	addMenuItem('menu.download', DOWNLOADS, 'Download Page');
	addMenuItem('menu.github', GITHUB, 'GitHub Page');
	addMenuItem('menu.author', AUTHOR_HOME.concat('/about'), 'About Author');
	addMenuItem('menu.changes', CHANGELOG, 'ChangeLog');

	setNewsFeed();
}
