<?
path = _POST.path;
errMsg = '<!-- no error -->';
code = _POST.code;
if( code.length > 0 ){
    //bakファイル作成
    bak = path+".bak";
    if( path.indexOf('/../') == -1 ){
        if( file_exists(path) ){
            copy(path,bak);
        }
        //保存
        if (! saveToFile(path, code)) {
            print( "<html><head></head><body><script type=\"text/javascript\">alert( 'error! ("+path+" not saved)' );</script></body><html>");
        }
    }
}
?>
