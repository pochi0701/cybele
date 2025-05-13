<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>アイコン作成（URLアイコン）</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
</head>
<body>
<?
    if(_SERVER.OS=="WINDOWS"){
        var base = dirname(_SERVER.DOCUMENT_ROOT);
        var root = base.replaceAll('/','\\');
        var root = _SERVER.DOCUMENT_ROOT;
        root = dirname(root).replaceAll('/','\\');
        var http = "http://"+getLocalAddress()+":"+getLocalPort()+'/';
        var data = '[InternetShortcut]\r\nURL='+http+'\r\nIconIndex=0\r\nHotKey=0\r\nIDList=\r\nIconFile='+root+'\\system\\common\\favicon.ico';
        //print(data);
        saveToFile("%USERPROFILE%/Desktop/Cybele.url",data.nkfconv("Ws"));
        print("デスクトップにURLアイコンを作成しました。");
    }else{
        print("linuxではアイコン作成をサポートしていません。");
    }
?>
</body>
</html>