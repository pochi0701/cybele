<!DOCTYPE html>
<html lang="ja">
<head>
  <meta charset="utf-8">
  <title>menu</title>
  <meta http-equiv="Pragma" content="no-cache">
  <meta http-equiv="Cache-Control" content="no-cache">
  <meta http-equiv="Expires" content="0">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link href="http://netdna.bootstrapcdn.com/bootstrap/3.3.6/css/bootstrap.min.css" rel="stylesheet">
  <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/font-awesome/4.5.0/css/font-awesome.min.css">
  <script src="http://code.jquery.com/jquery-1.12.0.min.js"></script>
  <script src="http://netdna.bootstrapcdn.com/bootstrap/3.3.6/js/bootstrap.min.js"></script>
  <script type="text/javascript" src="src/jquery.qrcode.js"></script>
  <script type="text/javascript" src="src/qrcode.js"></script>
  </head>
<body background="images/home_back.jpg" bgcolor="#999999" FOCUSCOLOR="#550033" FOCUSTEXT="#FFFFFF" style="padding-top:53px;">
<div class="text-center">
<img src="images/cybele.png" width="304" height="79">
</div>
<div class="container">
  <!-- 1.ナビゲーションバーの設定 -->
  <nav class="navbar navbar-inverse navbar-fixed-top">
    <!-- 2.ヘッダ情報 -->
    <div class="navbar-header">
      <button type="button" class="navbar-toggle" data-toggle="collapse" data-target="#nav-menu-1">
        <span class="sr-only">Toggle navigation</span>
        <span class="icon-bar"></span>
        <span class="icon-bar"></span>
        <span class="icon-bar"></span>
      </button>
      <a class="navbar-brand" href="#">Cybele</a>
    </div>
    <!-- 3.リストの配置 -->
    <div class="collapse navbar-collapse" id="nav-menu-1">
      <ul class="nav navbar-nav navbar-right">
        <li ><a href="/" target="_top">Home</a></li>
      </ul>
    </div>
  </nav>
</div> 
<div class="container">
  <div class="row">
    <div class="col-xs-offset-1 col-sm-offset-2 col-md-offset-3">
      <table>
        <tr>
          <td><a href="/system/edit/"><img src="images/edit.png" width="96" height="96" border="0"></a></td>
          <td><a href="user/?"><img src="images/user.png" width="96" height="96" border="0"></a></td>
          <td><a href="public/?"><img src="images/public.png" width="96" height="96" border="0"></a></td>
          <td><a href="/system/tools/"><img src="images/tools.png" width="96" height="96" border="0"></a></td>
          <td><a href="/system/edit/help.html"><img src="images/help.png" width="96" height="96" border="0"></a></td>
          <td><a href="http://<? print(getLocalAddress()); ?>:8000/"><div id="qrcodeCanvas1"></div></a></td>
        </tr>
      </table>
    </div>
  </div>
  <div>
    <div class="text-right"><img src="images/birdland_logo.png" width="146" height="54"> </div>
  </div>
</div>
<script>
	jQuery('#qrcodeCanvas1').qrcode({text:"http://<? print(getLocalAddress()); ?>:8000/"});	
</script>

</body>
</html>
