<?
/**
*    Web Editor
*/

path = _GET.path;
errMsg = "<!-- no error -->";
//読み込み処理
if (file_exists(path)) {
    code = loadFromFile(path);
}
//ファイルタイプ生成
ext = extractFileExt(path);
if        (ext == "js" ){
    fileType = "ace/mode/javascript";
} else if (ext == "css") {
    fileType = "ace/mode/css";
} else if (ext == "htm" || ext == "html" || ext == "jss"){
    fileType = "ace/mode/html";
} else if (ext == ".rb") {
    fileType = "ace/mode/ruby";
} else if (ext == "as") {
    fileType = "ace/mode/as";
} else if (ext == "xml") {
    fileType = "ace/mode/xml";
} else if (ext == "php") {
    fileType = "ace/mode/php";
} else if (ext == "md") {
    fileType = "ace/mode/markdown";
} else {
    fileType = "ace/mode/text";
}
?>
<!DOCTYPE html>
<html>
<head>
  <title><? print(basename(path)); ?></title>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <script type="text/javascript">
    <!--
    var hidden = false;
    var filename;
//    function loadCode() {
//        pushCode(0);
//        var frm = document.getElementsByTagName('form')[0];
//        frm.submit();
//        parent.left.myreload();
//    }
    function saveCode() {
        var txt=editor.getValue();
        saveData(document.forma.path.value,txt,0);
        var frm = document.getElementsByTagName('form')[0];
        frm.code.value = txt;
        frm.submit();
        parent.left.myreload();
        tabRewrite();
        return false;
    }
    function viewCode() {
        var path = document.forma.path.value;
        var root = "<? print(_SERVER.DOCUMENT_ROOT); ?>";
        var pos = path.indexOf(path,root);
        path = "http://<?print(_SERVER.HTTP_HOST);?>"+path.substring(root.length,path.length);
        //path = "http://127.0.0.1:8000"+path.substring(root.length,path.length);
        
        var txt=editor.getValue();
        saveData(document.forma.path.value,txt,0);
        var frm = document.getElementsByTagName('form')[0];
        frm.code.value = txt;
        frm.submit();
        
        myWin = window.open(path,path);
        parent.left.myreload();
        tabRewrite();
    }
    function tabRewrite(){
        var list = fileList();
        for( var i = 0 ; i < list.length ; i++ ){
            var pop = document.getElementById("e"+i);
            if( pop ){
                pop.text = url2base(list[i].url)+(list[i].change?"*":"");
                if( i == idx ){
                    pop.setAttribute('style','background-color: #ffa0a0');
                }else{
                    pop.setAttribute('style','background-color: #a0a0ff');
                }
            }
        }
        return;   
    }
    function popCode(idx){
        pushCode(0);
        var list = fileList();
        url = list[idx].url;
        var txt = fileData(idx);
        //このコードのタイプを設定
        //alert( url2ext(url));
        editor.getSession().setMode(url2ext(url));
        editor.getSession().setValue( txt );
        
        for( var i = 0 ; i < list.length ; i++ ){
            var pop = document.getElementById("e"+i);
            if( pop ){
                pop.text = url2base(list[i].url)+(list[i].change?"*":"");
                if( i == idx ){
                    pop.setAttribute('style','background-color: #ffa0a0');
                }else{
                    pop.setAttribute('style','background-color: #a0a0ff');
                }
            }
        }
        document.forma.path.value = url;
        return;
    }
    function pushCode(mode)
    {
        //親に渡して
        //var editor = ace.edit("editor");
        var txt=editor.getValue();
        if( mode == 0 ){
            saveData(document.forma.path.value,txt,1-mode);
        }
        else
        {
            //このコードのタイプを設定
            editor.getSession().setMode("<? print(fileType); ?>");
            //タブを作るためにセーブ
            saveData("<? print(encodeURI(path));?>",txt,0);
            var list = fileList();
            /* 親ノード */
            var parentObj=document.getElementById("searchLists");
            for( var i in list){
                /* 要素を生成 */
                var listObj=parentObj.appendChild(document.createElement("A"));
                /* a要素のhref属性にリンク先URLを設定 */
                listObj.setAttribute("href", "#");
                /* a要素のtitle属性にリンクテキストを設定 */
                listObj.setAttribute("title", list[i].url);
                /* a要素のtitle属性にリンクテキストを設定 */
                listObj.setAttribute("id", "e"+i);
                /* a要素のtitle属性にリンクテキストを設定 */
                listObj.setAttribute("class", "tab");
                listObj.setAttribute("onclick", "popCode("+i+")");
                if( list[i].url == '<? print(encodeURI(path));?>' ){
                    listObj.setAttribute('style','background-color: #ffa0a0');
                }
                /* a要素のリンクテキストをテキストノードとして追加 */
                listObj.appendChild(document.createTextNode(url2base(list[i].url)));
            }
        }
    }
    //パスからファイル名のみを抜き出し
    function url2base(url){
        var n = url.lastIndexOf("/");
        url = url.substring(n+1).toLowerCase();
        return url;
    }
    //パスからモードを生成
    function url2ext(url){
        var n = url.lastIndexOf(".");
        url = url.substring(n+1).toLowerCase();
        if( url == "js"){
            url = "javascript";
        }else if ( url == "bat"){
            url = "text";
        }
        return "ace/mode/"+url;
    }
    function myClose()
    {
        pushCode(0);
        var list = fileList();
        for( var i in list){
            if( document.forma.path.value == list[i].url && list[i].change ){
                if( ! window.confirm(url2base(list[i].url)+"は変更されています。破棄してよろしいですか？") ){
                    return false;
                }
            }
        }
        removeData(document.forma.path.value);
        list = fileList();
        if( list.length == 0 ){
            location.href=encodeURI("wiki.jss?page=NeonEditor");
        }else{
            location.href="<? print(_SERVER.SCRIPT_NAME);?>?path="+list[0].url;
        }
    }
    //左フレームの表示/非表示
    function mytree(){
        hidden = ! hidden;
        id = 'theFrame';
        if( hidden )
        {
            cols = '0,*';
        }else{
            cols = '20%,*'
        }
        frame = window.top.document.getElementById(id);
        if (!frame) {
            return;
        }
        frame.cols = cols;
    }
    function indent()
    {
        location.href=encodeURI("./indent.jss?filename="+document.forma.path.value);
    }
    function saveData(url,datum,flag)
    {
        var elm = localStorage.getItem('ctl');
        var list;
        var change = false;
        var pivot=0;
        if( elm != undefined ){
           list = JSON.parse(elm);
           pivot = list.length;
        }else{
           list = [];
        }
        for( var i in list ){
            if( list[i].url == url ){
                pivot = i;
                break;
            }
        }
        //データ登録
        if( url.length>0 ){
            var dat = localStorage.getItem('d'+pivot);
            if( flag && pivot < list.length ){
                change =   ( dat != datum ) || list[pivot].change;
            }
            localStorage.setItem("d"+pivot,datum);
            list[pivot] = {"url":url,"change":change};
            localStorage.setItem('ctl',JSON.stringify(list));
        }
    }
    function removeData(url)
    {
        var elm = localStorage.getItem('ctl');
        var list = JSON.parse(elm);
        var cnt = list.length;
        for( var i in list ){
            if( list[i].url == url ){
                list.splice(i,1);
                localStorage.setItem('ctl',JSON.stringify(list));
                //移動
                for( var j = i; j < cnt-1 ; j++ ){
                    //こうしないと数値にならない
                    var k = 1+parseInt(j,10);
                    localStorage.setItem('d'+j,localStorage.getItem('d'+k));
                }
                localStorage.setItem('d'+(cnt-1),undefined);
                return;
            }
        }
    }
    function fileList() {
        
        var elm = localStorage.getItem('ctl');
        if( elm != null ){
            return JSON.parse(elm);
        }else{
            return [];
        }
    }
    
    function fileData(idx) {
        var data = localStorage.getItem('d'+idx);
        return data;
    }
    // -->
    </script>
    
    
    <!-- Bootstrap -->
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-rbsA2VBKQhggwzxH7pPCaAqO46MgnOM80zW1RWuH61DGLwZJEdK2Kadq2F9CUG65" crossorigin="anonymous">
    <link rel ="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.1.1/css/all.min.css">
    <script src="http://code.jquery.com/jquery-1.12.0.min.js"></script>
    <style type="text/css" media="screen">
      <!--
      html,body { height:100%; }
      .tab {
          text-align: center;
          text-decoration: none;
          color: #FFF;
          margin: 1px 1px 1px 1px;
          padding: 2px 5px 2px 5px;
          background-color: #a0a0ff;
      }
      .tab:hover {
          background-color: #eaebd8;
      }
      #editor {
          position: relative;
          border: 1px solid lightgray;
          height: 100%;
          width: 100%;
      }
      //-->
    </style>
  </head>
  <body onLoad="pushCode(1);">
    <span style="color:red;"><? print(errMsg);?></span>
    <form name="forma" action="save.jss" method="post" target="tmp">
      <input type="hidden" name="code" />
      <input type="hidden" name="path" style="width:20em;" value="<? print(encodeURI(path)); ?>" />
      <a href="#" onclick="saveCode();" title="Save File(S)" accesskey="S"><i class="fa fa-cloud-upload fa-lg"></i></a>
      <a href="#" onclick="viewCode();" title="View File(V)" accesskey="V"><i class="fa fa-television fa-lg"></i></a>
      <a href="#" onclick="mytree();" title="expand" accesskey="H"><i class="fa fa-expand fa-lg"></i></a>
      <a href="#" onclick="myClose();" title="close" accesskey="T"><i class="fa fa-times fa-lg"></i></a>
    </form>
    <div id="searchLists"></div>
    <pre id="editor"><?print(htmlspecialchars(code));?></pre>

    <!-- SCRIPTS //-->
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/js/bootstrap.bundle.min.js" integrity="sha384-kenU1KFdBIe4zVF0s0G1M5b4hcpxyD9F7jL+jjXkk+Q2h455rYXK/7HAuoJl+0I4" crossorigin="anonymous"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/ace/1.17.0/ace.min.js"></script>
    <script type="text/javascript">
      <!--
      var editor = ace.edit("editor");
      editor.setTheme("ace/theme/textmate");
      //-->
    </script>
  </body>
</html>
