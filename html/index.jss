<!DOCTYPE html>
<html lang="ja">
<head>
    <meta charset="utf-8">
    <title>menu</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-9ndCyUaIbzAi2FUVXJi0CjmCapSmO7SnpJef0486qhLnuZ2cdeRhO02iuK6FUUVM" crossorigin="anonymous">
    <script src="https://cdnjs.cloudflare.com/ajax/libs/qrcodejs/1.0.0/qrcode.min.js"></script>
    <script src="https://cdn.jsdelivr.net/npm/axios/dist/axios.min.js"></script>    
</head>
<body background="images/winter03.jpg" bgcolor="#999999" FOCUSCOLOR="#550033" FOCUSTEXT="#FFFFFF">
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
            <img src="images/cybele.png" width="304" height="79"/>
        </div>
    </div>
    <div class="container d-flex align-items-center justify-content-center">
        <table>
            <tr id="icons">
            </tr>
        </table>
    </div>
    <div>
        <div class="text-end"><img src="images/birdland_logo.png"/> </div>
    </div>
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/js/bootstrap.bundle.min.js" integrity="sha384-geWF76RCwLtnZ8qwWowPQNguL3RmwHVBC9FhGdlKrxdiJJigb/j/68SIy3Te4Bkz" crossorigin="anonymous"></script>
    <script>
        function setup(url) {
            axios.get(`/data/setup.json`)
                .then(function (response) {
                    var repl  = "<?print (getLocalAddress() + ':' + getLocalPort());?>"
                    var setup = eval(response.data);
                    var contents="";
                    for( i = 0 ; i < setup.length ; i++){
                        if( setup[i].url.indexOf('%LOCALADDRESS%') >= 0 ){
                            setup[i].url = setup[i].url.replace('%LOCALADDRESS%',repl);
                        }
                        contents += '<td><a href="'+setup[i].url+'">'+setup[i].image+'</a></td>\n';
                    }
                    document.getElementById("icons").innerHTML = contents;
                    new QRCode(document.getElementById("qrcodeCanvas1"), { width: 96, height: 96, text: "http://<?print (getLocalAddress()+':'+getLocalPort()+'/');?>" });  
                    return false;
                })
                .catch(function (error) {
                    console.log(error);
                });
        }
        setup();
    </script>
</body>
</html>
