<!DOCTYPE html>
<html lang="ja">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>upload</title>
  <style>
    div{
      margin-bottom: 20px;
    }
  </style>
</head>
<body>
<?
   s = Object.keys(_POST);  
   for( var i=0 ; i<s.length ; i++ ){
         print( "["+s[i]+"] =>"+_POST[s[i]]+"<br>\n" );
   }
   if( _POST.upload != undefined )
   {
       print( "<br>\nファイル名：" );
       print( _POST.upload.filename);
       print( "<br>\nファイル長さ：" );
       print( _POST.upload.length);
       print( "<br>\nファイル内容：" );
       print( btoa(_POST.upload.filebody));
   }
?>
<div>
  <form action="upload.jss" method="post" enctype="multipart/form-data" >
    <div>
      <input type="text" name="title" value="">
    </div>
    <div>
      <input type="text" name="title2" value="">
    </div>
    <div>
      <input type="file" name="upload">
    </div>
    <div>
      <input type="submit" value="送信する">
    </div>
  </form>
</div>
</body>
</html>