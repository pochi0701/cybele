<?
    session_start();
    if( _SESSION["uid"] != undefined ){
        uid = _SESSION["uid"];
        pwd = _SESSION["pwd"];
        grd = _SESSION["grd"];
        unm = _SESSION["unm"];
    }
    var database;
    database = DBConnect("lesson");
    //student一覧
    var student=eval(database.SQL("select * from student;"));
    database.DBDisConnect();
?>
<!doctype html>
<html lang="ja">
<head>
  <meta charset="utf-8">
  <title>menu</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-0evHe/X+R7YkIZDRvuzKMRqM+OrBnVFBL6DOitfPri4tjfHxaWutUpFmBp4vmVor" crossorigin="anonymous">
  <script src="http://code.jquery.com/jquery-1.12.0.min.js"></script>
  <script src="https://cdnjs.cloudflare.com/ajax/libs/jquery.qrcode/1.0/jquery.qrcode.min.js"></script> 
</head>
<body>
<div class="text-center">
<h1>all student</h1>
</div>
<div class="container">
    <table class="table table-striped table-bordered table-hover">
      <thead><tr><th>ID</th><th>Grade</th><th>Student</th><th>link</th><th>QR Code</th></tr></thead>
      <tbody>
<?
      //list作成
      for( var i = 0 ; i < student.length ; i++ ){
          print("<tr><td>"+student[i]["id"]+"</td>");
          print("<td>"+student[i]["grade"]+"</td>");
          print("<td>"+student[i]["name"]+"</td>\n");
          //print( "<td></td></tr>\n");
          print("<td><a href=\"./lesson/lesson.jss?unm="+encodeURI(student[i]["name"])+"\">"+student[i]["name"]+"</a></td>\n");
          print("<td><div id=\"qrcodeCanvas"+i+"\"></div></td></tr>\n");
      }
      //終わり
?>
      </tbody>
    </table>
</div>
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/js/bootstrap.bundle.min.js" integrity="sha384-pprn3073KE6tl6bjs2QrFaJGz5/SUsLqktiwsUTF55Jfv3qYSDhgCecCxMW52nD2" crossorigin="anonymous"></script>
</body>
<script>
<?
      for( var i = 0 ; i < student.length ; i++ ){
  	      //print( "jQuery('#qrcodeCanvas1').qrcode({text:\"http://127.0.0.1:8000/user/lesson.jss?unm="+encodeURI(student[i]["name"])+"\"});" );	
  	      print( "jQuery('#qrcodeCanvas"+i+"').qrcode({width: 96, height: 96, text:\"http://"+getLocalAddress()+":8000/user/lesson/lesson.jss?unm="+encodeURI(student[i]["name"])+"\"});\n" );	
      }
?>

</script>
</html>
