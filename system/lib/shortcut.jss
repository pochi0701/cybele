<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>アイコン作成（ショートカットアイコン）</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
</head>
<body>
<?
    if(_SERVER.OS=="WINDOWS"){
        var base = dirname(_SERVER.DOCUMENT_ROOT);
        var root = base.replaceAll('/','\\');
        var msg = '$WshShell = New-Object -ComObject WScript.Shell\r\n';
        msg    += '$Shortcut = $WshShell.CreateShortcut("$env:UserProfile\\Desktop\\Cybele.lnk")\r\n';
        msg    += '$Shortcut.TargetPath = "'+root+'\\Cybele.exe"\r\n';
        msg    += '$Shortcut.IconLocation = "'+root+'\\system\\common\\execute.ico"\r\n';
        msg    += '$Shortcut.Save()\r\n';
        var target = base+'/shortcut.ps1';
        saveToFile(target,msg);
        command('powershell -NoProfile -ExecutionPolicy Bypass -File '+target);
        print("デスクトップにショートカットアイコンを作成しました。");
    unlink(target);
    }else{
        print("linuxではアイコン作成をサポートしていません。");
    }
?>
</body>
</html>