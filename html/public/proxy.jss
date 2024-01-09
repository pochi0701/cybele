<?
var url = _POST['url'];
?>
<html lang="ja">
    <head>
        <meta charset="UTF-8">
        <title><? print(url); ?></title>
        <style type="text/css">
            html, body {
                height:100%;
            }
            #container {
                height: 100%;
                padding: 1em;
            }
            iframe {
                width: 100%;
                height: 100%;
                border:1px solid #CCC;
            }
        </style>
    </head>
<body>
<form action="proxy.jss" method="post">
URL: <input type="text" name="url" />
<input type="submit" />
</form>
<?
if( url.length > 0 )
{
    print( '<div id="container">\n' );
    print( '<iframe src="/-.-'+url+'" name="sample">この部分はインラインフレームを使用しています。</iframe>\n' );
    print( '</div>\n' );
}
?>
</body>
</html>
