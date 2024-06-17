<?
    shutdown(_POST.password);
?>
<!doctype html>
<html lang="ja">
<head>
  <meta charset="utf-8">
  <title>SHUTDOWN SYSTEM</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-9ndCyUaIbzAi2FUVXJi0CjmCapSmO7SnpJef0486qhLnuZ2cdeRhO02iuK6FUUVM" crossorigin="anonymous">
  <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/js/bootstrap.bundle.min.js" integrity="sha384-geWF76RCwLtnZ8qwWowPQNguL3RmwHVBC9FhGdlKrxdiJJigb/j/68SIy3Te4Bkz" crossorigin="anonymous"></script>
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

