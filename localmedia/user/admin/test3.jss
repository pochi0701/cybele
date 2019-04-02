<?
//サイズ変換
function size_num_read(size) {
    var bytes = ["B","KB","MB","GB","TB"];
    for( var i=0 ; i < 5 ; i++) {
        if(size > 1024){
            size = size / 1024;
        }else{
            break;
        }
    }
    return (round(size*100)/100)+bytes[i];
}
//変数取得
var base    = _SERVER.DOCUMENT_ROOT;
var root    = _GET.root;
var newDir  = _GET.dirName;
var newFile = _GET.FileName;
var del     = _GET.del;
var renf    = _GET.renf;
var rent    = _GET.rent;
var vaf     = _GET.vaf;
var search  = _GET.search;
//ユーザ限定処理
//初回の処理
if( root.length==0){
    root = _SERVER.DOCUMENT_ROOT;
}
//ファイル削除
if( del.length>0 ){
    if( file_exists(del) ){
        unlink(del);
    }else if ( dir_exists(del) ){
        rmdir(del);
    }
    //リネーム
}else if( renf.length>0 && rent.length>0 ){
    print( "["+file_exists(renf)+"]["+file_exists(rent)+"]\n" );
    if( file_exists(renf) && (! file_exists(rent))){
        rename(renf,rent);
    }else if( dir_exists(renf) && (! dir_exists(rent))){
        rename(renf,rent);
    }else if ( file_exists(rent) ){
        print( "すでに同名のファイルが存在します" );
    }
    //パス名取得、作成
}else if (dir_exists(root) && newDir.length>0 ) {
    var path = root+"/"+newDir;
    if (!file_exists(path)){
        mkdir(path);
    }
    root = path;
    //ファイル名取得、作成
}else if (dir_exists(root) && newFile.length>0 ) {
    var path = root+"/"+newFile;
    if (!file_exists(path)){
        touch(path);
    }
}
//右端の/をなくす
while( root[root.length-1] == "/" ){
    root = root.substring(0,root.length-1);
}
var sf = root.substring(base.length,root.length);
if( sf == ""){
   sf = "/";
}
var me=_SERVER.SCRIPT_NAME;
?>
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
<!--
var root  = "<? print( root ); ?>";
var rootn = "<? print( encodeURI(root) ); ?>";
var rootu = "<? print( root.nkfconv("Sw") ); ?>";
var base  = "<? print( base ); ?>";
var me    = "<? print( me ); ?>";
function viewCode(path) {
    var root = "<? print(_SERVER.DOCUMENT_ROOT); ?>";
    var pos = path.indexOf(path,root);
    path = "http://<?print(_SERVER.HTTP_HOST);?>"+path.substring(root.length,path.length);
    myWin = window.open(path,path);
}
function editCode(url) {
    if( parent.right.pushCode){
        parent.right.pushCode(0);
    }
    parent.right.location.href=url;
    return false;
}
function viewDataImage(path) {
    path = "http://<? print( _SERVER.HTTP_HOST );?>"+path+"?action=ImageView.jss";
    myWin = window.open(path);
}
function viewDataSound(path) {
    path = "http://<? print( _SERVER.HTTP_HOST );?>"+path+"?action=audio.jss";
    myWin = window.open(path);
}
function viewDataVideo(path) {
    path = "http://<? print( _SERVER.HTTP_HOST );?>"+path+"?action=preview.jss";
    myWin = window.open(path);
}
function deleteFile(url,file) {
    if( window.confirm(file+"を削除します。よろしいですか？") ){
        location.href=url+"&root="+rootn;
    }
    return false;
}
function renameFile(url,file) {
    var msg = window.prompt("変更するファイル名を入力してください",file);
    if( msg != null && window.confirm(file+"を"+msg+"に変更します。よろしいですか？") ){
        location.href=url+"&rent="+rootn+encodeURI("/"+msg);
    }
    return false;
}
function createDir() {
    var msg = window.prompt("作成するフォルダ名を入力してください");
    if( msg != null && window.confirm("フォルダ"+msg+"を"+rootu+"に作成します。よろしいですか？") ){
        location.href=me+"?dirName="+msg+"&root="+rootn;
    }
    return false;
}
function createFile() {
    var msg = window.prompt("作成するファイル名を入力してください");
    if( msg != null && window.confirm("ファイル"+msg+"を"+rootu+"に作成します。よろしいですか？") ){
        location.href=me+"?FileName="+msg+"&root="+rootn;
    }
    return false;
}
function myreload(num) {
    location.href=me+"?root="+rootn+"&vaf="+num;
}
function dl() {
    var msg1 = window.prompt("ダウンロードするＵＲＬを入力してください","");
    var msg2 = "./dl.jss?target="+rootn+"&dl="+encodeURI(msg1);
    if( msg1 != null){
        if( window.confirm(msg1+"をダウンロードします。よろしいですか？") ){
            parent.tmp.location.href=msg2;
        }
    }
}
function todo() {
    var Window_Option='titlebar=no,menubar=no,toolbar=no,location=no,scrollbars=no,status=no,resizable=no,width=500,height=200';
    var W1;
    W1=window.open("","mwin",Window_Option);
    W1.location.href='/main/todo.jss';//NN対策
}
function expt() {
    //zipを作って
    var W1;
    if( window.confirm(rootu+"を圧縮してダウンロードします。よろしいですか？") ){
        location.href= "zip.jss?path="+rootn;
    }
    return false;
}
function view() {
    var W1;
        W1=window.open("","_blank");
    W1.location.href= "view.jss?path="+rootn;
    //parent.right.location.href="./view.jss?path="+rootn;
    return false;
}

