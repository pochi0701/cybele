<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>watch movie</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-0evHe/X+R7YkIZDRvuzKMRqM+OrBnVFBL6DOitfPri4tjfHxaWutUpFmBp4vmVor" crossorigin="anonymous">
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/js/bootstrap.bundle.min.js" integrity="sha384-pprn3073KE6tl6bjs2QrFaJGz5/SUsLqktiwsUTF55Jfv3qYSDhgCecCxMW52nD2" crossorigin="anonymous"></script>
</head>
<body>
    <?
    //UserAgentCheck
    var ua=_SERVER.HTTP_USER_AGENT;
    var ios = ((ua.indexOf("iPhone")>0) || (ua.indexOf("iPod")>0) );
    var android = (ua.indexOf("Android")>0);
    var target2 = "";

    //set target movie url
    url = _GET.url;
    if( url != undefined ){

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
    <form>
        <input type="button" value="戻る" onClick="history.back()">
    </form>
</body>
</html>
