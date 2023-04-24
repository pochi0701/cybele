<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>ip address</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-0evHe/X+R7YkIZDRvuzKMRqM+OrBnVFBL6DOitfPri4tjfHxaWutUpFmBp4vmVor" crossorigin="anonymous">
    <script src="http://code.jquery.com/jquery-1.12.0.min.js"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/jquery.qrcode/1.0/jquery.qrcode.min.js"></script>
</head>
<body>
    <div class="container">
        <?
        la = getLocalAddress();
        ga = loadFromFile("http://neon.cx/cybele/ip.php");
        ?>
        Local Address<br />
        <? print(la+":8000/"); ?><br />
        <div id="qrcodeCanvas"></div>
        <br />
        <br />
        <br />
        Global Address<br />
        <? print(ga+":8000/"); ?><br />
        <div id="qrcodeCanvas2"></div>
        <script>
            jQuery('#qrcodeCanvas').qrcode({
                width: 96, height: 96,
                text: "http://<? print(la+": 8000/"); ?>"
            });
            jQuery('#qrcodeCanvas2').qrcode({
                width: 96, height: 96,
                text: "http://<? print(ga+": 8000/"); ?>"
            });
        </script>
        <form>
            <input type="button" value="戻る" onClick="history.back()">
        </form>
    </div>
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/js/bootstrap.bundle.min.js" integrity="sha384-pprn3073KE6tl6bjs2QrFaJGz5/SUsLqktiwsUTF55Jfv3qYSDhgCecCxMW52nD2" crossorigin="anonymous"></script>
</body>
</html>
