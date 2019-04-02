<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1,user-scalable=no">
<meta name="format-detection" content="telephone=no" />
<meta name="apple-mobile-web-app-capable" content="yes" />  
<meta name="apple-mobile-web-app-status-bar-style" content="black-translucent">  
<meta http-equiv="Cache-Control" content="no-cache">
<title>watch movie</title>
</head>
<body>
<?
//UserAgentCheck
ua=_SERVER.HTTP_USER_AGENT;
ios = ((ua.indexOf("iPhone")>0) || (ua.indexOf("iPod")>0) );
android = (ua.indexOf("Android")>0);

//set target movie url
url = _GET.url;
if( url != undefined ){
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
  print( "<div style=\"width:"+width+"px; margin:0 auto 0 auto;\">\n" );
  if (ios || android){
    print("<video autoplay controls poster=\""+poster2+"\" preload=\"none\" onclick=\"this.play()\"  width=\""+width+"\" height=\""+height+"\">\n");
    print("<source src=\""+url+"\">\n");
    print("<p>動画を再生するにはvideoタグをサポートしたブラウザが必要です。</p>\n");
    print("</video>\n");
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
<br/>視聴できない場合はこの<a href="<?print(target2);?>">リンク</a>をクリックしてください。<br/>
<form>
<input type="button" value="戻る" onClick="history.back()">
</form>
</div>
</body>
</html>
