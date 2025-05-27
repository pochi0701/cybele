<?
//変数取得
var base    = _SERVER.DOCUMENT_ROOT;
var root    = _GET.root;
var scriptp  = _SERVER.SCRIPT_FILENAME;
var scriptn  = _SERVER.SCRIPT_NAME;
//ユーザ限定処理
//初回の処理
if( root.length==0){
    root = _SERVER.DOCUMENT_ROOT;
}
//右端の/をなくす
while( root[root.length-1] == "/" ){
    root = root.substring(0,root.length-1);
}
var sf = root.substring(base.length,root.length);
if( sf == ""){
   sf = "/";
}
?>
<!doctype html>
<html lang="ja">
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Online Editor</title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-9ndCyUaIbzAi2FUVXJi0CjmCapSmO7SnpJef0486qhLnuZ2cdeRhO02iuK6FUUVM" crossorigin="anonymous">
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.1.1/css/all.min.css">
    <script src="https://cdn.jsdelivr.net/npm/axios/dist/axios.min.js"></script>
    <script src="https://cdn.jsdelivr.net/npm/showdown@2.1.0/dist/showdown.min.js"></script>
    <script src="jss-mode.js"></script>
    <style type="text/css" media="screen">
      <!--
      html,body { height:100%; }
      .nav-link {
        height: 28px;
        padding-top: 0;
        color: white;
        background-color: #a0a0ff;
      }
      .nav-link.active {
        color: white !important;
        background-color: #ffa0a0 !important;
      }
      #editArea {
          position: relative;
          //border: 1px solid lightgray;
          height: 100%;
          width: 100%;
      }
      //-->
    </style>
