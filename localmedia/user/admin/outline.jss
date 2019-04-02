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
  <script type="text/javascript">
  var root;
  var seq;
  var crnt;
  //新規ノード作成。ルートの下
  function newNode()
  {
      document.getElementById("InputText").value     = "";
      document.getElementById("InputTextarea").value = "";
      document.getElementById("hiddenValue").value   = seq;
      crnt = seq;
      reWrite();
  }
  //ノード追加。カレントの下
  function insertNode()
  {
  }
  //
  function registNode(mytitle,mycontents,mynum)
  {
      if( mytitle == null || mytitle.length == 0 )
      {
          alert("タイトルは必須です");
          return null;
      }
      var temp = getNode(root,mynum);
      //すでに番号が存在する
      if( temp != null ){
          temp.title = mytitle;
          temp.value = mycontents;
      }else{
          addChild(root,new Node(mytitle,mycontents));
      }
      reWrite();
  }
  //Ｔｒｅｅ再描画  
  function reWrite()
  {
      document.getElementById("tree").innerHTML=makeTree(root,0);
  }
  function init()
  {
      //新規の場合。ロードの場合は適宜設定
      //編集中ツリーのインデックス初期値
      seq = 0;
      crnt = 0;
      root = new Node("プロジェクト名","プロジェクトの内容");
      reWrite();
  }
  // ツリー構造用データ
  // title    : ノードタイトル
  // value    : ノードが保持するデータ
  // children : 子ノード
  // now      : 作成日付
  function Node(title, value) {
      this.title    = title;
      this.value    = value;
      this.num      = seq;
      this.children = null;
      this.prnt     = null;
      this.now      = new Date();
      seq += 1;
      document.getElementById("hiddenValue").value = seq;
      crnt = seq;
  }
  //targetにnodeを子として追加
  function addChild(target,node)
  {
      if( target.children ){
          target.children.push(node);
      }else{
          target.children = [node];
      }
      node.prnt = target;
      return node;
  }
  //指定番号のノード取得
  function getNode(target,number)
  {
    if( target.num == number ){
        return target;
    }else if (target.children) {
        for (var i = 0; i < target.children.length; i++) {
            var ret = getNode(target.children[i], number);
            if( ret != null ){
                return ret;
            }
        }
    } 
    return null;
  }
  function viewData(num)
  {
      var temp = getNode(root,num);
      //番号が存在する
      if( temp != null ){
          document.getElementById("InputText").value     = temp.title;
          document.getElementById("InputTextarea").value = temp.value;
          document.getElementById("hiddenValue").value   = temp.num;
          crnt = temp.num;
          reWrite();
      }
  }
  //構造表示
  function makeTree(target, indent)
  {
    if( target== null){
        return "";
    }
    var str = "";
    // インデントを表示
    for (var i = 0; i < indent; i++) {
        str += "-";
    }
    if( target.num == crnt ){
        str = str + "<a href=\"#\" onClick=\"viewData("+target.num+");\" style=\"color: #ff0000; background-color: transparent\">"+target.title + "</a><br>\n";
    }else{
        str = str + "<a href=\"#\" onClick=\"viewData("+target.num+");\">"+target.title + "</a><br>\n";
    }
    //str = str +target.title + "<br>\n";
    // 子ノードがあれば子ノードを表示
    if (target.children) {
        for (var i = 0; i < target.children.length; i++) {
            str += makeTree(target.children[i], indent + 1);
        }
    }
   return str;
  }
  //JSON
  function test()
  {
      alert("aaa");
      var temp2 = root;
      var temp = JSON.stringify(temp2);
      alert(temp);
  }
  </script>
  </head>
<body style="padding-top:53px;" onload="init();">
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
          <a class="navbar-brand" href="#">OUTLINE</a>
        </div>
        <!-- 3.リストの配置 -->
        <div class="collapse navbar-collapse" id="nav-menu-1">
          <ul class="nav navbar-nav">

            <li class="dropdown">
              <a href="#" class="dropdown-toggle" data-toggle="dropdown">
                <span class="link-menu">ノード<b class="caret"></b></span>
              </a>
              <ul class="dropdown-menu">
                <li><a href="#" onClick="createDir();">ノード↑</a></li>
                <li><a href="#" onClick="createFile();">ノード↓</a></li>
                <li><a href="#" onClick="myreload(0);">ノード>></a></li>
                <li><a href="#" onClick="myreload(1);">ノード<<</a></li>
                <li class="divider"></li>
                <li><a href="#" onClick="newNode();">新規ノード</a></li>
                <li class="divider"></li>
                <li><a href="#" target="_top">ノード削除</a></li>
              </ul>
            </li>

            <li class="dropdown">
              <a href="#" class="dropdown-toggle" data-toggle="dropdown">
                <span class="link-menu">ユーティリティ<b class="caret"></b></span>
              </a>
              <ul class="dropdown-menu">
                <li><a href="wiki.jss" target="wiki.jss">保存</a></li>
                <li><a href="env.jss" target="env.jss">環境変数</a></li>
                <li><a href="#" onClick="sea();">検索</a></li>
                <li><a href="#" onClick="memo();return false;">メモ</a></li>
              </ul>
            </li>
            <li ><a href="#">temp</a></li>
          </ul>
        </div>
      </nav>
    </div> 

  <div class="container">
<div class="row">
  <div class="col-xs-12 col-sm-2 col-md-2">
    <div id="tree"></div>
  </div>
  <div class="col-xs-12 col-sm-10 col-md-10">
  
  
    <form class="form-horizontal" name="myform">
	<div class="form-group">
		<label  class="control-label" for="InputText">text</label>
		<div>
			<input type="text" class="form-control" id="InputText" placeholder="タイトル">
		</div>
	</div>
    <div class="form-group">
        <label class="control-label" for="InputTextarea">Context</label>
        <textarea placeholder="コンテンツ" rows="20" class="form-control" id="InputTextarea"></textarea>
        <input type="hidden" id="hiddenValue" value="">
    </div> 
    </form>
  	<button type="button" class="btn btn-primary" onClick='registNode(document.getElementById("InputText").value,document.getElementById("InputTextarea").value,document.getElementById("hiddenValue").value);'>登録</button>
  	<button type="button" class="btn btn-primary" onClick='test();'>test</button>
  
  
</div>
</div>
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
