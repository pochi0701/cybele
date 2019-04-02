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
            header("Location: http://www.yahoo.co.jp/");
        }
    }else if( _SESSION["uid"] != undefined ){
        uid = _SESSION["uid"];
        unm = _SESSION["unm"];
        pwd = _SESSION["pwd"];
        grd = _SESSION["grd"];
    }else{
        header("Location: http://neon.cx/");
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
  <meta http-equiv="Pragma" content="no-cache">
  <meta http-equiv="Cache-Control" content="no-cache">
  <meta http-equiv="Expires" content="0">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link href="http://netdna.bootstrapcdn.com/bootstrap/3.3.6/css/bootstrap.min.css" rel="stylesheet">
  <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/font-awesome/4.5.0/css/font-awesome.min.css">
  <style>
    @media (min-width:767px){
      .dropdown:hover > .dropdown-menu{
        display: block;
      }
    }
  </style>
  <script src="http://code.jquery.com/jquery-1.12.0.min.js"></script>
  <script src="http://netdna.bootstrapcdn.com/bootstrap/3.3.6/js/bootstrap.min.js"></script>
  </style>
</head>
<body style="padding-top:53px;">
  <div class="container">
    <!-- 1.ナビゲーションバーの設定 -->
    <nav class="navbar navbar-inverse navbar-fixed-top">
      <!-- 2.ヘッダ情報 -->
      <div class="navbar-header">
        <button type="button" class="navbar-toggle" data-toggle="collapse" data-target="#nav-menu-1">
          <span class="sr-only">Toggle navigation</span>
          <span class="icon-bar"></span>
          <span class="icon-bar"></span>
          <span class="icon-bar"></span>
        </button>
        <a class="navbar-brand" href="#">movie lesson</a>
      </div>
      <!-- 3.リストの配置 -->
      <div class="collapse navbar-collapse" id="nav-menu-1">
        <ul class="nav navbar-nav">
          <li ><a href="index.jss">TOP</a></li>
          <li class="dropdown">
            <a href="#" class="dropdown-toggle" data-toggle="dropdown">
              <span class="link-menu">Drop down<b class="caret"></b></span>
            </a>
            <ul class="dropdown-menu">
              <li><a href="index.jss">固定ページ</a></li>
              <li><a href="index.jss">サイドバー</a></li>
              <li><a href="index.jss">test1</a></li>
           </ul>
          </li>
          <li ><a href="index.jss">test2</a></li>
          <li ><a href="/">ROOT Page</a></li>
        </ul>
      </div>
    </nav>
  </div>
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
          print("<td><a href=\"./preview.jss?url=%2fuser%2f"+list[i]["movie"]+"&mid="+list[i]["mid"]+"\">"+list[i]["title"].replace("\n","<br/>")+"</a></td>\n");
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
  <!-- jQuery (necessary for Bootstrap's JavaScript plugins) -->
  <!-- Include all compiled plugins (below), or include individual files as needed -->
  <script type="text/javascript">
  jQuery(document).ready(function ($) {
    $('.dropdown-toggle').click(function(e) {
      // 要素で親メニューリンクとドロップダウンメニュー表示を切り分ける
      if ($(e.target).hasClass('link-menu')) {
        var location = $(this).attr('href');
        window.location.href = location;
        return false;
      }
      return true;
    });
  });
  </script>
</body>
</html>

