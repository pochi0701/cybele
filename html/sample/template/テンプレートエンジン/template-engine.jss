<?
   var data = "ddd";
   var dat = loadFromFile("template.html");
   //var dat2 = dat.addSlashes();
   var html = eval("\""+dat+"\"");
   print(html);
?>