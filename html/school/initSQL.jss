<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>SQL</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-9ndCyUaIbzAi2FUVXJi0CjmCapSmO7SnpJef0486qhLnuZ2cdeRhO02iuK6FUUVM" crossorigin="anonymous">
</head>
<body>
<?
var tables = [
"create table course(no number, name string, path string, detail string, purchase string, url string, start_date string);",
"create table content (no number, content_no number, name string, file string, detail string, path string, start string, end string, type string);",
"create table subcontent (no number,content_no number,sub_no number,name string,file string,done number,correct string,count string,execution string);",
];
var sql01 = [
"INSERT INTO course (no, name, path, detail, price, start_date, end_date) VALUES (0,'CYBELEの使い方紹介','school','使い方の説明','','https://www.birdland.co.jp/index.php?p=12','');",
"INSERT INTO course (no, name, path, detail, price, start_date, end_date) VALUES (1,'ルチノーITレッスン','school','キーボード入力から初めてプログラムを１から学べます。','','https://www.birdland.co.jp/index.php?p=16','');",
];
var database = DBConnect("_SYSTEM");
var ev;
var ready;

// course
ready = false;
ret = database.SQL("select count(*) as num from course;");
if( ret.startsWith('{') ){
    ev = eval(ret);
    if( ev.num == sql01.length ){
        //そのまま
        ready = true;
    }else{
        //個数が違う場合このコースだけ消す。
        database.SQL("delete from course;");
    }
}else{
    // create table
    database.SQL(tables[0]);
}
if(! ready){
    // insert
    for( var i = 0 ; i < sql01.length ; i++ ){
        ev = database.SQL(sql01[i]);
        if( ev.indexOf("OK")<0){
            print(sql01[i]+ev);
            break;
        }
    }
}
database.DBDisConnect();
// unlink
var me = _SERVER.DOCUMENT_ROOT+_SERVER.SCRIPT_NAME;
unlink(me);
?>
</body>
</html>