function memo() {
    var W1;
    W1=window.open("","_blank");
    W1.location.href= "data:text/html, <html contenteditable>";
    return false;
}

function logout() {
    //$_SESSION = array(); //すべてのセッション変数を初期化
    //session_destroy(); //セッションを破棄
    //top.window.close();
    top.location.href="login.jss?mode=LOGOUT";
    return true;
}
function sea() {
    var msg1 = window.prompt("検索する文字列を入力してください","");
    if( msg1 != null){
        location.href=me+"?root="+rootn+"&vaf=1&search="+msg1;
    }
}


  function viewNode(title,contents)
  {
      var e = document.getElementById ('InputText');
      e.value = title;
      e = document.getElementById ('InputTextarea');
      e.value = contents;
  }
// -->
</script>
  <style type="text/css" media="screen">

    #editor {
        bottom: 0;
        left: 0;
        right: 0;
    }
  </style>
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
          <ul class="nav navbar-nav">
             <li ><a href="index.php?signin=1">ログイン</a></li>
          </ul>
        </div>
      </nav>
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
          <a class="navbar-brand" href="#">WebEdit</a>
        </div>
        <!-- 3.リストの配置 -->
        <div class="collapse navbar-collapse" id="nav-menu-1">
          <ul class="nav navbar-nav">

            <li class="dropdown">
              <a href="#" class="dropdown-toggle" data-toggle="dropdown">
                <span class="link-menu">ファイル<b class="caret"></b></span>
              </a>
              <ul class="dropdown-menu">
                <li><a href="#" onClick="createDir();">フォルダ作成</a></li>
                <li><a href="#" onClick="createFile();">ファイル作成</a></li>
                <li><a href="#" onClick="myreload(0);">再読込</a></li>
                <li><a href="#" onClick="myreload(1);">全表示</a></li>
                <li class="divider"></li>
                <li><a href="#" onClick="logout();">ログアウト</a></li>
                <li><a href="/" target="_top">ホーム</a></li>
              </ul>
            </li>

            <li class="dropdown">
              <a href="#" class="dropdown-toggle" data-toggle="dropdown">
                <span class="link-menu">ユーティリティ<b class="caret"></b></span>
              </a>
              <ul class="dropdown-menu">
                <li><a href="wiki.jss" target="wiki.jss">Wiki</a></li>
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
  [<?print(sf);?>]
  <div id="tree">
  <table>
