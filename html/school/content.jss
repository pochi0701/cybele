<?  session_start();
    var database;
    var elm;
    var course_no = _GET.no;
    database = DBConnect("sample_database");
    var tmp = database.SQL("select * from content where no="+course_no+" order by content_no;");
    if(tmp.startsWith("[")){
        elm = eval(tmp);
        for(i = 0 ; i < elm.length ; i++){
           tmp2 = database.SQL("select count(*) as cnt from subcontent where no="+course_no+" and content_no="+(i+1)+" and done<100;");
           // 完了してないコンテンツの個数
           num = eval(tmp2);
           elm[i].cnt = num.cnt;
        }
    }
    database.DBDisConnect();
?>
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>コンテンツ一覧[オンライン学習]</title>
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
        <div class="row">
            <div class="col">
                <div class="btn bg-info btn-block">
                    <a href="#" onclick="window.location.href='index.jss';">コース一覧に戻る</a>
                </div>
            </div>
        </div>
        <h2>お知らせ</h2>
        <div class="accordion" id="accordionPanelsStayOpenNews">
            <div class="accordion-item">
                <h2 class="accordion-header">
                    <button class="accordion-button" type="button" data-bs-toggle="collapse" data-bs-target="#panelsStayOpen-collapseNews" aria-expanded="true" aria-controls="panelsStayOpen-collapseNews">
                        無料コンテンツ
                    </button>
                </h2>
                <div id="panelsStayOpen-collapseNews" class="accordion-collapse collapse show">
                    <div class="accordion-body">
                        Cybeleの使い方は無料コンテンツとなっています。<br>
                        ご自由にご利用ください。
                    </div>
                </div>
            </div>
        </div>
        <br>
    
        <h2>コンテンツ一覧</h2>
        <div class="accordion" id="accordionPanelsStay">
            <?
            for(var i = 0 ; i<elm.length ; i++){
            print('<div class="accordion-item">\r\n');
            print('    <h2 class="accordion-header">\r\n');
            print('        <button class="accordion-button" type="button" data-bs-toggle="collapse" data-bs-target="#panelsStayOpen-collapse'+i+'" aria-expanded="true" aria-controls="panelsStayOpen-collapse'+i+'">\r\n');
                               print(htmlspecialchars(elm[i].name));
            print('        </button>\r\n');
            print('    </h2>\r\n');
            print('    <div id="panelsStayOpen-collapse'+i+'" class="accordion-collapse collapse show">\r\n');
            print('        <div class="accordion-body">\r\n');
            print('<a class="btn btn-outline-primary" href="subcontent.jss?no='+elm[i].no+'&contentno='+elm[i].content_no+'">閲覧</a>\r\n');
            print(elm[i].detail+"<br>\r\n");
            if(elm[i].cnt > 0){
                print('残り課題 <span class="badge text-bg-danger">'+elm[i].cnt+'</span>\r\n');
            }else{
                print('残り課題 <span class="badge text-bg-info">'+elm[i].cnt+'</span>\r\n');
            }
            print('        </div>\r\n');
            print('    </div>\r\n');
            print('</div>\r\n');
            }
            ?>
        </div>
    </div>
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/js/bootstrap.bundle.min.js" integrity="sha384-geWF76RCwLtnZ8qwWowPQNguL3RmwHVBC9FhGdlKrxdiJJigb/j/68SIy3Te4Bkz" crossorigin="anonymous"></script>
    <script>
      //document.querySelectorAll('.accordion-body').forEach(function(item) {
      //  item.addEventListener('click', function() {
      //    var uri = item.getAttribute('data-url');
      //    if( uri != null ){
      //        window.location.href = uri;
      //    }
      //  });
      //});
      //document.addEventListener('visibilitychange', function () {
      //  if (document.visibilityState === 'visible') {
      //     window.location.reload();
      //  }
      //});
    </script>
  </body>
</html>