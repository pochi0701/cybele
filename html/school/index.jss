<?  session_start();
    var scriptn  = _SERVER.SCRIPT_NAME;
    var base = "http://"+_SERVER.SERVER_ADDR+":"+_SERVER.SERVER_PORT+dirname(scriptn)+"/initSQL.jss";
    if(file_exists(_SERVER.DOCUMENT_ROOT+dirname(scriptn)+"/initSQL.jss")){
        loadFromFile(base);
    }
    var course_no;
    var database;
    database = DBConnect("sample_database");
    var tmp = database.SQL("select * from course order by no;");
    var elm;
    if(tmp.startsWith("[") ){
        elm = eval(tmp);
    }else{
        elm = undefined;
    }
    database.DBDisConnect();
?>
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>コース一覧[オンライン学習]</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-9ndCyUaIbzAi2FUVXJi0CjmCapSmO7SnpJef0486qhLnuZ2cdeRhO02iuK6FUUVM" crossorigin="anonymous">
</head>
<body>
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
    
        <h2>お知らせ</h2>
        <div class="accordion" id="accordionPanelsStayOpenExample" color="info">
            <div class="accordion-item">
                <h2 class="accordion-header">
                    <button class="accordion-button" type="button" data-bs-toggle="collapse" data-bs-target="#panelsStayOpen-collapseNews" aria-expanded="true" aria-controls="panelsStayOpen-collapseNews">
                        コンテンツについて
                    </button>
                </h2>
                <div id="panelsStayOpen-collapseNews" class="accordion-collapse collapse show">
                    <div class="accordion-body">
                        「Cybeleの使い方紹介」は無料コンテンツとなっています。<br>
                        ご自由にご利用ください。タイトルの下部をクリックすると、コンテンツに移動します。
                    </div>
                </div>
            </div>
        </div>
        <br>
    
        <h2>コース一覧</h2>
        <div class="accordion" id="accordionPanelsStayOpenExample">
            <?
            //print(elm);
            for(var i = 0 ; i<elm.length ; i++){
            print('<div class="accordion-item">\r\n');
            print('    <h2 class="accordion-header">\r\n');
            print('        <button class="accordion-button" type="button" data-bs-toggle="collapse" data-bs-target="#panelsStayOpen-collapse'+i+'" aria-expanded="true" aria-controls="panelsStayOpen-collapse'+i+'">\r\n');
                               print(htmlspecialchars(elm[i].name));
            print('        </button>\r\n');
            print('    </h2>\r\n');
            print('    <div id="panelsStayOpen-collapse'+i+'" class="accordion-collapse collapse show">\r\n');
            //print('        <div class="accordion-body"  data-url="content.jss?no='+elm[i].no+'">\r\n');
            print('        <div class="accordion-body"  data-url="">\r\n');
            //print(elm[i].detail+"<br>");
            if(elm[i].purchase.length>0){
                print('購入済：<a class="btn btn-outline-primary" href="content.jss?no='+elm[i].no+'">コンテンツを開く</a>\r\n');
            }else{
                print('未購入：<a class="btn btn-outline-primary" href="get_content.jss?version='+elm[i].no+'">購入</a>\r\n');
            }
            print('        </div>\r\n');
            print('    </div>\r\n');
            print('</div><br>\r\n');
            }
            ?>
        </div>
    </div>
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/js/bootstrap.bundle.min.js" integrity="sha384-geWF76RCwLtnZ8qwWowPQNguL3RmwHVBC9FhGdlKrxdiJJigb/j/68SIy3Te4Bkz" crossorigin="anonymous"></script>
    <script>
  //document.querySelectorAll('.accordion-body').forEach(function(item) {
  //  item.addEventListener('click', function() {
  //      var uri = item.getAttribute('data-url');
  //      if( uri != null ){
  //          window.location.href = uri;
  //      }
  //  });
  //});
</script>
</body>
</html>