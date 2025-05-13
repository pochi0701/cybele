<?
var url = _GET.url;
var la = getLocalAddress();
var lp = getLocalPort();
url = "http://"+la + ":" + lp +url;
?><!DOCTYPE html>
<html lang="ja">
<head>
    <meta charset="utf-8">
    <title>QRCode</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-9ndCyUaIbzAi2FUVXJi0CjmCapSmO7SnpJef0486qhLnuZ2cdeRhO02iuK6FUUVM" crossorigin="anonymous">
    <script src="https://cdnjs.cloudflare.com/ajax/libs/qrcodejs/1.0.0/qrcode.min.js"></script>
</head>
<body>
    <div class="container">
        <button type="button" class="btn btn-primary" onClick="window.close();">閉じる</button><br/><br/>
        <div id="qrcodeCanvas"></div>
        <br/>
        URL:<a href="<? print(url); ?>"><? print(url); ?></a>
    </div>
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/js/bootstrap.bundle.min.js" integrity="sha384-geWF76RCwLtnZ8qwWowPQNguL3RmwHVBC9FhGdlKrxdiJJigb/j/68SIy3Te4Bkz" crossorigin="anonymous"></script>
    <script>
        new QRCode(document.getElementById("qrcodeCanvas"), { width: 96, height: 96, text: "<? print(url); ?>" });
    </script>
</body>
</html>
