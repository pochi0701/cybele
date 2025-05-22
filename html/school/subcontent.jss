<?  session_start();
    var database;
    var elm;
    var course_no = _GET.no;
    var content_no = _GET.contentno;
    var path = "/school/";//_SERVER.SCRIPT_NAME;
    database = DBConnect("sample_database");
    var tmp1 = database.SQL("select * from content where no="+course_no+" and content_no="+content_no+";");
    if(tmp1.startsWith("[")){
        elm1 = eval(tmp1);
        content_name = elm1[0].name;
        path += course_no+'/'+elm1[0].path+'/';
    }
    var tmp = database.SQL("select * from subcontent where no="+course_no+" and content_no="+content_no+" order by sub_no;");
    if(tmp.startsWith("[")){
        elm = eval(tmp);
    }
    database.DBDisConnect();
    function formatDateTime(dt) {
        // 入力された文字列を部分に分割
        year = dt.substring(0, 4);
        month = dt.substring(4, 6);
        day = dt.substring(6, 8);
        hours = dt.substring(8, 10);
        minutes = dt.substring(10, 12);
        seconds = dt.substring(12, 14);
        // フォーマットされた文字列を返す
        return year+"/"+month+"/"+day+" "+hours+":"+minutes+":"+seconds;
    }
?>
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>オンライン学習</title>
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
                    <a href="#" onclick="window.location.href='content.jss?no=<?print(course_no);?>';">コンテンツ一覧に戻る</a>
                </div>
            </div>
        </div>
        <h2><?print(content_name);?></h2>
        <div class="accordion" id="accordionPanelsStay">
            <?
            for(var i = 0 ; i<elm.length ; i++){
                connect_char = (elm[i].file.indexOf("?")>=0)?"&":"?";
                if(elm[i].file.startsWith('/')){
                    path2 = "";
                }else{
                    path2 = path;
                }
            print('<div class="accordion-item">\r\n');
            print('    <h2 class="accordion-header">\r\n');
            print('        <button class="accordion-button" type="button" data-bs-toggle="collapse" data-bs-target="#panelsStayOpen-collapse'+i+'" aria-expanded="true" aria-controls="panelsStayOpen-collapse'+i+'">');
                                   print( (i+1)+":");
                               print(htmlspecialchars(elm[i].name));
            print('        </button>\r\n');
            print('    </h2>\r\n');
            print('    <div id="panelsStayOpen-collapse'+i+'" class="accordion-collapse collapse show">\r\n');
            print('        <div class="accordion-body">');
                print('<a class="btn btn-outline-primary" href="'+path2+elm[i].file+connect_char+'no='+elm[i].no+'&contentno='+elm[i].content_no+'&subno='+elm[i].sub_no+'" target="_blank">閲覧</a>\r\n');
                if(elm[i].done == 100){
                    print('<span class="badge text-bg-info">実施完了</span>\r\n');
                    print('<span class="text-end">完了日：'+formatDateTime(elm[i].execution)+'</span>\r\n');
                }else if(elm[i].done >= 1) {
                    print('<span class="badge text-bg-warning">'+elm[i].done+'%完了</span>\r\n');
                    print('<span class="text-end">実施日：'+formatDateTime(elm[i].execution)+'</span>\r\n');
                }else{
                    print('<span class="badge text-bg-danger">未完了</span>\r\n');
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
      //      window.location.href = uri;
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