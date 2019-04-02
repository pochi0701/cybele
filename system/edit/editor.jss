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
if        (ext == "js" || ext == "jss"){
    fileType = "ace/mode/javascript";
} else if (ext == "css") {
    fileType = "ace/mode/css";
} else if (ext == "htm" || ext == "html" ){
    fileType = "ace/mode/html";
} else if (ext == ".rb") {
    fileType = "ace/mode/ruby";
} else if (ext == "as") {
    fileType = "ace/mode/as";
} else if (ext == "xml") {
    fileType = "ace/mode/xml";
} else if (ext == "php") {
    fileType = "ace/mode/php";
} else {
    fileType = "ace/mode/text";
}
?>
<!DOCTYPE html>
<html>
<head>
  <title><? print(basename(path)); ?></title>
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
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
        parent.up.saveData(document.forma.path.value,txt,0);
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
        parent.up.saveData(document.forma.path.value,txt,0);
        var frm = document.getElementsByTagName('form')[0];
        frm.code.value = txt;
        frm.submit();
        
        myWin = window.open(path,path);
        parent.left.myreload();
        tabRewrite();
    }
    function checkCode() {
        parent.tmp.location.href="check.jss?target="+document.forma.path.value;
    }
    function tabRewrite(){
        var list = parent.up.fileList();
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
        var list = parent.up.fileList();
        url = list[idx].url;
        var txt = parent.up.fileData(idx);
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
            parent.up.saveData(document.forma.path.value,txt,1-mode);
        }
        else
        {
            //このコードのタイプを設定
            editor.getSession().setMode("<? print(fileType); ?>");
            //タブを作るためにセーブ
            parent.up.saveData("<? print(encodeURI(path));?>",txt,0);
            var list = parent.up.fileList();
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
        var list = parent.up.fileList();
        for( var i in list){
            if( document.forma.path.value == list[i].url && list[i].change ){
                if( ! window.confirm(url2base(list[i].url)+"は変更されています。破棄してよろしいですか？") ){
                    return false;
                }
            }
        }
        parent.up.removeData(document.forma.path.value);
        list = parent.up.fileList();
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
    // -->
    </script>
    <!-- Bootstrap -->
    <link href="http://netdna.bootstrapcdn.com/bootstrap/3.3.6/css/bootstrap.min.css" rel="stylesheet">
    <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/font-awesome/4.5.0/css/font-awesome.min.css">
      <style>
        @media (min-width:767px){
          .dropdown:hover > .dropdown-menu{
            display: block;
          }
        }
    </style>
    <script src="http://code.jquery.com/jquery-1.12.0.min.js"></script>
    <script src="http://netdna.bootstrapcdn.com/bootstrap/3.3.6/js/bootstrap.min.js"></script>
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
<!--      <a href="#" onclick="loadCode();" title="Load File(L)" accesskey="L"><i class="fa fa-cloud-download fa-lg"></i></a> //-->
      <a href="#" onclick="saveCode();" title="Save File(S)" accesskey="S"><i class="fa fa-cloud-upload fa-lg"></i></a>
      <a href="#" onclick="viewCode();" title="View File(V)" accesskey="V"><i class="fa fa-television fa-lg"></i></a>
      <a href="#" onclick="checkCode();" title="Check File(G)" accesskey="G"><i class="fa fa-check fa-lg"></i></a>
      <a href="#" onclick="indent();" title="indent(i)" accesskey="I"><i class="fa fa-indent fa-lg"></i></a>
      <a href="#" onclick="myClose();" title="close" accesskey="T"><i class="fa fa-times fa-lg"></i></a>
      <a href="#" onclick="mytree();" title="expand" accesskey="H"><i class="fa fa-expand  fa-lg"></i></a>    </form>
    <div id="searchLists"></div>
    <pre id="editor"><?print(htmlspecialchars(code));?></pre>

    <!-- SCRIPTS //-->
    <script src="https://cdnjs.cloudflare.com/ajax/libs/ace/1.2.9/ace.js"></script>
    <script type="text/javascript">
      <!--
      var editor = ace.edit("editor");
      editor.setTheme("ace/theme/textmate");
      //-->
    </script>
  </body>
</html>
