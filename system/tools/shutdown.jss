<?
    shutdown(_POST.password);
?>
<!doctype html>
<html lang="ja">
<head>
  <meta charset="utf-8">
  <title>SHUTDOWN SYSTEM</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-rbsA2VBKQhggwzxH7pPCaAqO46MgnOM80zW1RWuH61DGLwZJEdK2Kadq2F9CUG65" crossorigin="anonymous">
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.1.1/css/all.min.css">
  <script src="http://code.jquery.com/jquery-1.12.0.min.js"></script>
  <script src="https://cdnjs.cloudflare.com/ajax/libs/jquery.qrcode/1.0/jquery.qrcode.min.js"></script>
</head>
<body>
  <div class="text-center">
  <h1>システムシャットダウン：パスワードを入力してください。</h1>
  </div>
  <div class="container">
    <form class="form-horizontal col-sm-4 col-sm-offset-4" role="form" method="post" action="">
    <div class="form-group">
        <label class="sr-only" for="name">シャットダウンパスワード</label>
        <input name="password" id="age" type="password" class="form-control" placeholder="Password" />
    </div>
    <div class="form-group">
        <button type="submit" name="submit" value="submit" class="btn btn-primary">シャットダウン</button>
    </div>
    </form>
  </div>
</body>
</html>

