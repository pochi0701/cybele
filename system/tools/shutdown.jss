<?
    shutdown(_POST.password);
?>
<!DOCTYPE html>
<html lang="ja">
<head>
  <meta charset="utf-8">
  <title>SHUTDOWN SYSTEM</title>
  <meta http-equiv="Pragma" content="no-cache">
  <meta http-equiv="Cache-Control" content="no-cache">
  <meta http-equiv="Expires" content="0">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link href="http://netdna.bootstrapcdn.com/bootstrap/3.3.6/css/bootstrap.min.css" rel="stylesheet">
  <link href="themes/standard/common.css" rel="stylesheet">
  <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/font-awesome/4.5.0/css/font-awesome.min.css">
  <script src="http://code.jquery.com/jquery-1.12.0.min.js"></script>
  <script src="http://netdna.bootstrapcdn.com/bootstrap/3.3.6/js/bootstrap.min.js"></script>
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

