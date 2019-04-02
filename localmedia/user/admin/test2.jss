<!DOCTYPE html>
<html lang="ja">
<head>
  <meta charset="utf-8">
  <title>アウトラインプロセッサ2</title>
  <meta http-equiv="Pragma" content="no-cache">
  <meta http-equiv="Cache-Control" content="no-cache">
  <meta http-equiv="Expires" content="0">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link href="http://netdna.bootstrapcdn.com/bootstrap/3.3.6/css/bootstrap.min.css" rel="stylesheet">
  <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/font-awesome/4.5.0/css/font-awesome.min.css">
  <style>
    @media (min-width:767px){
      .dropdown:hover > .dropdown-menu{
        display: block;
      }
    }
    .container {width: auto};
  </style>
  <script src="http://code.jquery.com/jquery-1.12.0.min.js"></script>
  <script src="http://netdna.bootstrapcdn.com/bootstrap/3.3.6/js/bootstrap.min.js"></script>
  <script>
  function viewNode(title,contents)
  {
      var e = document.getElementById ('InputText');
      e.value = title;
      e = document.getElementById ('InputTextarea');
      e.value = contents;
  }
  </script>
  </head>
<body style="padding-top:53px;">
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
          <a class="navbar-brand" href="#">初期メニュー</a>
        </div>
        <!-- 3.リストの配置 -->
        <div class="collapse navbar-collapse" id="nav-menu-1">
          <ul class="nav navbar-nav">
             <li ><a href="index.php?signin=1">ログイン</a></li>
          </ul>
        </div>
      </nav>
    </div>
  <div class="container">
<div class="row">
  <div class="col-xs-12 col-sm-2 col-md-2">
あうあうあー  </div>
  <div class="col-xs-12 col-sm-10 col-md-10">
  
  
    <form class="form-horizontal">
	<div class="form-group">
		<label class="control-label" for="InputText">text</label>
		<div>
			<input type="text" class="form-control" id="InputText" placeholder="タイトル">
		</div>
	</div>
    <div class="form-group">
        <label class="control-label" for="InputTextarea">Context</label>
        <textarea placeholder="コンテンツ" rows="20" class="form-control" id="InputTextarea"></textarea>
    </div> 
    </form>
  	<button type="button" class="btn btn-primary" onClick="viewNode('あああ','いいい');">登録</button>
  
  
</div>
</div>
<div class="text-center">
<ul class="pagination">
<li class="disabled"><a href="index.php?&page=0">前</a></li>
<li class="disabled"><a href="index.php?&page=0">次</a></li>
</ul>
</div>
    </div>
    <div class="text-center">
<a href="index.php?feed=atom"><i class="fa fa-rss fa-3x"></i></a>
</div>
    <!-- ===== copyright ===== -->
    <div class="copyright">
      <p class="text-center">
        Copyright(c) 2015 <a href="http://www.birdland.co.jp">Birdland Ltd.</a> All Rights Reserved.
      </p>
    </div>
    <!-- jQuery (necessary for Bootstrap's JavaScript plugins) -->
    <!-- Include all compiled plugins (below), or include individual files as needed -->
    <script type="text/javascript">
    jQuery(document).ready(function ($) {
        $('.dropdown-toggle').click(function(e) {
             // 要素で親メニューリンクとドロップダウンメニュー表示を切り分ける
             if ($(e.target).hasClass('link-menu')) {
                 var location = $(this).attr('href');
                 window.location.href = location;
                 return false;
             }
             return true;
        });
    });
    </script>
  </body>
</html>
