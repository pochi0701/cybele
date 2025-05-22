<?
    var version = _GET.version;
    var email = _POST.email;
    var contid = _POST.contentid;
    var termid = biosUUID();
    var script = _SERVER.SCRIPT_FILENAME;
    var errmsg = "";
    if(contid !== undefined && termid !== undefined )
    {
        base = script;
        baseptr = base.length-1;
        while( baseptr >= 0 && base.substring(baseptr,baseptr+1) != '/' ){
            baseptr -= 1;
        }
        baseptr++;
        var base = base.substring(0,baseptr);
        var dat = loadFromFile("http://neon.cx/market/download2.php?version="+version+"&contid="+contid+"&termid="+termid);
        if( dat != "none" ){
          saveToFile(base+"lesson.tgz",dat);
          command("tar zxvf "+base+"lesson.tgz");
          unlink( base+"lesson.tgz");
          header("Location: /school/");
        }else{
          errmsg = "このコンテンツは購入してません。";
        }
    }
?>
<!DOCTYPE html>
<html lang="ja">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>レッスン教材をダウンロードします</title>
    <!-- Bootstrap CSS -->
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css" rel="stylesheet">
    <script src="https://cdn.jsdelivr.net/npm/axios/dist/axios.min.js"></script>
</head>
<body>
<div class="container mt-5">
    <input type="button" class="btn btn-primary" value="戻る" onClick="location.href='index.jss'">
    <h2>レッスン教材ダウンロード</h2>
    <p id="error" class="text-danger"></p>
    <form method="post">
       <div class="row">
            <div class="mb-3 col-md-4">
                <label for="password" class="form-label">コンテンツID</label>
                <input type="password" class="form-control" id="contentid" name="contentid" placeholder="コンテンツIDを入力してください" required>
            </div>
        </div>
        <button type="submit" class="btn btn-primary">ダウンロード</button>
    </form>
</div>
    <!-- Bootstrap Bundle with Popper -->
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/js/bootstrap.bundle.min.js"></script>
    <script>
       var errmsg = "<?print(errmsg);?>";
       if( errmsg.length > 0 ){
           const err = document.getElementById("error");
           err.innerText = errmsg;
       }
    </script>
</body>
</html>