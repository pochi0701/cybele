<!doctype html>
<html lang="ja">
<head>
    <meta charset="utf-8">
    <title>環境変数一覧</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-rbsA2VBKQhggwzxH7pPCaAqO46MgnOM80zW1RWuH61DGLwZJEdK2Kadq2F9CUG65" crossorigin="anonymous">
</head>
<body>
    <!-- 1.ナビゲーションバーの設定 -->
    <nav class="navbar navbar-expand-lg navbar-dark bg-dark sticky-top">
        <div class="container-fluid">
            <a class="navbar-brand" href="/" target="_top">Cybele</a>
            <button class="navbar-toggler" type="button" data-bs-toggle="collapse" data-bs-target="#navbarSupportedContent" aria-controls="navbarSupportedContent" aria-expanded="false" aria-label="Toggle navigation">
                <span class="navbar-toggler-icon"></span>
            </button>
            <div class="collapse navbar-collapse flex-grow-1 text-right" id="myNavbar">
                <ul class="navbar-nav ms-auto flex-nowrap">
                    <li><a href="/" class="nav-link m-2 menu-item" target="_top">Home</a></li>
                </ul>
            </div>
        </div>
    </nav>
    <div class="container">
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
    </div>
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/js/bootstrap.bundle.min.js" integrity="sha384-kenU1KFdBIe4zVF0s0G1M5b4hcpxyD9F7jL+jjXkk+Q2h455rYXK/7HAuoJl+0I4" crossorigin="anonymous"></script>
</body>
</html>    
