<?
url = _GET.url;
?><!DOCTYPE html>
<html>
<head>
<title>Image View<? print(url); ?></title>
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
</head>
<body>
  <div class="container">
    <div align="center">
      <img src="<?print(url);?>" style="width:640px;" onLoad="new viewer({image: this,frame: ['640px','480px']});" />
    </div>
    <form>
        <input type="button" class="btn btn-primary" value="戻る" onClick="history.back();">
    </form>
    mouse wheel:Zoom up/Zoom down/Scroll
  </div>
</body>
</html>