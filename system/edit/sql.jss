<?
    session_start();
    var database;
    var sql2 = _POST.sql;
    var db_id = _POST.db_id;
    var databases;
    var tables;
    var text;
    var selectdb;
    var result = undefined;
    var orgstr = undefined;

    database = DBConnect("_SYSTEM");
    text = database.SQL("show databases");
    databases = eval(text);
    database.DBDisConnect();
    //databases = _SESSION.databases;
    if( db_id == undefined ){
        db_id = _SESSION.db_id;
        if( db_id == undefined ){
           _SESSION.db_id  = 1;
           db_id = _SESSION.db_id;
        }
    }else{
        _SESSION.db_id = db_id;
    }

    //結果の取得
    if( sql2.length > 0 ){
        database = DBConnect(databases[db_id]);
        orgstr = database.SQL(sql2);
        result = eval(orgstr);
        database.DBDisConnect();
    }

    for( var i = 0 ; i < databases.length ; i++){
        database = DBConnect(databases[i]);
        text = database.SQL("show tables");
        tables[i] = eval(text);
        database.DBDisConnect();
    }



?>
<!doctype html>
<html lang="ja">
<head>
    <meta charset="utf-8">
    <title>DB-Browser</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-9ndCyUaIbzAi2FUVXJi0CjmCapSmO7SnpJef0486qhLnuZ2cdeRhO02iuK6FUUVM" crossorigin="anonymous">
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/js/bootstrap.bundle.min.js" integrity="sha384-geWF76RCwLtnZ8qwWowPQNguL3RmwHVBC9FhGdlKrxdiJJigb/j/68SIy3Te4Bkz" crossorigin="anonymous"></script>
    <script type="text/javascript">
    <!--
        var databases=[<?for(i=1;i<databases.length;i++){print((i>1)?',':'');print('"'+databases[i]+'"');}?>];
        var tables=[<?
                    for(i=1;i<databases.length;i++){
                        print((i>1)?',':'');
                        print('[');
                        for(j=0;j<tables[i].length;j++){
                            print((j>0)?',':'');
                            print( '"'+tables[i][j]+'"');
                        }
                        print(']');
                    }
                    ?>];
        var selected = <?print(db_id);?>;
        function selectChange(event) {
            var num = document.getElementById("database").value;
            document.getElementById("db_id").value = num
            selectDatabase(num);
        }
        function selectDatabase(num){
            selected = num;
            var contents = '<ul class="list-group">';
            for( j = 0 ; j < tables[num-1].length ; j++){
                contents += '<li class="list-group-item" onclick="listTable('+(num-1)+','+j+');">'+tables[num-1][j]+'</li>';
            }
            contents += '</ul>';
            document.getElementById("selected").innerHTML = contents;
        }
        function OnFileSelect( inputElement ){
            var filepath = inputElement.value;
            document.getElementById("sql").value = "create table TABLE_NAME from "+filepath+";";
        }
        function listTable(dbidx,tableidx)
        {
            document.getElementById("sql").value = "select * from "+tables[dbidx][tableidx]+" limit 3;";
        }
        function setDatabase(db) {
            make_hidden("id", db, "SQL");
        }
        var toggle = 0;
        function disp(txt) {
            if (toggle == 0) {
                document.getElementById("dat").innerHTML = txt;
            }
            else {
                document.getElementById("dat").innerHTML = "";
            }
            toggle = 1 - toggle;
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
                if (document.forms[formname] == undefined) {
                    console.error("ERROR: form " + formname + " is not exists.");
                }
                document.forms[formname].appendChild(q);
            } else {
                document.forms[0].appendChild(q);
            }
        }
        hpt  = 'databaseを選択してテーブルをクリックするとテーブル一部表示のSQLが入力欄に設定されます。<br>';
        hpt += 'Execute SQLを押すとSQLが実行されます。';
        hpt += 'CSV to TableでCSV(タイトル付。文字コードはUTF8)を選択するとCSVをテーブルに変換するSQLが入力欄に設定されます。<br>';
        hpt += 'ファイルパスを正しく取得できないので、手動でパスを正しく変更しExecute SQLを押してください。<br>';
        hpt += 'ファイル名がテーブル名に、CSVの1行目の項目がカラム名に、最初の項目の内容により文字列か数値かが決まり入力されます。<br>';
        hpt += '<table class="table table-striped table-bordered">';
        hpt += '<thead><tr><th>効果</th><th>コマンド</th></tr></thead><tbody>';
        hpt += '<tr><td>データベース一覧</td><td>SHOW DATABASES;</td></tr>';
        hpt += '<tr><td>テーブル一覧</td><td>SHOW TABLES;</td></tr>';
        hpt += '<tr><td>テーブル構造表示</td><td>DESC table_name;</td></tr>';
        hpt += '<tr><td>データベース作成</td><td>CREATE DATABASE database_name;</td></tr>';
        hpt += '<tr><td>テーブル作成</td><td>CREATE TABLE table_name(column_name1 number/string,...);</td></tr>';
        hpt += '<tr><td>テーブル作成(CSVから)</td><td>CREATE TABLE table_name FROM FILE_PATH;</td></tr>';
        hpt += '<tr><td>データベース使用</td><td>USE database_name;</td></tr>';
        hpt += '<tr><td>データベース削除</td><td>DROP DATABASE database_name;</td></tr>';
        hpt += '<tr><td>テーブル削除</td><td>DROP TABLE table_name;</td></tr>';
        hpt += '<tr><td>テーブル項目追加</td><td>ALTER TABLE TABLENAME ADD (COLUMN1 definition1,COLUMN2 definition2..);</td></tr>';
        hpt += '<tr><td>テーブル名変更</td><td>ALTER TABLE TABLENAME MODIFY (COLUMN1 definition1,COLUMN2 definition2..);</td></tr>';
        hpt += '<tr><td>テーブル項目削除</td><td>ALTER TABLE TABLENAME DROP (COLUMN1, COLUMN2..);</td></tr>';
        hpt += '<tr><td>テーブル名変更</td><td>ALTER TABLE TABLENAME RENAME TO NEWTABLENAME;</td></tr>';
        hpt += '<tr><td>テーブル項目名変更</td><td>ALTER TABLE TABLENAME RENAME COLUMN OLDCOLUMN TO NEWCOLUMN;</td></tr>';
        hpt += '</tbody></table>';
        //-->
        function loadFinished(){
            selectDatabase(selected);
        }
        window.onload = loadFinished;
    </script>
