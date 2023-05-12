<!doctype html>
<html lang="ja">
<head>
  <meta charset="utf-8">
  <title>動画授業</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-rbsA2VBKQhggwzxH7pPCaAqO46MgnOM80zW1RWuH61DGLwZJEdK2Kadq2F9CUG65" crossorigin="anonymous">
</head>
<body>
  <div class="text-center">
  <h1>動画授業</h1>
  </div>
  <div class="container">
      <div class="row">
      SQLを使って動画授業とその進行状況を保存します。<br>
      生徒一覧から名前をクリックするかQRコードを参照すると一回分の授業を受けたと想定します。<br>
      QRコードは同じLAN内にあるスマートフォンやタブレットで参照してください。
      Add Student,Del Studentで生徒の追加削除ができます。
      簡単なサンプルなので、エラー処理、構成等はしっかり考えていません<br>
      実行メニュー
      </div>
      <div class="row">
      <a href="list.jss">生徒一覧からの動画授業</a><br>
      <a href="add.jss">受講生徒追加</a><br>
      <a href="del.jss">受講生徒削除</a><br>
      </div>

  </div>
  <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/js/bootstrap.bundle.min.js" integrity="sha384-kenU1KFdBIe4zVF0s0G1M5b4hcpxyD9F7jL+jjXkk+Q2h455rYXK/7HAuoJl+0I4" crossorigin="anonymous"></script>
</body>
</html>
