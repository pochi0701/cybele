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
    <title>Code Viewer</title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-9ndCyUaIbzAi2FUVXJi0CjmCapSmO7SnpJef0486qhLnuZ2cdeRhO02iuK6FUUVM" crossorigin="anonymous">
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.1.1/css/all.min.css">
    <script src="https://cdn.jsdelivr.net/npm/axios/dist/axios.min.js"></script>
    <style type="text/css" media="screen">
        <!--
        html, body {
            height: 100%;
        }

        #editArea {
            position: relative;
            //border: 1px solid lightgray;
            height: 100%;
            width: 100%;
        }
        // -->
    </style>
</head>
<body onload="init();">
    <!-- EDITOR -->
    <a href="#" onclick="saveCode();" title="Save File(AltS)" accesskey="S"><i class="fa fa-cloud-upload fa-lg"></i></a>
    <a href="#" onclick="viewCode(filepath);" title="View File(AltV)" accesskey="V"><i class="fa fa-television fa-lg"></i></a>
    <div id="editArea"></div>

    <!-- SCRIPTS //-->
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/js/bootstrap.bundle.min.js" integrity="sha384-geWF76RCwLtnZ8qwWowPQNguL3RmwHVBC9FhGdlKrxdiJJigb/j/68SIy3Te4Bkz" crossorigin="anonymous"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/ace/1.32.2/ace.min.js" integrity="sha512-AGoM9vaICK6JL/xBUOBmP2khdtorQ4a7srtuqKGAtvF7/IzwDHd30lD0O3fk2b1V2aYXWKJLPWr3obywNxSEqw==" crossorigin="anonymous" referrerpolicy="no-referrer"></script>
    <script type="text/javascript">
        const extractFileExt = str => str.slice(str.lastIndexOf("."));
        const extractFileName = str => str.slice(str.lastIndexOf("/") + 1);
        const extractFilePath = str => str.substring(0, str.lastIndexOf("/"));
        var editor;
        var filepath;
        var root = "<? print( root ); ?>";
        var base = "<? print( base ); ?>";
        var scriptp = "<? print( scriptp ); ?>";
        var mycode;
        scriptp = extractFilePath(scriptp) + "/";
        var scriptn = "<? print( scriptn ); ?>";
        scriptn = extractFilePath(scriptn) + "/";
        // load and edit
        function init() {
            loadFile(base + root);
            return false;
        }
        // display result of rendering code.
        function viewCode(path) {
            if (path == undefined) {
                return;
            }
            if(editor.getValue() != mycode){
                if( window.confirm(extractFileName(root)+"は変更されています。保存しますか？") ){
                    saveCode();
                }
            }
            document_root = "<? print(_SERVER.DOCUMENT_ROOT); ?>";
            if (path.endsWith('md')) {
                path = "http://<?print(_SERVER.HTTP_HOST);?>" + path.substring(document_root.length, path.length) + "?action=MarkDown.jss";
            } else {
                path = "http://<?print(_SERVER.HTTP_HOST);?>" + path.substring(document_root.length, path.length);
            }
            var myWin = window.open(path, path);
        }
        // file save (no check)
        function saveFile(path, code) {
            const params = new URLSearchParams();
            params.append('path', path);
            params.append('code', code);
            axios.post(`${scriptn}file_save.jss`, params)
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
            if (editor == undefined) {
                return false;
            }
            mycode = editor.getValue();
            saveFile(this.filepath, mycode);
            return false;
        }
        // file load
        function loadFile(path) {
            axios.get(`${scriptn}file_load.jss?path=${path}`)
                .then(function (response) {
                    // set code to editor
                    mycode = response.data;
                    // editorの設定
                    if (this.editor == undefined) {
                        document.getElementById("editArea").innerHTML = "";
                        this.editor = ace.edit("editArea");
                        this.editor.setTheme("ace/theme/textmate");
                    }
                    this.editor.container.style.display = "";
                    this.editor.setValue(mycode, -1);
                    //このコードのタイプを設定
                    this.editor.getSession().setMode(getFileType(path));
                    this.filepath = path;
                    return;
                })
                .catch(function (error) {
                    console.log(error);
                });
        }
        // get file type for ACE editor.
        function getFileType(path) {
            //ファイルタイプ生成
            var ext = extractFileExt(path);
            fileType = "";
            if (ext == ".js") {
                fileType = "ace/mode/javascript";
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
    </script>
</body>
</html>