</head>
<body>
    <!-- 1.ナビゲーションバーの設定 -->
    <nav class="navbar navbar-expand-lg navbar-dark bg-dark sticky-top">
        <div class="container-fluid">
            <a class="navbar-brand" href="/" target="_top">DBBrowser</a>
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
    <div class="container-fluid">
        <div class="row">
            <div class="col-md-5">
            <label for="formFile" class="form-label">CSV To Table</label>
            <input class="form-control" type="file" id="formFile" accept="text/csv" onchange="OnFileSelect( this );">
            </div>
        </div>
        <div class="row">
            <div class="col-md-2">
                Databases<br />
                <select id="database" class="form-select" aria-label="databases" onchange="selectChange(event);">
                    <?
                    for( var i = 0 ; i < databases.length ; i++ ){
                        if( tables[i].length > 0){
                            print( '<option value="'+i+'"'+((i==db_id)?' selected':'')+'>'+databases[i]+'</option>\n' );
                        }
                    }
                    ?>
                </select>
                <p id="selected"></p>
            </div>
            <div class="col-md-10">
                <h3>SQL Browser</h3>
                <form name="SQL" method="post">
                    <!-- SQL入力エリア -->
                    <div class="form-group">
                        <label>Input SQL</label>
                        <textarea class="form-control" rows="6" id="sql" name="sql"><?print( sql2 );?></textarea>
                    </div>
                    <input type="hidden" id="db_id" name="db_id" value="<?print(db_id);?>">
                    <!-- 送信ボタン -->
                    <button type="submit" class="btn btn-primary">Execute SQL</button>
                </form>
                <div>
                    <?
                    //2次元テーブルの時
                    if( result !== undefined ){
                        var rset = Object.keys(result[0]);
                        print( '<table class="table table-striped table-bordered">\n');
                        if( rset.length > 0 ){
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
                        }
                        else
                        {
                            print( '<thead><tr><th>#</th><th>Items</th></tr></thead>\n' );
                            print( '<tbody>\n');
                            for( var row = 0 ; row < result.length ; row++ ){
                                print( '<tr><th>'+(row+1)+'</th><td>'+result[row]+'</td></tr>\n');
                            }
                        }
                        print( '</tbody>\n');
                        print( '</table>\n');
                    }else{
                        print( "RESULT:"+orgstr );
                    }
                    ?>
                </div>
            </div>
            <p id="dat"></p>
        </div>
        <div class="row">
            <div class="col-md-2">
                <input type="button" class="btn btn-primary" value="HELP" onClick="disp(hpt)">
            </div>
        </div>
    </div>
</body>
</html>

