<?
    session_start();
    var uid;
    var unm;
    var pwd;
    var grd;
    var database;
    database = DBConnect("lesson");
    if( _GET.unm != undefined ){
        unm = _GET.unm;
        pwd = _GET.pwd;
        _SESSION["unm"]=unm;
        _SESSION["pwd"]=pwd;
        var elm = eval(database.SQL("select * from student where name=\""+unm+"\";"));
        if( elm != undefined ){
            uid = elm[0].id;
            grd = elm[0].grade;
            _SESSION["uid"]=uid;
            _SESSION["grd"]=grd;
        }else{
            header("Location: uncertification.html");
        }
    }else if( _SESSION["uid"] != undefined ){
        uid = _SESSION["uid"];
        unm = _SESSION["unm"];
        pwd = _SESSION["pwd"];
        grd = _SESSION["grd"];
    }else{
        header("Location: uncertification.html");
    }
    var list = eval(database.SQL("select lesson.id as id,lesson.grade as grade,lesson.title as title,movie.title as movie,lesson.movieid as mid from lesson,movie where grade="+grd+" and lesson.movieid=movie.id"));
    //id,grade,title,movie(filename),movieid
    for( var i = 0 ; i < list.length ; i++){
        //var ss = database.SQL("select count(*) as times from history where movie="+list[i]["mid"]+" and uid="+uid+";");
        //print( ss );
        var cnt = eval(database.SQL("select count(*) as times from history where movie="+list[i]["mid"]+" and uid="+uid+";"));
        //cnt.dump();
        list[i]["cnt"]=cnt["times"];
    }
    database.DBDisConnect();
    //var list = eval(loadFromCSV("./lesson.csv"));
    //list.remove(list[0]);
?>
<!DOCTYPE html>
<html lang="ja">
<head>
    <meta charset="utf-8">
    <title>movie lesson</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-rbsA2VBKQhggwzxH7pPCaAqO46MgnOM80zW1RWuH61DGLwZJEdK2Kadq2F9CUG65" crossorigin="anonymous">
    <script src="http://code.jquery.com/jquery-1.12.0.min.js"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/jquery.qrcode/1.0/jquery.qrcode.min.js"></script>
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
  <div class="text-center">
  <h1><? print("Welcome "+unm+" to movie lesson!"); ?></h1>
  </div>
  <div class="container">
    <table class="table table-striped table-bordered table-hover">
      <thead><tr><th>ID</th><th>Grade</th><th>Contents</th><th>view times</th></tr></thead>
      <tbody>
<?
      //list作成

      for( i = 0 ; i < list.length ; i++ ){
        if( grade == 0 || list[i]["grade"] == grade ){
          print("<tr><td>"+list[i]["id"]+"</td>");
          print("<td>"+list[i]["grade"]+"</td>");
          print("<td><a href=\"./preview.jss?url=%2fsample%2fmultimedia%2f"+list[i]["movie"]+"&mid="+list[i]["mid"]+"\">"+list[i]["title"].replaceAll("\n","<br/>")+"</a></td>\n");
          print("<td>"+list[i]["cnt"]+"</td></tr>\n");
        }
      }
      //終わり
?>
      </tbody>
    </table>
  </div>
  <!-- ===== copyright ===== -->
  <div class="copyright">
    <p class="text-center">
      Copyright(c) 2015 <a href="http://www.birdland.co.jp">Birdland Ltd.</a> All Rights Reserved.
    </p>
  </div>
  <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/js/bootstrap.bundle.min.js" integrity="sha384-kenU1KFdBIe4zVF0s0G1M5b4hcpxyD9F7jL+jjXkk+Q2h455rYXK/7HAuoJl+0I4" crossorigin="anonymous"></script>
</body>
</html>

