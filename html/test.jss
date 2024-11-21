<html>
<head>
    <meta charset="utf-8">
</head>    
<?

   //var patterns = '/\\$/';
   //var replacements = "&DOLLAR;";
   //var string = 'aaaa\\$bbbbb';
   //var bbb = string.preg_replace(patterns,replacements);
   
   //var patterns = '/\\{\\{([^{]*)\\}\\}/';
   //var replacements = '\"+image( \"$1\" )+\"';
   //var string = 'aa{{test.jpg|aaaa}}aa';
   
   var patterns = '/JAPAN|NIPPON/';
   var replacements = '日本';
   var string = 'ようこそJAPANへ';

   var bbb = string.preg_replace(patterns,replacements);
   print(string+"<br/>");
   print(bbb);
?>
</html>
