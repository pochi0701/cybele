<?
url = _GET.url;
?>
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title><? print(basename(url)); ?></title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-9ndCyUaIbzAi2FUVXJi0CjmCapSmO7SnpJef0486qhLnuZ2cdeRhO02iuK6FUUVM" crossorigin="anonymous">
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/js/bootstrap.bundle.min.js" integrity="sha384-geWF76RCwLtnZ8qwWowPQNguL3RmwHVBC9FhGdlKrxdiJJigb/j/68SIy3Te4Bkz" crossorigin="anonymous"></script>
    <script src="https://cdn.jsdelivr.net/npm/showdown@2.1.0/dist/showdown.min.js"></script>
    <script src="https://cdn.jsdelivr.net/npm/axios/dist/axios.min.js"></script>
    <style type="text/css" media="screen">
      <!--
        pre {
          margin: 1em 0; /* ブロック前後の余白 */
          padding: 1em; /* ブロック内の余白 */
          border-radius: 5px; /* 角丸 */
          background: #25292f; /* 背景色 */
          color: #fff; /* 文字色 */
          white-space: pre-wrap; /* はみ出たときに折り返す */
        }
      //-->
    </style>
</head>
<body>
    <div class="container">
        <input type="button" class="btn btn-primary" value="戻る" onClick="history.back();">
        <div id="md"></id>
    </div>
    <script>
        // tree view
        function view(url) {
            axios.get(`${url}`)
                .then(function (response) {
                    // コンバーターを作成する
                    var converter = new showdown.Converter();
                    converter.setOption('tables', true);
                    // markdownをhtmlに変換する
                    var markdown = response.data;
                    var html = converter.makeHtml(markdown);
                    var mydiv = document.getElementById("md");
                    mydiv.innerHTML = html;
                    return false;
                })
                .catch(function (error) {
                    console.log(error);
                });
        }
        view("<?print(url);?>");
     </script>
</body>
</html>
