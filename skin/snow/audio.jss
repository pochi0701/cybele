<?
url = _GET.url;
if( url != undefined ){
  var name = eval(JSON.mp3id3tag(_SERVER.DOCUMENT_ROOT+url));
}
?><!DOCTYPE html>
<html>
<head>
<title>music play<? print(name.title); ?></title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1,user-scalable=no">
<meta name="format-detection" content="telephone=no" />
<meta name="apple-mobile-web-app-capable" content="yes" />  
<meta name="apple-mobile-web-app-status-bar-style" content="black-translucent">  
<meta http-equiv="Cache-Control" content="no-cache">
<!-- Bootstrap -->
<link href="http://netdna.bootstrapcdn.com/bootstrap/3.3.6/css/bootstrap.min.css" rel="stylesheet">
<script src="http://code.jquery.com/jquery-1.12.0.min.js"></script>
<script src="http://netdna.bootstrapcdn.com/bootstrap/3.3.6/js/bootstrap.min.js"></script>
<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/font-awesome/4.5.0/css/font-awesome.min.css">
<script type="text/javascript">
<!--
var root = "<? print( root ); ?>";
// -->
</script>
</head>
<body>
<div class="container">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1,user-scalable=no">
<meta name="format-detection" content="telephone=no" />
<meta name="apple-mobile-web-app-capable" content="yes" />  
<meta name="apple-mobile-web-app-status-bar-style" content="black-translucent">  
<meta http-equiv="Cache-Control" content="no-cache">
</head>
<body>
<?
//UserAgentCheck
ua      = _SERVER.HTTP_USER_AGENT;
ios     = ((ua.indexOf("iPhone")>0) || (ua.indexOf("iPod")>0) );
android = (ua.indexOf("Android")>0);

//set target movie url
if( url != undefined ){
  print("TITLE  :"+name.title+"<br>\n");
  print("ALBUM  :"+name.album+"<br>\n");
  print("ARTIST :"+name.artist+"<br>\n");
  //DIVセンタリング
  print( "<div style=\"margin:0 auto 0 auto;\">\n" );
  print( "<audio src=\""+url+"\" preload=\"auto\" controls autoplay=\"true\" >you need audio tag enabled browser.</audio>\n");
  print( "</div>" );
}
?>
<form>
<input class="btn btn-primary" type="button" value="戻る" onClick="history.back()">
</form>
</body>
</html>
