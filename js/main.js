
var VER_MAJ = '0';
var VER_MIN = '4';
var VER_REL = '6';
var PROJ_SF = 'https://sourceforge.net/p/myabcs';
var PROJ_GH = 'https://github.com/AntumDeluge/myabcs';
var DOWNLOADS = PROJ_SF.concat("/files");
var AUTHOR_HOME = 'http://antumdeluge.wordpress.com/';
var CHANGELOG = 'https://sourceforge.net/p/myabcs/code/ci/master/tree/CHANGES.txt';

var changesList = '&nbsp;- <a href="https://cmake.org/">CMake</a> will be used for building.';

setAppInfo = function() {
	document.getElementsByClassName('appinfo')[0].innerHTML = NAME.concat(' ', VER_MAJ, '.', VER_MIN, '.', VER_REL);
}

addMenuItem = function(id, url, text) {
	var menuCell, menuText;

	var tableMenu = document.getElementsByClassName('menu')[0].querySelector('tr');

	menuCell = document.createElement('td');
	menuCell.setAttribute('class', 'menucell');
	menuCell.setAttribute('id', id);
	menuText = document.createElement('h5');
	menuCell.appendChild(menuText);
	menuLink = document.createElement('a');
	menuLink.setAttribute('href', url);
	menuLink.innerHTML = text;
	menuText.appendChild(menuLink);

	tableMenu.appendChild(menuCell);
}

setNewsFeed = function() {
	var newsTable = document.getElementsByClassName("newsfeed")[0];
	var textArea = newsTable.querySelector('td');

	var changesHeader = document.createElement('h5');
	changesHeader.setAttribute('style', 'font-size:12pt;');
	changesHeader.innerHTML = 'Upcoming Changes:<br>'.concat(changesList);

	textArea.appendChild(changesHeader);
	textArea.appendChild(changes);
}

window.onload = function() {
	setAppInfo();

	addMenuItem('menu.project', PROJ_SF, 'SourceForge Project');
	addMenuItem('menu.github', PROJ_GH, 'GitHub Project');
	addMenuItem('menu.download', DOWNLOADS, 'Download Page');
	addMenuItem('menu.author', AUTHOR_HOME.concat('/about'), 'About Author');
	addMenuItem('menu.changes', CHANGELOG, 'ChangeLog');

	setNewsFeed();
}
