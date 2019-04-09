<?
    session_start();
    var database;
    var sql2 = _POST.sql;
    var db_id = _POST.id;
    var databases;
    var tables;
    var text;
    databases = _SESSION.databases;
    if( db_id == undefined ){
        db_id = _SESSION.db_id;
        if( db_id == undefined ){
           _SESSION.db_id  = 1;
           db_id = _SESSION.db_id;          
        }
    }else{
        _SESSION.db_id = db_id;
    }

    if( databases == undefined ){
        database = DBConnect("_SYSTEM");
        text = database.SQL("show databases");
        databases = eval(text);
        database.DBDisConnect();
        _SESSION.databases = databases;
    }
    if( tables == undefined ){
        for( var i = 0 ; i < databases.length ; i++){
            database = DBConnect(databases[i]);
            text = database.SQL("show tables");
            tables[i] = eval(text);
            database.DBDisConnect();
        }
        _SESSION.tables = tables;
    }
    var result = undefined;

    //結果の取得
    if( sql2.length > 0 ){
        database = DBConnect(databases[db_id]);
        var result = eval(database.SQL(sql2));
        database.DBDisConnect();
        print( result );
    }
?>
<!DOCTYPE html>
<html lang="ja">
<head>
  <meta charset="utf-8">
  <title>menu</title>
  <meta http-equiv="Pragma" content="no-cache">
  <meta http-equiv="Cache-Control" content="no-cache">
  <meta http-equiv="Expires" content="0">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link href="http://netdna.bootstrapcdn.com/bootstrap/3.3.6/css/bootstrap.min.css" rel="stylesheet">
  <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/font-awesome/4.5.0/css/font-awesome.min.css">
  <script src="http://code.jquery.com/jquery-1.12.0.min.js"></script>
  <script src="http://netdna.bootstrapcdn.com/bootstrap/3.3.6/js/bootstrap.min.js"></script>
  <script>
  function setDatabase(db){
      make_hidden("id",db,"SQL");
  }
/**
 * make_hidden : hiddenを作成する : Version 1.2
 */
function make_hidden(name, value, formname) {
    var q = document.createElement('input');
    q.type = 'hidden';
    q.name = name;
    q.value = value;
    if (formname) {
    	if ( document.forms[formname] == undefined ){
    		console.error( "ERROR: form " + formname + " is not exists." );
    	}
    	document.forms[formname].appendChild(q);
    } else {
    	document.forms[0].appendChild(q);
    }
}
  </script>
</head>
<body style="padding-top:53px;">
<div class="text-center">
</div>
<div class="container">
  <!-- 1.ナビゲーションバーの設定 -->
  <nav class="navbar navbar-inverse navbar-fixed-top">
    <!-- 2.ヘッダ情報 -->
    <div class="navbar-header">
      <button type="button" class="navbar-toggle" data-toggle="collapse" data-target="#nav-menu-1">
        <span class="sr-only">Toggle navigation</span>
        <span class="icon-bar"></span>
        <span class="icon-bar"></span>
        <span class="icon-bar"></span>
      </button>
      <a class="navbar-brand" href="/" target="_top">DBBrowser</a>
    </div>
    <!-- 3.リストの配置 -->
    <div class="collapse navbar-collapse" id="nav-menu-1">
      <ul class="nav navbar-nav navbar-right">
        <li ><a href="/" target="_top">Home</a></li>
      </ul>
    </div>
  </nav>
</div>

<div class="container">
	<div class="row">
		<div class="col-md-2">
		    Databases<br/>
            <ul>
              <?
                 for( var i = 0 ; i < databases.length ; i++ ){
                     if( tables[i].length > 0){
                         print( '<li>\n');
                         print( '<a data-toggle="collapse" id= "menu'+i+'" href=".collapse-menu'+i+'" onclick="setDatabase('+i+');">'+databases[i]+'</a>\n');
                         print( '<ul class="collapse '+(db_id==i?'in':'')+' collapse-menu'+i+'">\n');
                         for( var j=0 ; j < tables[i][j].length ; j++ ){ 
                             print( '<li><a href="#">'+tables[i][j]+'</a></li>\n');
                         }                     
                         print( '</ul>\n');
                         print( '</li>\n');
                     }
                 }
              ?>
            </ul>
		</div>
		<div class="col-md-10">
           <h3>SQL Browser</h3>
           <form  name="SQL" method="post">
                <!-- SQL入力エリア -->
                <div class="form-group">
                <label>Input SQL</label>
                <textarea class="form-control" rows="6" name="sql"><?print( sql2 );?></textarea>
                </div>
                <!-- 送信ボタン -->
                <button type="submit" class="btn btn-default">Execute SQL</button>
           </form>
           <div>
               <? 
                   if( result !== undefined ){
                       var rset = Object.keys(result[0]);
                       print( '<table class="table">\n');
                       print( '<thead>\n<tr>\n');
                       print( '<th>#</th>\n');
                       for( var col = 0 ; col< rset.length ; col++ ){
                           print( '<th>'+rset[col]+'</th>\n');
                       }
                       print( '</tr>\n</thead>\n');
                       print( '<tbody>\n');
                       for( var row = 0 ; row < result.length ; row++ ){
                           print( '<tr>\n');
                           print( '<th>'+(row+1)+'</th>\n');
                           for( var col = 0 ; col < rset.length ; col++ ){
                               print( '<td>'+result[row][rset[col]]+'</td>\n');
                           }
                           print( '</tr>\n');
                       }
                       print( '</tbody>\n');
                       print( '</table>\n');
                    }
                ?>
           </div>
		</div>
	</div>
</div>
</body>
</html>

