<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>アイコン作成（URLアイコン）</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-9ndCyUaIbzAi2FUVXJi0CjmCapSmO7SnpJef0486qhLnuZ2cdeRhO02iuK6FUUVM" crossorigin="anonymous">
</head>
<body>
<?
    if(_SERVER.OS=="WINDOWS"){
        var root = _SERVER.DOCUMENT_ROOT;
        root = dirname(root).replaceAll('/','\\');
        var http = "http://"+getLocalAddress()+":"+getLocalPort()+'/';
        var data = '[InternetShortcut]\r\nURL='+http+'\r\nIconIndex=0\r\nHotKey=0\r\nIDList=\r\nIconFile='+root+'\\system\\favicon.ico';
        //print(data);
        saveToFile("%USERPROFILE%/Desktop/test.url",data.nkfconv("Ws"));
        print("デスクトップにURLアイコンを作成しました。");
    }else{
        print("linuxではアイコン作成をサポートしていません。");
    }
?>
</body>
</html>