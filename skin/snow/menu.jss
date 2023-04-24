<?
//URLエンコード
function size_num_read(size) {
    bytes = ["B","KB","MB","GB","TB"];
    for( i=0 ; i < 5 ; i++) {
        if(size > 1024){
            size = size / 1024;
        }else{
            break;
        }
    }
    //return "<span style=\"font-size: small\">".	round($size, 2).$val."</span>";
    return (round(size*100)/100)+bytes[i];
}
//変数取得
base    = _SERVER.DOCUMENT_ROOT;
root    = _GET.root;

//ユーザ限定処理
//初回の処理
if( root.length==0){
    root = _SERVER.DOCUMENT_ROOT;
}
sf = root.substring(base.length,root.length);
if( sf == ""){
   sf = "/";
}
//右端の/をなくす
while( root[root.length-1] == "/" ){
    root = root.substring(0,root.length-1);
}
me=_SERVER.SCRIPT_NAME;
?>
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>file tree - <? print(sf); ?></title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-0evHe/X+R7YkIZDRvuzKMRqM+OrBnVFBL6DOitfPri4tjfHxaWutUpFmBp4vmVor" crossorigin="anonymous">
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/js/bootstrap.bundle.min.js" integrity="sha384-pprn3073KE6tl6bjs2QrFaJGz5/SUsLqktiwsUTF55Jfv3qYSDhgCecCxMW52nD2" crossorigin="anonymous"></script>
    <link rel ="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.1.1/css/all.min.css">
    <script type="text/javascript">
    <!--
        var root = "<? print( root ); ?>";
        // -->
    </script>
</head>
<body>
    <!-- 1.ナビゲーションバーの設定 -->
    <nav class="navbar navbar-expand-lg navbar-dark bg-dark sticky-top">
        <div class="container-fluid">
            <a class="navbar-brand" href="/" target="_top">Cybele</a>
            <button class="navbar-toggler" type="button" data-bs-toggle="collapse" data-bs-target="#navbarSupportedContent" aria-controls="navbarSupportedContent" aria-expanded="false" aria-label="Toggle navigation">
                <span class="navbar-toggler-icon"></span>
            </button>
            <div class="collapse navbar-collapse flex-grow-1 text-end" id="myNavbar">
                <ul class="navbar-nav ms-auto flex-nowrap">
                    <li><a href="/" class="nav-link m-2 menu-item" target="_top">Home</a></li>
                </ul>
            </div>
        </div>
    </nav>
    <div class="container">
        [<? print( sf ); ?>]
        <div id="tree" />
        <table>
        <?
            //親ディレクトリ
            filePath = dirname(root);
            if ( filePath != ""  && filePath.indexOf(base)>=0 ){
                url = "?root="+encodeURI(filePath);
                print( "<tr><td><a href=\""+url+"\" class=\"lnk\">[<i class=\"fa fa-arrow-up fa-lg\"></i>]</a></td></tr>\n");
            }
            //ディレクトリの場合
            if (dir_exists(root)) {
                //ディレクトリ読み込み
                files = eval(scandir(root));
                for( i = files.length-1; i>=0 ;i--){
                    if( basename(files[i]) == "." || basename(files[i]) == ".." ){
                        files.remove(files[i]);
                    }
                }
                if( files.length>0 ){
                    //check each folders
                    for( i = 0 ; i < files.length ; i++ ){
                        file = files[i];
                        filePath = root+"/"+basename(file);
                        if( dir_exists(file) ){
                            //make link tag
                            stat = eval(file_stat(file));
                            title = stat.date+" "+stat.permission;
                            url1 = "?root="+encodeURI(filePath);
                            print( "<tr><td><i class=\"fa fa-folder fa-lg\"></i></td><td><a href=\""+url1+"\" class=\"lnk\" title=\""+title+"\">["+basename(file)+"]</a></td></tr>\n");
                        }
                    }
                    //check each files.
                    for( i=0 ; i < files.length ; i++ ){
                        file = files[i];
                        filePath = root+"/"+basename(file);
                        if( file_exists(filePath) ){
                            fl = filePath;
                            if( fl.indexOf(base)>=0){
                                fl = dirname(fl.substring(base.length,fl.length));
                            }
                            fl= fl+"/";
                            ext = extractFileExt(file).toLowerCase();
                            if( ext != "bak"){
                                fname = basename(file).nkfconv("Sw");
                                option = "";

                                //make link tag
                                stat = eval(file_stat(filePath));
                                title = stat.date+" "+stat.permission;
                                if( ext == "jpg" || ext == "png" || ext == "gif" ){
                                    icon = "fas fa-image fa-lg";
                                    url1 = fl+basename(filePath)+"?action=ImageView.jss";
                                }else if( ext == "mp4" || ext == "3gp" || ext == "mov" ){
                                    icon = "fa fa-film fa-lg";
                                    url1 = fl+basename(filePath)+"?action=preview.jss";
                                }else if( ext == "md"){
                                    icon = "fa-brands fa-markdown fa-lg";
                                    url1 = fl+basename(filePath)+"?action=MarkDown.jss";
                                }else if( ext == "mp3" ){
                                    var mp3=eval(JSON.mp3id3tag(filePath));
                                    if( mp3.title.length > 0 ){
                                        fname = mp3.title;
                                    }
                                    icon = "fa fa-music fa-lg";
                                    url1 = fl+basename(filePath)+"?action=audio.jss";
                                }else if( ext == "html" || ext == "htm" ){
                                    icon = "fa fa-file fa-lg";
                                    url1 = fl+basename(filePath);
                                }else if( ext == "jss" ){
                                    icon = "fa fa-code fa-lg";
                                    url1 = fl+basename(filePath);
                                }else{
                                    icon = "fa fa-question-circle fa-lg";
                                    url1 = fl+basename(filePath);
                                }
                                lnk = "lnk";
                                print( "<tr><td><i class=\""+icon+"\"></i></td><td><a href=\""+url1+"\" class=\""+lnk+"\" title=\""+title+"\">"+fname+"</a></td><td>"+option+"</td></tr>\n");
                            }
                        }
                    }
                }
            }
        ?>
        </table>
    </div>
</body>
</html>
