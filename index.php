<?php
$NAME = 'MyABCs';
$VER_MAJ = '0';
$VER_MID = '4';
$VER_MIN = '4';
$PROJECT = 'https://sourceforge.net/p/myabcs';
$DOWNLOADS = ($PROJECT . "/files");
$AUTHOR = 'Jordan Irwin (AntumDeluge)';
$AUTHOR_HOME = 'http://antumdeluge.wordpress.com/';
$ICON = 'data/icon.png';
$BGIMAGE = 'data/airplane-mask.png';
$FAVICON = 'data/favicon.png';
$GITHUB = 'https://github.com/AntumDeluge/myabcs';
$NEWSFEED = '<table align="center">
  <tr>
    <td>
      <h5>
        Upcoming Changes:<br>
        - Changing to a BSD-style license.<br>
        - <a href="http://libsdl.org">SDL2</a> will be used for rendering audio.<br>
        - <a href="https://sourceforge.net/p/wxsvg">wxSvg</a> will be used for rendering images.<br>
        - Will be ported to <a href="http://wxwidgets.org/">wxWidgets 3.x</a>.<br>
        - More universal build method.
      </h5>
    </td>
  </tr>
</table>';

print("<html>
<head>
<title>$NAME</title>

<link rel=\"shortcut icon\" href=\"$FAVICON\" />

</head>

<body background=\"$BGIMAGE\">
<br>
<div style=\"text-align:center\"><img border=\"0\" src=\"$ICON\" /></div>
<h2 style=\"text-align:center\">$NAME $VER_MAJ.$VER_MID.$VER_MIN</h2>
<h5 style=\"text-align:center\">By $AUTHOR</h5>
<table style=\"margin-left:auto; margin-right:auto;\" cellpadding=\"10\">
  <tr>
    <td><h5 style=\"text-align:center\"><a href=\"$PROJECT\">Project Page</a></h5></td>
    <td><h5 style=\"text-align:center\"><a href=\"$DOWNLOADS\">Download Page</a></h5></td>
    <td><h5 style=\"text-align:center\"><a href=\"$GITHUB\">GitHub Page</a></h5></td>
    <td><h5 style=\"text-align:center\"><a href=\"$AUTHOR_HOME/about\">About Author</a></h5></td>
  </tr>
</table>
$NEWSFEED

</body>
</html>");
?>