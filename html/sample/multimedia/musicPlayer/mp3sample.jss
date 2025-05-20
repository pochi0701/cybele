<!DOCTYPE html>
<html lang="ja">
<head>
    <meta charset="utf-8">
    <title>menu</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-9ndCyUaIbzAi2FUVXJi0CjmCapSmO7SnpJef0486qhLnuZ2cdeRhO02iuK6FUUVM" crossorigin="anonymous">
    <link rel="stylesheet" type="text/css" href="index.css">
</head>
<body>
MP3ファイルから内容を取り出して表示します。
<?
    var file="/sample/multimedia/musicPlayer/mailmonster.mp3";
    var name = JSON.mp3id3tag(_SERVER["DOCUMENT_ROOT"]+file);
    var music = eval(name);
    print('ファイル：'+file+"<br>\r\n");
    print('<table class="table">');
    print('<thead>');
    print(' <tr>');
    print('  <th>#</th>');
    print('  <th scope="col">タイトル</th>');
    print('  <th scope="col">アルバム</th>');
    print('  <th scope="col">作成年</th>');
    print('  <th scope="col">コメント</th>');
    print(' </tr>');
    print(' </thead>');
    print(' <tbody>');
    print(' <tr>');
    print('  <th scope="row">1</th>');
    print('  <td>'+music.title+'</td>');
    print('  <td>'+music.album+'</td>');
    print('  <td>'+music.year+'</td>');
    print('  <td>'+music.comment+'</td>');
    print(' </tr>');
    print(' </tbody>');
    print('</table>');
?>
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/js/bootstrap.bundle.min.js" integrity="sha384-geWF76RCwLtnZ8qwWowPQNguL3RmwHVBC9FhGdlKrxdiJJigb/j/68SIy3Te4Bkz" crossorigin="anonymous"></script>
</body>
</html>
