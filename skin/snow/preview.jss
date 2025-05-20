<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>watch movie</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-rbsA2VBKQhggwzxH7pPCaAqO46MgnOM80zW1RWuH61DGLwZJEdK2Kadq2F9CUG65" crossorigin="anonymous">
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/js/bootstrap.bundle.min.js" integrity="sha384-kenU1KFdBIe4zVF0s0G1M5b4hcpxyD9F7jL+jjXkk+Q2h455rYXK/7HAuoJl+0I4" crossorigin="anonymous"></script>
</head>
<body>
    <div class="container">
      <button type="button" class="btn btn-primary" onClick="history.back();">戻る</button>
      <?
      //UserAgentCheck
      var ua=_SERVER.HTTP_USER_AGENT;
      var ios = ((ua.indexOf("iPhone")>0) || (ua.indexOf("iPod")>0) );
      var android = (ua.indexOf("Android")>0);
      var target2 = "";

      //set target movie url
      url = _GET.url;
      if( url !== undefined ){

          //get extension
          ext  = extractFileExt(url);

          //generate fullpath
          //name = _SERVER.SCRIPT_NAME;
          name = "/"; //basename;
          path = "http://" +_SERVER.HTTP_HOST + name;
          if( url.indexOf("http://") < 0 && url.indexOf("https://") < 0 ){
              target2 = path+url;
          }else{
              target2 = url;
          }
          target= encodeURI(target2);

          //set width/height(if needed)
          width =_GET.width; if( width.length==0) width=320;
          height=_GET.height;if( height.length==0) height=200;

          //set poster とりあえずカレント指定
          poster2=path+"poster.jpg";
          poster=encodeURI(poster2);
    
          //DIVセンタリング
          print( "<div style=\"width:"+width+"px; margin:0 auto 0 auto;\">\n" );
          if (true){
              print("<video autoplay controls poster=\""+poster2+"\" preload=\"none\" onclick=\"this.play()\" width=\""+width+"\" height=\""+height+"\">\n");
              print("<source src=\""+url+"\">\n");
              print("<p>動画を再生するにはvideoタグをサポートしたブラウザが必要です。</p>\n");
              print("</video>\n");
          }
      }
      ?>
      <br />視聴できない場合はこの<a href="<?print(target2);?>">リンク</a>をクリックしてください。<br />
    </div>
</body>
</html>
