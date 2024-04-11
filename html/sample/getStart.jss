<?
    email = _POST.email;
    passwd = _POST.password;
    if(email != undefined && passwd != undefined )
    {
        var dat = loadFromFile("http://neon.cx/market/download.php?email="+email+"&password="+passwd);
        saveToFile("./lesson2.tgz",dat);
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
    <h2>レッスン教材ダウンロード</h2>
    <form method="post">
        <div class="row">
            <div class="mb-3 col-md-4">
                <label for="email" class="form-label">メールアドレス</label>
                <input type="email" class="form-control" id="email" name="email" placeholder="登録したメールアドレスを入力してください" required>
            </div>
       </div>
       <div class="row">
            <div class="mb-3 col-md-4">
                <label for="password" class="form-label">パスワード</label>
                <input type="password" class="form-control" id="password" name="password" placeholder="登録したパスワードを入力してください" required>
            </div>
        </div>
        <button type="submit" class="btn btn-primary">ダウンロード</button>
    </form>
</div>
    <!-- Bootstrap Bundle with Popper -->
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/js/bootstrap.bundle.min.js"></script>
</body>
</html>