<?
    session_start();
    if( _SESSION["uid"] !== undefined ){
        uid = _SESSION["uid"];
        pwd = _SESSION["pwd"];
        grd = _SESSION["grd"];
        unm = _SESSION["unm"];
    }
    if( uid === undefined ){
       header("Location: http://neon.cx/");
    }
    var database;
    //movie id
    var mid=_GET.mid;
    database = DBConnect("lesson");
    var mx1=1;
    var mx = eval(database.SQL("select id from history order by id desc limit 0,1;"));
    if( mx !== undefined ){
        mx1 = mx[0].id+1;
    }
    var dt = Date();
    database.SQL("insert into history(id,movie,uid,date) values("+mx1+","+mid+","+uid+",\""+dt.toDateString("%Y/%m/%d")+"\");");
    database.DBDisConnect();
?>
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <title>movie</title>
    <meta charset="utf-8">
    <title>menu</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-rbsA2VBKQhggwzxH7pPCaAqO46MgnOM80zW1RWuH61DGLwZJEdK2Kadq2F9CUG65" crossorigin="anonymous">
  <style>
   body {
      color:#fff !important;
      background:#000 !important;
   }
  </style>
</head>
<body>
<div class="text-center">
<h1><? print( "grade "+grd+". "+unm+"'s lesson ");?></h1>
</div>
<div class="container">
<?
//UserAgentCheck
ua=_SERVER.HTTP_USER_AGENT;
ios = ((ua.indexOf("iPhone")>0) || (ua.indexOf("iPod")>0) );
android = (ua.indexOf("Android")>0);

//set target movie url
url = _GET.url;
if( url !== undefined ){
  //get extension
  ext  = extractFileExt(url);
  
  //generate fullpath
  //name = _SERVER.SCRIPT_NAME;
  name = "/";//basename;
  path = "http://"+_SERVER.HTTP_HOST+name;
  if( url.indexOf("http://") < 0 && url.indexOf("https://") < 0 ){
      target2 = path+url;
  }else{
      target2 = url;
  }
  target= encodeURI(target2);
  //print(target);
  
  //set width/height(if needed)
  width =_GET.width; if( width.length==0) width=320;
  height=_GET.height;if( height.length==0) height=200;
  
  //set poster とりあえずカレント指定
  poster2=path+"poster.jpg";
  poster=encodeURI(poster2);
  
  //DIVセンタリング
  print( "<div class=\"text-center\">\n" );
  if (ext == 'mp4' || ios || android){
    print("<div align=\"center\" class=\"embed-responsive embed-responsive-16by9\">\n");
    print("  <video autoplay class=\"embed-responsive-item\" controls poster=\""+poster2+"\" preload=\"none\" onclick=\"this.play();\" >\n");
    print("    <source src=\""+url+"\" type=\"video/mp4\">\n");
    print("    <p>動画を再生するにはvideoタグをサポートしたブラウザが必要です。</p>\n");
    print("  </video>\n");
    print("</div>\n");
  }else if( ext != "3gp" ){
    print("<object width=\""+width+"\" height=\""+height+"\">\n");
    print("<param name=\"movie\" value=\"http://fpdownload.adobe.com/strobe/FlashMediaPlayback_101.swf\"></param>\n");
    print("<param name=\"flashvars\" value=\"src="+target+"&poster="+poster+"&playButtonOverlay=false&autoPlay=true\"></param>\n");
    print("<param name=\"allowFullScreen\" value=\"true\"></param>\n");
    print("<param name=\"allowscriptaccess\" value=\"always\"></param>\n");
    print("<embed\n");
    print(" src=\"http://fpdownload.adobe.com/strobe/FlashMediaPlayback_101.swf\"\n");
    print(" type=\"application/x-shockwave-flash\"\n");
    print(" allowscriptaccess=\"always\"\n");
    print(" allowfullscreen=\"true\"\n");
    print(" width=\""+width+"\"\n");
    print(" height=\""+height+"\"\n");
    print(" flashvars=\"src="+target+"&poster="+poster+"&playButtonOverlay=false&autoPlay=true\"></embed>\n");
    print("</object>\n");
  }else{
    print("<object name=\"QT\" classid=\"clsid:02BF25D5-8C17-4B23-BC80-D3488ABDDC6B\"");
    print(" codebase=\"http://www.apple.com/qtactivex/qtplugin.cab\" width=\""+width+"\" height=\""+heigh+"\">\n");
    print("<param name=\"type\" value=\"video/quicktime\"></param>\n");
    print("<param name=\"src\" value=\""+target2+"\"></param>\n");
    print("<param name=\"qtsrc\" value=\""+target2+"\"></param>\n");
    print("<param name=\"autoplay\" value=\"true\"></param>\n");
    print("<param name=\"controller\" value=\"true\"></param>\n");
    print("<param name=\"bgcolor\" value=\"#000000\"></param>\n");
    print("<embed\n");
    print(" name=\"QT\"\n");
    print(" src=\""+target2+"\"\n");
    print(" qtsrc=\""+target2+"\"\n");
    print(" autoplay=\"true\"\n");
    print(" controller=\"true\"\n");
    print(" bgcolor=\"#000000\"\n");
    print(" type=\"video/quicktime\"\n");
    print(" width=\""+width+"\"\n");
    print(" height=\""+height+"\"\n");
    print(" pluginspage=\"http://www.apple.com/qtactivex/qtplugin.cab\"></embed>\n");
    print("</object>\n");
  }
}
?>
<br/>If you can't see the movie, click <a href="<?print(target2);?>">[here]</a>.<br/>
<form>
<a href="lesson.jss" class="btn btn-primary btn-large">return</a>
<!-- <input type="button" class="btn btn-primary btn-large" value="return" onClick="history.back()"> -->
</form>
</div>
</div>
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/js/bootstrap.bundle.min.js" integrity="sha384-kenU1KFdBIe4zVF0s0G1M5b4hcpxyD9F7jL+jjXkk+Q2h455rYXK/7HAuoJl+0I4" crossorigin="anonymous"></script>
</body>
</html>
