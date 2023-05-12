<?
url = _GET.url;
if( url != undefined ){
  var name = eval(JSON.mp3id3tag(_SERVER.DOCUMENT_ROOT+url));
}
?>
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>music play<? print(name.title); ?></title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-rbsA2VBKQhggwzxH7pPCaAqO46MgnOM80zW1RWuH61DGLwZJEdK2Kadq2F9CUG65" crossorigin="anonymous">
    <script src="http://code.jquery.com/jquery-1.12.0.min.js"></script>
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/js/bootstrap.bundle.min.js" integrity="sha384-kenU1KFdBIe4zVF0s0G1M5b4hcpxyD9F7jL+jjXkk+Q2h455rYXK/7HAuoJl+0I4" crossorigin="anonymous"></script>
    <script type="text/javascript">
        <!--
        var root = "<? print( root ); ?>";
        // -->
    </script>
</head>
<body>
    <div class="container">
        <?
        //UserAgentCheck
        ua      = _SERVER.HTTP_USER_AGENT;
        ios     = ((ua.indexOf("iPhone")>0) || (ua.indexOf("iPod")>0) );
        android = (ua.indexOf("Android")>0);

        //set target movie url
        if( url != undefined ){
            print("TITLE  :"+name.title+"<br>\n");
            print("ALBUM  :"+name.album+"<br>\n");
            print("ARTIST :"+name.artist+"<br>\n");
            //DIVセンタリング
            print( "<div style=\"margin:0 auto 0 auto;\">\n" );
            print( "<audio src=\""+url+"\" preload=\"auto\" controls autoplay=\"true\">you need audio tag enabled browser.</audio>\n");
            print( "</div>" );
        }
        ?>
        <form>
            <input class="btn btn-primary" type="button" value="戻る" onClick="history.back()">
        </form>
    </div>
</body>
</html>
