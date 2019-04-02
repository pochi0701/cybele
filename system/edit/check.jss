<?
//var/log/httpdをchmod 755とすること。
//require_once("req.");

exec( "sudo /home/kanazawa/viewlog tail /var/log/httpd/error_log",d,res );

target = array_get(_GET,"target");
data = "";
for( i = 0 ; i < count(d) ; i++ ){
    if( strpos(d[i],target) ){
        //[から]を切り取る
        dt = explode("]",d[i]);
        dt = explode("[",dt[0]);
        if( strtotime(dt[1])>mktime(date("H"),date("i")-15,date("s"),date("m"),date("d"),date("y"))){
            data = rtrim(d[i])."\\n".data;
        }
    }
}
if( ! strlen(data)){
    data = "最近のエラーはありません。";
}
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="ja" xml:lang="ja">
<head>
<script type="text/javascript">
<!--
function checkCode() {
    alert( "<? print( data );?>" );
}
//-->
</script>
</head>
<body onload="checkCode();">
</body>
</html>
