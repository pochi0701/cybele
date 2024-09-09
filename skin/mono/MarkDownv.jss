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
    <script src="https://cdnjs.cloudflare.com/ajax/libs/mermaid/10.9.0/mermaid.min.js" integrity="sha512-gx05X612uoaolzJ5Ee/786b3th+qzAm90TOvRwZJwnE8yr0sHQG8eLxdR0/Hbqc2IFh2mWHzzXk79g3atNSwsQ==" crossorigin="anonymous" referrerpolicy="no-referrer"></script>
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

        table, td, th {
            border: 1px #000000 solid;
        }
        table th {
            color: #fff;
            background: #88f;
        }
        table td {
	        background: #eee;
        }
        table tr:nth-child(odd) td {
	        background: #fff;
        }
        // -->
    </style>
</head>
<body>
    <div class="container">
        <input type="button" class="btn btn-primary" value="戻る" onClick="history.back();">
        <div id="md"></div>
        <script>
            let codes = [];
            // コードの抜き出し
            function pullCode(code, language) {
                const start = "```" + language;
                const st_len = start.length;
                do {
                    let stp = code.indexOf(start);
                    if (stp < 0) {
                        break;
                    }
                    let edp = code.indexOf("```", stp + st_len);
                    if (edp < 0) {
                        break;
                    }
                    // 置換用IDの作成
                    const myId = crypto.randomUUID();
                    // 置換データの作成
                    let mid = "<div class=\"mermaid\">" + code.substring(stp + st_len, edp - stp - st_len) + "</div>";
                    codes.push({ "key": myId, "value": mid });
                    code = code.substring(0, stp) + myId + code.substring(edp + 3);
                } while (true);
                return code;
            }
            // コードの書き込み
            function pushCode(code) {
                for (let count = 0; count < codes.length; count++) {
                    code = code.replace(codes[count].key, codes[count].value);
                }
                return code;
            }
            // Markdownレンダリング
            function view(url) {
                axios.get(`${url}`)
                    .then(function (response) {
                        // コンバーターを作成する
                        let converter = new showdown.Converter();
                        converter.setOption('tables', true);
                        converter.setFlavor('github');
                        // markdownをhtmlに変換する
                        let markdown = pullCode(response.data, "mermaid");
                        let html = converter.makeHtml(markdown);
                        let mydiv = document.getElementById("md");
                        // mermaidを実施して完了
                        mydiv.innerHTML = pushCode(html);
                        mermaid.init();
                        return false;
                    })
                    .catch(function (error) {
                        alert("コンテンツはありません。");
                        console.log(error);
                    });
            }
            // 指定したファイルをサーバーから取得
            view("<?print(url);?>");
        </script>
</body>
</html>
