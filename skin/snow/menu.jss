<?
//URLエンコード
function size_num_read(size) {
    bytes = ["B","KB","MB","GB","TB"];
    for( i=0 ; i < 5 ; i++) {
        if(size > 1024){
            size = size / 1024;
        }else{
            break;
        }
    }
    //return "<span style=\"font-size: small\">".	round($size, 2).$val."</span>";
    return (round(size*100)/100)+bytes[i];
}
//変数取得
base    = _SERVER.DOCUMENT_ROOT;
root    = _GET.root;

//ユーザ限定処理
//初回の処理
if( root.length==0){
    root = _SERVER.DOCUMENT_ROOT;
}
sf = root.substring(base.length,root.length);
if( sf == ""){
   sf = "/";
}
//右端の/をなくす
while( root[root.length-1] == "/" ){
    root = root.substring(0,root.length-1);
}

me=_SERVER.SCRIPT_NAME;
?><!DOCTYPE html>
<html>
<head>
<title>file tree - <? print(sf); ?></title>
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
<link href="/css/menu.css" rel="stylesheet" type="text/css" />
<script type="text/javascript">
<!--
var root = "<? print( root ); ?>";
// -->
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
  [<? print( sf ); ?>]
  <div id="tree" />
  <table>
<?
  //親ディレクトリ
  filePath = dirname(root);
  if ( filePath != ""  && filePath.indexOf(base)>=0 ){
      url = "?root="+encodeURI(filePath);
      print( "<tr><td><a href=\""+url+"\" class=\"lnk\">[<img src=\"/image/up.gif\" border=\"0\">]</a></td></tr>\n");
  }
  //ディレクトリの場合
  if (dir_exists(root)) {

      //ディレクトリ読み込み
      files = eval(scandir(root));
      for( i=files.length-1; i>=0 ;i--){
          if( basename(files[i]) == "." || basename(files[i]) == ".." ){
              files.remove(files[i]);
          }
      }
      if( files.length>0 ){
          //check each folders
          for( i = 0 ; i < files.length ; i++ ){
              file = files[i];
              filePath = root+"/"+basename(file);
              if( dir_exists(file) ){
                  //make link tag
                  stat = eval(file_stat(file));
                  title = stat.date+" "+stat.permission;
                  url1 = "?root="+encodeURI(filePath);
                  print( "<tr><td><img src=\"/image/snow-icon-folder.png\"></td><td><a href=\""+url1+"\" class=\"lnk\" title=\""+title+"\">["+basename(file)+"]</a></td></tr>\n");
              }
          }
          //check each files.
          for( i=0 ; i < files.length ; i++ ){
              file = files[i];
              filePath = root+"/"+basename(file);
              if( file_exists(filePath) ){
                  fl = filePath;
                  if( fl.indexOf(base)>=0){
                      fl = dirname(fl.substring(base.length,fl.length));
                  }
                  fl= fl+"/";
                  ext = extractFileExt(file).toLowerCase();
                  if( ext != "bak"){
                      fname = basename(file).nkfconv("Sw");
                      option = "";
                      
                      //make link tag
                      stat = eval(file_stat(filePath));
                      title = stat.date+" "+stat.permission;
                      if( ext == "jpg" || ext == "png" || ext == "gif" ){
                          icon = "fa fa-file-image-o";//"/image/snow-icon-image.png";
                          url1 = fl+basename(filePath)+"?action=ImageView.jss";
                      }else if( ext == "mp4" || ext == "3gp" || ext == "mov" ){
                          icon = "fa fa-file-video-o";//"/image/snow-icon-movie.png";
                          url1 = fl+basename(filePath)+"?action=preview.jss";
                      }else if( ext == "mp3" ){
                          var mp3=eval(JSON.mp3id3tag(filePath));
                          if( mp3.title.length ){
                              fname = mp3.title;
                          }
                          option = mp3.artist+" "+mp3.year;
                          icon = "fa fa-music";//"/image/snow-icon-music.png";
                          url1 = fl+basename(filePath)+"?action=audio.jss";
                      }else if( ext == "html" || ext == "htm" ){
                          icon = "fa fa-file";//"/image/snow-icon-doc.png";
                          url1 = fl+basename(filePath);
                      }else if( ext == "jss" ){
                          icon = "fa fa-file-code-o";//"/image/snow-icon-doc.png";
                          url1 = fl+basename(filePath);
                      }else{
                          icon = "fa fa-file";//"/image/snow-icon-unknown.png";
                          url1 = fl+basename(filePath);
                      }
                      lnk = "lnk";
                      print( "<tr><td><i class=\""+icon+"\" aria-hidden=\"true\"></i></td><td><a href=\""+url1+"\" class=\""+lnk+"\" title=\""+title+"\" >"+fname+"</a></td><td>"+option+"</td></tr>\n");
                  }
              }
          }
      }
  }
  print("</table>\n");
?>
</div>
</div>
</body>
</html>
