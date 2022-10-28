<?
var url = _GET.url;
var la = getLocalAddress();
url = "http://"+la + ":8000"+url;
?><!DOCTYPE html>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1,user-scalable=no">
<meta name="format-detection" content="telephone=no" />
<meta name="apple-mobile-web-app-capable" content="yes" />  
<meta name="apple-mobile-web-app-status-bar-style" content="black-translucent">  
<meta http-equiv="Cache-Control" content="no-cache">
   <title>QRCode</title>
    <meta SYABAS-COMPACT=OFF> 
		<meta SYABAS-FULLSCREEN> 
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<meta http-equiv="Pragma" content="no-cache">
    <script src="http://code.jquery.com/jquery-1.12.0.min.js"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/jquery.qrcode/1.0/jquery.qrcode.min.js"></script>
</head>
<body>
URL<br/>
<br/>
<? print(url); ?><br/><br/>
<div id="qrcodeCanvas"></div>

<script>
	jQuery('#qrcodeCanvas').qrcode({width: 96, height: 96,
		text	: "<? print(url); ?>"
	});	
</script>
</body>
</html>
