<?
function createZip(addr,file)
{
    //この中にファイルを全部入れておく。サブディレクトリなどあってもOK
    tempDir = _SERVER.DOCUMENT_ROOT;//作業用ディレクトリ;
    //ここにzipファイルを作ります
    filepath = tempDir+"/"+file;//生成するzipファイルのパス;
    //command = "zip -r  {$filepath} {$addr}";
    //cmd = "tar zcvf "+filepath+" "+ addr +" 1>/dev/null 2>/dev/null";
    cmd = "tar zcvf "+filepath+" "+ addr;
    //実行します
    command(cmd);
    return filepath;
}
addr = _GET.path;
if( addr[addr.length-1] == '/' ){
    addr = addr.substring(0,addr.length-1);
}
var addrLength = addr.length;
if( addrLength > 0 ){
    // カレントフォルダの取得
    var temp = addrLength - 1;
    var folder = "";
    while( temp >= 0){
        if( addr[temp] == '/' || addr[temp] == '\\' ){
           folder = addr.substring(temp,addrLength-temp);
           break;
        }
    }


    tempname=Math.randInt(1000000,9999999);
    //md5(uniqid(rand(), true));
    //tempname .= '.zip';
    tempname += '.tgz';
    path = createZip(addr,tempname);
    
    // ダウンロードさせるファイル名
    tmp_file = path;
    j_file   = tempname;
    // ヘッダ
    header("Content-Type: application/octet-stream");
    // ダイアログボックスに表示するファイル名
    header("Content-Disposition: attachment; filename="+j_file);
    // 対象ファイルを出力する。
    aa = loadFromFile(tmp_file);
    print( aa );
    //削除
    unlink(tmp_file);
}else{
    print( "圧縮するファイルがみつかりません。" );
}
?>
