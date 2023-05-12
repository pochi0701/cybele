<!doctype html>
<html lang="ja">
<head>
    <meta charset="utf-8">
    <title>menu</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-rbsA2VBKQhggwzxH7pPCaAqO46MgnOM80zW1RWuH61DGLwZJEdK2Kadq2F9CUG65" crossorigin="anonymous">
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.1.1/css/all.min.css">
    <script src="http://code.jquery.com/jquery-1.12.0.min.js"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/jquery.qrcode/1.0/jquery.qrcode.min.js"></script>
</head>
<body background="winter04.jpg" bgcolor="#999999" FOCUSCOLOR="#550033" FOCUSTEXT="#FFFFFF">
    <!-- 1.ナビゲーションバーの設定 -->
    <nav class="navbar navbar-expand-lg navbar-dark bg-dark sticky-top">
        <div class="container-fluid">
            <a class="navbar-brand" href="/" target="_top">Cybele</a>
            <button class="navbar-toggler" type="button" data-bs-toggle="collapse" data-bs-target="#navbarSupportedContent" aria-controls="navbarSupportedContent" aria-expanded="false" aria-label="Toggle navigation">
                <span class="navbar-toggler-icon"></span>
            </button>
            <div class="collapse navbar-collapse flex-grow-1 text-end" id="myNavbar">
                <ul class="navbar-nav ms-auto flex-nowrap">
                    <li><a href="/" class="nav-link m-2 menu-item" target="_top">Home</a></li>
                </ul>
            </div>
        </div>
    </nav>
    <div class="container">
        <div class="text-center">
            <img src="cybele.png" width="304" height="79">
        </div>
    </div>
    <div class="container d-flex align-items-center justify-content-center">
        <table>
            <tr id="icons">
            </tr>
        </table>
    </div>

    <div>
        <div class="text-end"><img src="/images/birdland_logo.png"> </div>
    </div>
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/js/bootstrap.bundle.min.js" integrity="sha384-kenU1KFdBIe4zVF0s0G1M5b4hcpxyD9F7jL+jjXkk+Q2h455rYXK/7HAuoJl+0I4" crossorigin="anonymous"></script>
    <script>
        var repl  = "<?print(getLocalAddress());?>:8000"
        var setup = <?print(loadFromFile("setup.json"));?>;
        var contents="";
        for( i = 0 ; i < setup.length;i++){
            contents += '<td><a href="'+setup[i].url+'">'+setup[i].image+'</a></td>\n';
        }
        document.getElementById("icons").innerHTML = contents;
        jQuery('#qrcodeCanvas1').qrcode({ width: 96, height: 96, text: "http://<? print(getLocalAddress()); ?>:8000/" });
    </script>
</body>
</html>
