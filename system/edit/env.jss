<html>
<head><title>環境変数一覧</title></head>
<body>
<pre>
<?
   print( "_SERVER\n\n");
   var s = Object.keys(_SERVER);  
   for( var i=0 ; i<s.length ; i++ ){
         print( "["+s[i]+"] =>"+_SERVER[s[i]]+"\n" );
   } 
   print( "\n_GET<br/>\n\n" );
   s = Object.keys(_GET);  
   for( var i=0 ; i<s.length ; i++ ){
         print( "["+s[i]+"] =>"+_GET[s[i]]+"\n" );
   }
   print( "\n_POST<br/>\n\n" );
   s = Object.keys(_POST);  
   for( var i=0 ; i<s.length ; i++ ){
         print( "["+s[i]+"] =>"+_POST[s[i]]+"\n" );
   }
?>
</pre>
</body>
</html>
