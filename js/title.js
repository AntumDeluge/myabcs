
var NAME = 'MyABCs';
var FAVICON = 'data/favicon.png';


// set page title
document.title = NAME;

// set favicon image
function setFavicon(img) {
	let linkFavicon = document.createElement('link');
	linkFavicon.type = 'image/png';
	linkFavicon.rel = 'shortcut icon';
	linkFavicon.href = img;

	let tagHead = document.querySelector('head');
	tagHead.appendChild(linkFavicon);
}

setFavicon(FAVICON);
