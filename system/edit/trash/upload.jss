<!DOCTYPE html PUBLIC "-//IETF//DTD HTML 2.0//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
</head>
<html>
<title>UPLOADER</title>
<body>
<?
function TrimPort(ip)
{
    pos = strpos(ip,":");
    if( pos ){
        return substr(ip,0,pos);
    }else{
        return ip;
    }
}
/*
* ファイルのアップロードを行う。
* @param _FILES：アップロード情報（POST）
*/
uf = "";
function uploadfile( _FILES,folder  ){
    MAX_FILE_SIZE = 2000 * 1024 *1024;                                      // アップロードするファイルの最大バイト数の設定
    uploaddir    = folder;//dirname(_SERVER.SCRIPT_FILENAME)+"/";           // アップロードフォルダ名
    file_name    = _FILES['filename']['name'];                              // ファイル名
    tmp_filename = _FILES['filename']['tmp_name'];
    retvalue = true;
    global uf;
    uf = "http://"+_SERVER.SERVER_NAME+dirname(_SERVER.SCRIPT_NAME)+"/"+file_name;
    
    if( file_exists( tmp_filename ) ){
        // filesize() は 2GB より大きなファイルについては期待とは違う値を返すことがあります。
        file_size = filesize( tmp_filename );  // ファイルサイズ
        if( file_size <= MAX_FILE_SIZE ){
            // アップするファイルとして JPG GIF PNG の拡張子を持ったのもしか実行しない
            ret = move_uploaded_file( tmp_filename , uploaddir+file_name );
            if( true == ret ){
                changePermission( uploaddir+file_name , 0766 );
            }else{
                print ("アップロード失敗");
                retvalue = false;
            }
        }else{
            print ("ファイルサイズが大きすぎます");
            retvalue = false;
        }
    }
    return retvalue;
}


/**
* パーミッション変更
* @param       filename        :       ファイル名
* @param       pm              :       属性値
*/
function changePermission( filename , pm )
{
    if( 0 == file_exists( filename ) )
    {
        return;
    }
    ret = chmod( filename , pm );
}

//本体
uploaded = 0;
folder   = _GET.folder;
if( _POST.uploadhide != undefined ){
    uploaded = 2;
    if( 0 < _FILES.filename.name.length() ){
        //アップロード
        ret = uploadfile(folder);
        if( true == ret ){
            uploaded = 1;
            //拡張子と元に分ける
        }
    }
}
?>
<div id="banner">
<h1>UPLOADER</h1>
</div>
こんにちわ。<br/>
アップロード可能なファイルはjpg,png,gifです。<br/>
<form id = "formname" name = "formname" action = "<? print(_SERVER.SCRIPT_NAME); ?>" method = "POST" enctype = "multipart/form-data">
<div class="popup">
<?
if( uploaded == 1){
    print( "<input type=\"text\" size=\"50\" value=\""+uf+"\"/><br/>\n");
}else if ( uploaded == 2 ){
    print( "アップロード失敗<br/>\n");
}
?>
<input type="hidden" id="uploadhide" name="uploadhide" value = "upload">
<input type="hidden" id="folder"     name="folder"     value = "<? print(folder);?>">
登録するファイル名を設定して登録ボタンを押して下さい。<br/>
<table border="1" cellspacing="1">
  <tr>
    <th>登録するファイル</th>
  </tr>
  <tr>
    <td><input type = "file" id = "filename" name = "filename" size="30"></td>
  </tr>
</table>
<input type="submit" name="upload" value="　登　録　">
</div>
</form>
<br>
</body>
</html>
