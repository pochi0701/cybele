<html>
<head>
<meta charset="utf-8">
<title>画像一覧</title>
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-9ndCyUaIbzAi2FUVXJi0CjmCapSmO7SnpJef0486qhLnuZ2cdeRhO02iuK6FUUVM" crossorigin="anonymous">
</head>
<body>
<button type="button" class="btn btn-primary" onClick="window.close();">閉じる</button><br>
<?
//画像ファイル一覧を表示するパスを指定する

path = _GET.path;
home = path.substr(_SERVER.DOCUMENT_ROOT.length,path.length-_SERVER.DOCUMENT_ROOT.length);
//print( "["+home+"]" );
home = "http://"+_SERVER.HTTP_HOST+home;

//パスの表示
print("ディレクトリ「<b>"+path+"</b>」の画像ファイル一覧<br><br>");


//降順にソートする場合はソート順に"1"を指定する
array = eval(scandir(path));

//count命令により配列の個数を取得する
num = array.length;

//HTML文を出力　テーブルの開始を指定
print("<table><tr>");

//横に並べる画像の最大数を設定する
max = 5;

//カウント数の初期化
cnt = 0;

//配列の数だけ繰り返す
for (i=0;i<num;i++){
    //filenameにァイル名を設定
    filename = home + "/"+basename(array[i]);

    //ファイル名の拡張子が｢gif｣または｢GIF｣または｢jpg｣または｢JPG｣
    //または｢JPEG｣または｢png｣または｢PNG｣の場合は実寸表示の
    //リンク付きで画像を表示する
    ext = extractFileExt(filename).toLowerCase();
    if  (ext == "gif" || ext == "jgp" || ext == "jpeg" || ext == "png" ){
        print("<td><a href=\""+filename+"\"><img src = \""+filename+"\" title=\""+filename+"\" width=\"128\" border=\"0\"/><br/>"+basename(filename)+"</a></td>");

        //カウント数の初期化
        cnt = cnt + 1;

        //カウント数の判定 最大数以上の場合は改行し、カウントを初期化する
        if (cnt >= max) {
            print("</tr><tr>");
            cnt = 0;
        }
    }
}
//HTML文を出力　テーブルの終了を指定
print("</tr></table>");

?>
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/js/bootstrap.bundle.min.js" integrity="sha384-geWF76RCwLtnZ8qwWowPQNguL3RmwHVBC9FhGdlKrxdiJJigb/j/68SIy3Te4Bkz" crossorigin="anonymous"></script>
</body>
</html>