<?
  //親ディレクトリ
  var filePath = dirname(root);
  if ( filePath != ""  && filePath.indexOf(base)>=0 ){
      var url = "?root="+encodeURI(filePath);
      print( "<tr><td><a href=\""+url+"\" class=\"lnk\">[<i class=\"fa fa-angle-double-up fa-lg\"></i>]</a></td><td></td><td></td></tr>\n");
  }
  //ディレクトリの場合
  if (dir_exists(root)) {
      //ディレクトリ読み込み
      var files = eval(scandir(root));
      for( var i=files.length-1; i>=0 ;i--){
          if( basename(files[i]) == "." || basename(files[i]) == ".." ){
              files.remove(files[i]);
          }
      }
      if( files.length>0 ){
          //check each folders
          for( var i = 0 ; i < files.length ; i++ ){
              var file = files[i];
              var filePath = root+"/"+basename(file);
              var filePathe = encodeURI(filePath);
              if( dir_exists(file) ){
                  //make link tag
                  var stat = eval(file_stat(file));
                  var title = stat.date+" "+stat.permission;
                  var url1 = "?root="+filePathe;
                  var url2 = "?root="+encodeURI(root)+"&del="+filePathe;
                  var url3 = "?root="+encodeURI(root)+"&renf="+filePathe;
                  //print( "<tr><td><a href=\""+url1+"\" class=\"lnk\" title=\""+title+"\">["+basename(file)+"]</a></td>");
                  //print( "<td>".stat.date."</td>";
                  //print "<td></td>"
                  //print( "<td><a href=\"#\" onClick='deleteFile(\""+url2+"\",\""+basename(file)+"\");' title=\"Delete Folder\"><i class=\"fa fa-trash-o fa-lg\"></i></a></td>");
                  //print( "<td><a href=\"#\" onClick='renameFile(\""+url3+"\",\""+basename(file)+"\");' title=\"Rename Folder\"><i class=\"fa fa-pencil-square-o fa-lg\"></i></a></td></tr>\n");
                  print("<tr><td>\n");
                  print("<div class=\"btn-group\">\n");
                  print("<a href=\"#\" type=\"button\" data-toggle=\"dropdown\" class=\"dropdown-toggle\">["+basename(file)+"]</span></a>\n");
                  print("<ul class=\"dropdown-menu\">\n");
                      print("     <li><a href=\""+url1+"\" >open</a></li>\n");
                      print("     <li><a href=\"#\" onClick='deleteFile(\""+url2+"\",\""+basename(file)+"\");'>Delete</a></li>\n");
                      print("     <li><a href=\"#\" onClick='renameFile(\""+url3+"\",\""+basename(file)+"\");'>Rename</a></li>\n");
                      print("     <li class=\"divider\"></li>\n");
                      print("     <li><a href=\"#\">Separated link</a></li>\n");
                  print(" </ul>\n");
                  print("</div>\n");
                  print( "</td></tr>\n");
              }
          }
          //check each files.
          for( var i=0 ; i < files.length ; i++ ){
              var file = files[i];
              var filePath = root+"/"+basename(file);
              var filePathe = encodeURI(filePath);
              var filePathf = filePath.substring(base.length,filePath.length);
              if( file_exists(filePath) ){
                  var ext = extractFileExt(file);
                  var disp = 1;
                  if( ext.toLowerCase() == "bak" ){
                      disp = 0;
                  }else if( ext == "swf" || ext == "jpg" || ext == "gif" || ext == "png" ){
                      disp = 2;
                  }else if( ext == "mp3" ){
                      disp = 3;
                  }else if( search.length > 0 ){
                      var fil = loadFromFile(filePath);
                      if( fil.indexOf(search)<0){
                          disp = 0;
                      }
                  }
                  if( disp ){
                      //make link tag
                      var stat = eval(file_stat(filePath));
                      var title = stat.date+" "+stat.permission;
                      var url1 = "editor.jss?path="+filePathe;
                      var url2 = me+"?del="+filePathe;
                      var url3 = me+"?renf="+filePathe;
                      var lnk = "lnk";
                      var wte = "";
                      if( disp == 1 ){
                          wte = "editCode(\""+url1+"\");";
                      }else if( disp == 2 ){
                          wte = "viewDataImage(\""+filePathf+"\");";
                      }else if( disp == 3 ){
                          wte = "viewDataSound(\""+filePathf+"\");";
                      }
                      print("<tr><td>\n");
                      print("<div class=\"btn-group\">\n");
                      print("<a href=\"#\" type=\"button\" data-toggle=\"dropdown\" class=\"dropdown-toggle\">"+basename(file)+"</span></a>\n");
                      print("<ul class=\"dropdown-menu\">\n");
                      print("     <li><a href=\"#\" onClick='viewtCode(\""+url1+"\");'>View</a></li>\n");
                      print("     <li><a href=\"#\" onClick='"+wte+"'>Edit</a></li>\n");
                      print("     <li><a href=\"#\" onClick='deleteFile(\""+url2+"\",\""+basename(file)+"\");'>Delete</a></li>\n");
                      print("     <li><a href=\"#\" onClick='renameFile(\""+url3+"\",\""+basename(file)+"\");'>Rename</a></li>\n");
                      print("     <li class=\"divider\"></li>\n");
                      print("     <li><a href=\"#\">Separated link</a></li>\n");
                      print(" </ul>\n");
                      print("</div>\n");



                      //print "<td>".date("m/d H:i:s", filemtime($filePath))."</td>"; 
                      //print "<td align=\"right\">".size_num_read(filesize($filePath))."</td>";
                      //print( "<td><a href=\"#\" onClick='deleteFile(\""+url2+"\",\""+basename(file)+"\");' title=\"Delete File\"><i class=\"fa fa-trash-o fa-lg\"></i></a></td>");
                      //print( "<td><a href=\"#\" onClick='renameFile(\""+url3+"\",\""+basename(file)+"\");' title=\"REname File\"><i class=\"fa fa-pencil-square-o fa-lg\"></i></a></td>");
                      print( "</td></tr>\n");
                  }
              }
          }
      }
  }
  print("</table>\n");
  print( "<hr>");
?>
  </div>
</div>
  <div class="col-xs-12 col-sm-10 col-md-10">
    <ul class="nav nav-tabs">
      <li class="active"><a href="#">Home</a></li>
      <li><a href="#">Menu 1</a></li>
      <li><a href="#">Menu 2</a></li>
      <li><a href="#">Menu 3</a></li>
    </ul>
        <pre id="editor"><?print(htmlspecialchars(code));?></pre>


  </div>
</div>
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
 
             return true;
        });
    });
    </script>
    <!-- SCRIPTS //-->
    <script src="https://cdnjs.cloudflare.com/ajax/libs/ace/1.2.0/ace.js"></script>
    <script type="text/javascript">
      <!--
      var editor = ace.edit("editor");
      editor.setTheme("ace/theme/textmate");
      //-->
    </script>
  </body>
</html>
