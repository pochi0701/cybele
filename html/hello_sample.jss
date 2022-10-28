<html>
<head>
<script>
    localStorage.setItem('abc', 'aaa');
    var b = localStorage.getItem('abc');
</script>
</head>
<body>
  <?
       print("HELLO WORLD!");
  ?>
<script>
alert(b);
</script>
</body>
</html>