</head>
<body onload="init();">
    <button class="btn btn-primary" type="button" data-bs-toggle="offcanvas" data-bs-target="#offcanvasWithBothOptions" aria-controls="offcanvasWithBothOptions">Ⲷ</button>
    <!-- OFF CANVAS -->
    <div class="offcanvas offcanvas-start" data-bs-scroll="true" tabindex="-1" id="offcanvasWithBothOptions" aria-labelledby="offcanvasWithBothOptionsLabel">
        <div class="offcanvas-header">
            <h5 class="offcanvas-title" id="offcanvasWithBothOptionsLabel">Folder view</h5>
            <button type="button" class="btn-close" data-bs-dismiss="offcanvas" aria-label="Close"></button>
        </div>
        <div class="offcanvas-body">
            <div class="container">
                <div class="dropdown">
                    <button class="btn btn-default btn-outline-primary dropdown-toggle btn-small" type="button" data-bs-toggle="dropdown" aria-expanded="false">
                        Menu<span class="caret">
                    </button>
                    <ul class="dropdown-menu">
                        <li><a class="dropdown-item" href="#" onClick="createDir();">フォルダ作成</a></li>
                        <li><a class="dropdown-item" href="#" onClick="createFile();">ファイル作成</a></li>
                        <li><a class="dropdown-item" href="/system/lib/lib.html" target="_lib">ライブラリ</a></li>
                        <li><a class="dropdown-item" href="#" onClick="dl();">ダウンロード</a></li>
                        <li><a class="dropdown-item" href="#" onClick="viewImages();">画像一覧</a></li>
                        <li><a class="dropdown-item" href="#" onClick="expt();return false;">エクスポート</a></li>
                        <li><a class="dropdown-item" href="/" target="_top">Home</a></li>
                    </ul>
                </div>
                <div id="tree"><p></p></div>
            </div>
        </div>
    </div>
    <!-- EDITOR -->
    <a href="#" onclick="saveCode();" title="Save File(AltS)" accesskey="S"><i class="fa fa-cloud-upload fa-lg"></i></a>
    <a href="#" onclick="viewCode(filepath);" title="View File(AltV)" accesskey="V"><i class="fa fa-television fa-lg"></i></a>
    <a href="#" onclick="myClose();" title="close(AltC)" accesskey="C"><i class="fa fa-times fa-lg"></i></a>
    <div id="searchLists"></div>
    <div id="markDown"></div>    
    <div id="editArea"></div>

    <!-- SCRIPTS //-->
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/js/bootstrap.bundle.min.js" integrity="sha384-geWF76RCwLtnZ8qwWowPQNguL3RmwHVBC9FhGdlKrxdiJJigb/j/68SIy3Te4Bkz" crossorigin="anonymous"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/ace/1.37.5/ace.min.js" integrity="sha512-YjQHlQGNexP37C52f9apwLx+lnJXu8Nbk/812nUhqZkaHPIiOCC3Jg0Q8lL7cZpLpMvFlLIxV8fgQ9fZCjqaWQ==" crossorigin="anonymous" referrerpolicy="no-referrer"></script>
    <script type="text/javascript">
        const extractFileExt = str => str.slice(str.lastIndexOf("."));
        const extractFileName = str => str.slice(str.lastIndexOf("/") + 1);
        const extractFilePath = str => str.substring(0, str.lastIndexOf("/"));
        const document_root = "<?print(_SERVER.DOCUMENT_ROOT);?>";
        var editor;
        var filepath;
        var lastIndexed;
        var converter;
        var root = "<? print( root ); ?>";
        var base = "<? print( base ); ?>";
        var scriptp = "<? print( scriptp ); ?>";
        scriptp = extractFilePath(scriptp)+"/";
        var scriptn = "<? print( scriptn ); ?>";
        scriptn = extractFilePath(scriptn)+"/";
        function setEditor(code,path)
        {
            // editorの設定
            if (this.editor === undefined) {
                document.getElementById("editArea").innerHTML = "";
                this.editor = ace.edit("editArea");
                this.editor.setTheme("ace/theme/textmate");
            }
            document.getElementById("markDown").innerHTML = "";
            this.editor.container.style.display = "";
            this.editor.setValue(code,-1);
            //このコードのタイプを設定
            this.editor.getSession().setMode(getFileType(path));
            this.filepath = path;
        }
        function init() {
            // list取得[{"filepath":filepath,"modify":false/true}]
            elm = getElements();
            this.lastIndexed = (elm.length>0)?0:-1;
            if(this.lastIndexed>=0){
                // TAB表示
                showTab(elm, elm[this.lastIndexed].filepath);
                // EDITOR表示
                setEditor(elm[this.lastIndexed].code,elm[this.lastIndexed].filepath);
                 // Tree表示
                treeView("?root=" + extractFilePath(elm[this.lastIndexed].filepath));
            }else{
                showMarkDown(`${scriptp}common/develop.md`);
                treeView("");
            }
            return false;
        }
        // display result of rendering code.
        function viewCode(path) {
            if(path === undefined){
                return;
            }
            checkEditData();
            if(this.lastIndexed>=0){
                let elm = getElements();
                if(elm[this.lastIndexed].modify){
                    if( window.confirm(extractFileName(elm[this.lastIndexed].filepath)+"は変更されています。保存しますか？") ){
                        saveCode();
                    }
                }
            }
            if (path.endsWith('md')) {
                path = "http://<?print(_SERVER.HTTP_HOST);?>" + path.substring(document_root.length, path.length) + "?action=MarkDown.jss";
            } else {
                path = "http://<?print(_SERVER.HTTP_HOST);?>" + path.substring(document_root.length, path.length);
            }
            myWin = window.open(encodeURI(path), path);
        }
        function QRCode(path) {
            let path2 = path.substring(document_root.length, path.length);
            path = `${scriptn}lib/qrcode.jss?url=${path2}`;
            myWin = window.open(encodeURI(path), path);
        }
        function openDir(url) {
            root = url;
            treeView("?root=" + url);
            return false;
        }
        function renameFile(uri, file) {
            urie = uri + (uri.endsWith("/") ? "" : "/");
            msg = window.prompt("変更するファイル名を入力してください", file);
            if (msg != null && window.confirm(file + "を" + msg + "に変更します。よろしいですか？")) {
                treeView("?root=" + uri + "&renf=" + urie + encodeURI(file) + "&rent=" + urie + encodeURI(msg));
            }
            return false;
        }
        function deleteFile(uri, file) {
            urie = uri + (uri.endsWith("/") ? "" : "/");
            if (window.confirm(file + "を削除します。よろしいですか？")) {
                treeView("?root=" + uri + "&del=" + urie + encodeURI(file));
            }
            return false;
        }
        function createDir() {
            msg = window.prompt("作成するフォルダ名を入力してください", "フォルダ名");
            if (msg != null && window.confirm("フォルダ" + msg + "を" + root + "に作成します。よろしいですか？")) {
                treeView("?root=" + root + "&dirName=" + encodeURI(msg));
                root = root + "/" + msg;
            }
            return false;
        }
        function createFile() {
            msg = window.prompt("作成するファイル名を入力してください", "ファイル名");
            if (msg != null && window.confirm("ファイル" + msg + "を" + root + "に作成します。よろしいですか？")) {
                treeView("?root=" + root + "&FileName=" + encodeURI(msg));
            }
            return false;
        }
        function dl() {
            msg1 = window.prompt("ダウンロードするＵＲＬを入力してください", "");
            msg2 = scriptn+"dl.jss?target=" + root + "&dl=" + encodeURI(msg1);
            if (msg1 != null) {
                if (window.confirm(msg1 + "をダウンロードします。よろしいですか？")) {
                    window.open(msg2, "_dl");
                }
            }
        }
        function expt() {
            //zipを作って
            if (window.confirm(root + "を圧縮してダウンロードします。よろしいですか？")) {
                window.open(`${scriptn}zip.jss?path=${root}` , "_zip");
            }
            return false;
        }
        function viewImages() {
            window.open(`${scriptn}viewImages.jss?path=${root}`, "_images");
            return false;
        }
        // EDITOR
        async function showMarkDown(path) {
            this.lastIndexed = -1;
            this.filepath = undefined;
            if(this.editor !== undefined){
                this.editor.container.style.display = "none";
            }
            document.getElementById("searchLists").innerHTML = "";
            document.getElementById("markDown").innerHTML = "";
            // markdownをhtmlに変換する
            // コンバーターを作成する
            if(converter === undefined){
                converter = new showdown.Converter();
                converter.setOption('tables', true);
            }
            doc="";
            await axios.get(`${scriptn}common/file_load.jss?path=${path}`)
                .then(function (response) {
                    doc = response.data;
                })
                .catch(function (error) {
                    console.log(error);
                });
            html = converter.makeHtml(doc);
            document.getElementById("markDown").innerHTML = html;
        }
        function myClose()
        {
            if(this.lastIndexed < 0 ){
                return;
            }
            checkEditData();
            elm = getElements();
            if(elm[this.lastIndexed].modify){
                if( window.confirm(extractFileName(elm[this.lastIndexed].filepath)+"は変更されています。破棄してよろしいですか？") ){
                    elm.splice(this.lastIndexed,1);
                    if(elm.length == 0){
                        showMarkDown(`${scriptp}common/develop.md`);
                    }else{
                        this.lastIndexed -= 1;
                        if(this.lastIndexed < 0){
                            this.lastIndexed = 0;
                        }
                        this.filepath = elm[this.lastIndexed].filepath;
                        setEditor(elm[this.lastIndexed].code,elm[this.lastIndexed].filepath);
                        showTab(elm, elm[this.lastIndexed].filepath);
                    }
                    localStorage.setItem('files', JSON.stringify(elm));
                }
            }else{
                elm.splice(this.lastIndexed,1);
                if(elm.length == 0){
                    showMarkDown(`${scriptp}common/develop.md`);
                }else{
                    this.lastIndexed -= 1;
                    if(this.lastIndexed < 0){
                        this.lastIndexed = 0;
                    }
                    this.filepath = elm[this.lastIndexed].filepath;
                    setEditor(elm[this.lastIndexed].code,elm[this.lastIndexed].filepath);
                    showTab(elm, elm[this.lastIndexed].filepath);
                }
                localStorage.setItem('files', JSON.stringify(elm));
            }
            return false;
        }
        // TAB change
        function changeTab(index) {
            // list取得[{"filepath":filepath,"modify":false/true}]
            elm = getElements();
            this.filepath = elm[index].filepath;
            if (this.lastIndexed !== undefined) {
                currentCode = this.editor.getValue()
                if (elm[this.lastIndexed].code != currentCode) {
                    elm[this.lastIndexed].code = currentCode;
                    elm[this.lastIndexed].modify = true;
                    localStorage.setItem('files', JSON.stringify(elm));
                }
            }
            setEditor(elm[index].code,elm[index].filepath);
            showTab(elm, elm[index].filepath);
            this.lastIndexed = index;
        }

        // TAB表示
        function showTab(elm, path) {
            str = '<ul class="nav nav-tabs">';
            elm.forEach(function (datum, index) {
                basefile = extractFileName(datum.filepath);
                str += '<li class="nav-item">';
                strMod = (datum.modify) ? "*" : "";
                if (datum.filepath == path) {
                    str += '<a class="nav-link active align-text-top" aria-current="page" href="#" onClick="changeTab(' + index + ');">' + basefile + strMod + '</a>';
                    this.lastIndexed = index;
                } else {
                    str += '<a class="nav-link align-text-top" aria-current="page" href="#" onClick="changeTab(' + index + ');">' + basefile + strMod + '</a>';
                }
                str += '</li>';
            });
            str += '  </ul>';
            document.getElementById("searchLists").innerHTML = str;
        }
        function getElements()
        {
            try {
                files = localStorage.getItem('files');
                if( files !== undefined && files != null ) {
                    elm = JSON.parse(files);
                }else{
                    elm = [];
                }
            } catch (error) {
                elm = [];
            }
            return elm;
        }
        // データ変更チェック
        function checkEditData() {
            // list取得[{"filepath":filepath,"modify":false/true}]
            elm = getElements();
            index = elm.findIndex(datum => datum.filepath == this.filepath);
            if (index >= 0) {
                code = this.editor.getValue();
                if (code != elm[index].code) {
                    elm[index].code = code;
                    elm[index].modify = true;
                    localStorage.setItem('files', JSON.stringify(elm));
                }
            }
            return false;
        }
        // file save (no check)
        function saveFile(path, code) {
            const params = new URLSearchParams();
            params.append('path', path);
            params.append('code', code);
            axios.post(`${scriptn}common/file_save.jss`, params)
                .then(function (response) {
                    console.log(response);
                    return false;
                })
                .catch(function (error) {
                    console.log(error);
                });
        }

        // save code to file.
        function saveCode() {
            if(editor === undefined){
                return false;
            }
            code = editor.getValue();
            // list取得[{"filepath":filepath,"modify":false/true}]
            elm = getElements();
            // 変更中のものは変更中が優先
            // elm中にdataが存在するか調べる。
            index = elm.findIndex(datum => datum.filepath == this.filepath);
            if (index >= 0) {
                elm[index].code = code;
                elm[index].modify = false;
                localStorage.setItem('files', JSON.stringify(elm));
                saveFile(this.filepath, code);
                showTab(elm, elm[index].filepath);
                return false;
                // this.lastIndexed,filepathは変更されない
            } else {
                alert('エディター中に' + this.filepath + 'が存在しない');
            }
        }
        // file load
        function loadFile(path) {
            path = decodeURI(path);
            axios.get(`${scriptn}common/file_load.jss?path=${path}`)
                .then(function (response) {
                    // check current data
                    checkEditData();
                    // set code to editor
                    loadCode(path, response.data, false);

                    // canvas close
                    let myOffCanvas = document.getElementById("offcanvasWithBothOptions");
                    let openedCanvas = bootstrap.Offcanvas.getInstance(myOffCanvas);
                    openedCanvas.hide();
                    return false;
                })
                .catch(function (error) {
                    console.log(error);
                });
        }
        // set code and path to editor.
        function loadCode(path, code, modified) {
            // list取得[{"filepath":filepath,"modify":false/true}]
            elm = getElements();
            // 変更中のものは上書きするか問い合わせる
            data = { "filepath": `${path}`, "code": `${code}`, "modify": modified };
            // elm中にdataが存在するか調べる。
            index = elm.findIndex(datum => datum.filepath == path);
            if (index >= 0) {
                if (elm[index].code != code) {
                    // changed.
                    if (window.confirm(extractFileName(path) + "は変更されています。リロードしてよろしいですか？")) {
                        elm[index].modify = false
                        elm[index].code = code;
                        localStorage.setItem('files', JSON.stringify(elm));
                    }
                } else {
                    // no change
                    elm[index].modify = false
                    elm[index].code = code;
                    localStorage.setItem('files', JSON.stringify(elm));
                }
            } else {
                // new
                elm.push(data);
                index = elm.length-1;
                localStorage.setItem('files', JSON.stringify(elm));
            }
            setEditor(code,path);
            showTab(elm, path);
            return elm;
        }

        // get file type for ACE editor.
        function getFileType(path) {
            //ファイルタイプ生成
            let ext = extractFileExt(path);
            fileType = "";
            if (ext == ".js") {
                fileType = "ace/mode/javascript";
            } else if (ext == ".jss") {
                fileType = "ace/mode/jss";
            } else if (ext == ".css") {
                fileType = "ace/mode/css";
            } else if (ext == ".htm" || ext == ".html" || ext == ".jss") {
                fileType = "ace/mode/html";
            } else if (ext == ".rb") {
                fileType = "ace/mode/ruby";
            } else if (ext == ".as") {
                fileType = "ace/mode/as";
            } else if (ext == ".xml") {
                fileType = "ace/mode/xml";
            } else if (ext == ".php") {
                fileType = "ace/mode/php";
            } else if (ext == ".md") {
                fileType = "ace/mode/markdown";
            } else {
                fileType = "ace/mode/text";
            }
            return fileType;
        }
        // tree view
        function treeView(url) {
            axios.get(`${scriptn}tree_view.jss${url}`)
                .then(function (response) {
                    trees = document.getElementById("tree");
                    trees.innerHTML = response.data;
                    return false;
                })
                .catch(function (error) {
                    console.log(error);
                });
        }
    </script>
</body>
</html>
