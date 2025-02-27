<?  session_start();
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
                        無料コンテンツ
                    </button>
                </h2>
                <div id="panelsStayOpen-collapseNews" class="accordion-collapse collapse show">
                    <div class="accordion-body">
                        Cybeleの使い方紹介は無料コンテンツとなっています。<br>
                        ご自由にご利用ください。
                    </div>
                </div>
            </div>
        </div>
        <br>
    
        <h2>コース一覧</h2>
        <div class="accordion" id="accordionPanelsStayOpenExample">
            <?
            for(var i = 0 ; i<elm.length ; i++){
            print('<div class="accordion-item">');
            print('    <h2 class="accordion-header">');
            print('        <button class="accordion-button" type="button" data-bs-toggle="collapse" data-bs-target="#panelsStayOpen-collapse'+i+'" aria-expanded="true" aria-controls="panelsStayOpen-collapse'+i+'">');
                               print(htmlspecialchars(elm[i].name));
            print('        </button>');
            print('    </h2>');
            print('    <div id="panelsStayOpen-collapse'+i+'" class="accordion-collapse collapse show">');
            print('        <div class="accordion-body"  data-url="content.jss?no='+elm[i].no+'">');
            print(elm[0].detail+"<br>");
            print("価格:"+elm[i].price+"<br>");
            print("価格:"+elm[i].price+"<br>");
            print('        </div>');
            print('    </div>');
            print('</div><br>');
            }
            ?>
        </div>
    </div>
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/js/bootstrap.bundle.min.js" integrity="sha384-geWF76RCwLtnZ8qwWowPQNguL3RmwHVBC9FhGdlKrxdiJJigb/j/68SIy3Te4Bkz" crossorigin="anonymous"></script>
    <script>
  document.querySelectorAll('.accordion-body').forEach(function(item) {
    item.addEventListener('click', function() {
      window.location.href = item.getAttribute('data-url');
    });
  });
</script>
</body>
</html>