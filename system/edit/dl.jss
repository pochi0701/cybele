<?
    target = _GET.target;
    dl     = _GET.dl;
    tmp    = file_get_contents(dl);
    basename = basename(dl);
    target = rtrim(target,"/");
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="ja" xml:lang="ja">
<head>
<?
    if( ! tmp){
	print( "<body onload=\"javascript:alert('{dl}はダウンロードできません。')\">");
    }else{
	print( "<body>" );
        if( strlen(target)>0){
            if( basename.length == 0 || strpos(basename,".")==0){
                basename = "index.html";
            }
            //print "[basename][target][tmp]";
            if (fh = fopen("{target}/{basename}", 'w')) {
                fwrite(fh, tmp);
                fclose(fh);
            }
        }
    }
?>
</body></html>
