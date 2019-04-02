<!DOCTYPE html>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1,user-scalable=no">
<meta name="format-detection" content="telephone=no" />
<meta name="apple-mobile-web-app-capable" content="yes" />  
<meta name="apple-mobile-web-app-status-bar-style" content="black-translucent">  
<meta http-equiv="Cache-Control" content="no-cache">
   <title>menu</title>
    <meta SYABAS-COMPACT=OFF> 
		<meta SYABAS-FULLSCREEN> 
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<meta http-equiv="Pragma" content="no-cache">
		<meta SYABAS-BACKGROUND="home_back.jpg">
		<meta SYABAS-COMPACT=OFF>
    <script src="src/jquery.min.js"></script>
    <script type="text/javascript" src="src/jquery.qrcode.min.js"></script>
    <script type="text/javascript" src="src/jquery.qrcode.js"></script>
    <script type="text/javascript" src="src/qrcode.js"></script>
<!--[if lt IE 7.]>
	<script defer type="text/javascript" src="pngfix.js"></script>
<![endif]-->
</head>
<body>
<?
la = getLocalAddress();
ga = loadFromFile("http://neon.cx/raspberry/ip.php");
?>
Local Address<br/>
<? print(la+":8000/"); ?><br/>
<div id="qrcodeCanvas"></div>
<br/>
<br/>
<br/>
Global Address<br/>
<? print(ga+":8000/"); ?><br/>
<div id="qrcodeCanvas2"></div>
<script>
	jQuery('#qrcodeCanvas').qrcode({
		text	: "http://<? print(la+":8000/"); ?>"
	});	
	jQuery('#qrcodeCanvas2').qrcode({
		text	: "http://<? print(ga+":8000/"); ?>"
	});	
</script>
<form>
<input type="button" value="戻る" onClick="history.back()">
</form>
</body>
</html>
