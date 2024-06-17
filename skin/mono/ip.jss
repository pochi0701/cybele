<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>ip address</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-9ndCyUaIbzAi2FUVXJi0CjmCapSmO7SnpJef0486qhLnuZ2cdeRhO02iuK6FUUVM" crossorigin="anonymous">
    <script src="https://cdnjs.cloudflare.com/ajax/libs/qrcodejs/1.0.0/qrcode.min.js"></script>
</head>
<body>
    <div class="container">
        <button type="button" class="btn btn-primary" onClick="history.back();">戻る</button><br/>
        <?
        la = getLocalAddress();
        lp = getLocalPort();
        ga = loadFromFile("http://neon.cx/cybele/ip.php");
        ?>
        Local Address<br />
        <?print(la + ":" + lp + "/"); ?><br />
        <div id="qrcodeCanvas"></div>
        <br />
        <br />
        <br />
        Global Address<br />
        グローバルアドレスは、サーバーが外部に開放されてないと見ることができません<br/>
        <? print(ga + ":" + lp + "/"); ?><br />
        <div id="qrcodeCanvas2"></div>
        <script>
             new QRCode(document.getElementById("qrcodeCanvas"), { width: 96, height: 96, text: "http://<? print(la + ":" + lp + "/"); ?>" });
             new QRCode(document.getElementById("qrcodeCanvas2"), { width: 96, height: 96, text: "http://<? print(ga + ":" + lp + "/"); ?>" });
        </script>
    </div>
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/js/bootstrap.bundle.min.js" integrity="sha384-geWF76RCwLtnZ8qwWowPQNguL3RmwHVBC9FhGdlKrxdiJJigb/j/68SIy3Te4Bkz" crossorigin="anonymous"></script>
</body>
</html>
