<?
    session_start();
    if( _SESSION["uid"] != undefined ){
        uid = _SESSION["uid"];
        pwd = _SESSION["pwd"];
        grd = _SESSION["grd"];
        unm = _SESSION["unm"];
    }
    //if( uid == undefined ){
    //   header("Location: http://neon.cx/");
    //}
    var database;
    database = DBConnect("lesson");
    //student一覧
    var student=eval(database.SQL("select * from student;"));
    database.DBDisConnect();
?>
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <title>movie</title>
  <meta http-equiv="Pragma" content="no-cache">
  <meta http-equiv="Cache-Control" content="no-cache">
  <meta http-equiv="Expires" content="0">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link href="http://netdna.bootstrapcdn.com/bootstrap/3.3.6/css/bootstrap.min.css" rel="stylesheet">
  <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/font-awesome/4.5.0/css/font-awesome.min.css">
  <script src="http://code.jquery.com/jquery-1.12.0.min.js"></script>
  <script src="http://netdna.bootstrapcdn.com/bootstrap/3.3.6/js/bootstrap.min.js"></script>
  <script type="text/javascript" src="../src/jquery.qrcode.js"></script>
  <script type="text/javascript" src="../src/qrcode.js"></script>
  <style>
//   body {
//      color:#fff !important;
//      background:#000 !important;
//   }
   @media (min-width:767px){
     .dropdown:hover > .dropdown-menu{
       display: block;
     }
   }
  </style>
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
          print("<td><a href=\"./lesson.jss?unm="+encodeURI(student[i]["name"])+"\">"+student[i]["name"]+"</a></td>\n");
          print("<td><div id=\"qrcodeCanvas"+i+"\"></div></td></tr>\n");
      }
      //終わり
?>
      </tbody>
    </table>
</div>
</body>
<script>
<?
      for( var i = 0 ; i < student.length ; i++ ){
  	      //print( "jQuery('#qrcodeCanvas1').qrcode({text:\"http://127.0.0.1:8000/user/lesson.jss?unm="+encodeURI(student[i]["name"])+"\"});" );	
  	      print( "jQuery('#qrcodeCanvas"+i+"').qrcode({text:\"http://"+getLocalAddress()+":8000/user/lesson.jss?unm="+encodeURI(student[i]["name"])+"\"});\n" );	
      }
?>

</script>
</html>
