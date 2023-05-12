<?
url = _GET.url;
?>
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>Image View<? print(url); ?></title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-rbsA2VBKQhggwzxH7pPCaAqO46MgnOM80zW1RWuH61DGLwZJEdK2Kadq2F9CUG65" crossorigin="anonymous">
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/js/bootstrap.bundle.min.js" integrity="sha384-kenU1KFdBIe4zVF0s0G1M5b4hcpxyD9F7jL+jjXkk+Q2h455rYXK/7HAuoJl+0I4" crossorigin="anonymous"></script>
    <script src="https://cdn.jsdelivr.net/npm/showdown@2.1.0/dist/showdown.min.js"></script>
</head>
<body>
    <div class="container">
        <div id="md"></id>
        <input type="button" class="btn btn-primary" value="戻る" onClick="history.back();">
    </div>
<script type="text/plain" id="text"><?print(loadFromFile(_SERVER.DOCUMENT_ROOT+url));?></script>  
    <script>
        // markdownのテキストを定義する
        var markdown = document.getElementById('text').textContent;
        // コンバーターを作成する
        var converter = new showdown.Converter();
        converter.setOption('tables', true);
        // markdownをhtmlに変換する
        var html = converter.makeHtml(markdown);
        var mydiv = document.getElementById("md");
        mydiv.innerHTML = html;
    </script>
</body>
</html>
