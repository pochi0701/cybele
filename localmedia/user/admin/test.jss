<?
    var database;
    database = DBConnect("_SYSTEM");
    var DBlist = eval(database.SQL("SHOW DATABASES;"));
    var TBlist = [];
    database.DBDisConnect();
    for( var i = 0 ; i < DBlist.length ; i++){
        database = DBConnect(DBlist[i]);
        TBlist[i] = [];
        var ls = database.SQL("SHOW TABLES;");
        var tbl = eval(ls);
        for( var j = 0 ; j < tbl.length ; j++){
            TBlist[i][j] = tbl[j];
        }
        database.DBDisConnect();
    }
?>
<!DOCTYPE html>
<html>
    <head>
        <meta charset="UTF-8">
        <title>DBAdmin</title>
        <link rel="stylesheet" href="//netdna.bootstrapcdn.com/bootstrap/3.1.1/css/bootstrap.min.css">
        <link rel="stylesheet" href="//netdna.bootstrapcdn.com/bootstrap/3.1.1/css/bootstrap-theme.min.css">
        <script type="text/javascript" src="//code.jquery.com/jquery-1.11.0.min.js"></script>   
        <script src="//netdna.bootstrapcdn.com/bootstrap/3.1.1/js/bootstrap.min.js"></script>
        <style type="text/css">
        <!--
        #header {
            height: 50px;
            background-color: #EEE;
            width: 100%;
            text-align: center;
        }
        #footer {
            height: 30px;
            background-color: #AAA;
            text-align: center;
        }
        @media (min-width:767px){
          .dropdown:hover > .dropdown-menu{
            display: block;
          }
        }
        .container {width: auto};
        -->
        </style>
    </head>
    <body>
        <div id="header">
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
          <a class="navbar-brand" href="#">DBAdmin</a>
        </div>
        <!-- 3.リストの配置 -->
        <div class="collapse navbar-collapse" id="nav-menu-1">
          <ul class="nav navbar-nav">
             <li ><a href="./test.jss">構造</a></li>
             <li ><a href="./test.jss">SQL</a></li>
             <li ><a href="index.php?p=16">検索</a></li>
             <li ><a href="index.php?signin=1">クエリ</a></li>
          </ul>
        </div>
      </nav>
      
        </div>
    <hr>
    <!--Button with dropdown menu-->
    <div class="btn-group">
        <a href="#" type="button" data-toggle="dropdown" class="dropdown-toggle">Action</span></a>
        <ul class="dropdown-menu">
            <li><a href="#">Action</a></li>
            <li><a href="#">Another action</a></li>
            <li class="divider"></li>
            <li><a href="#">Separated link</a></li>
        </ul>
    </div>
    <hr>
        <div id="contents" class="container">
            <div class="row">
              <div class="col-xs-3">
              <?
                print( "<div class=\"panel-group\" id=\"collapse\">\n");
                for( var i = 0 ; i < DBlist.length ; i++){
	                print( "<div class=\"panel panel-default\">\n");
		            print( "    <div class=\"panel-heading\">\n");
                    print( "        <h4 class=\"panel-title\">\n");
				    print( "            <a data-toggle=\"collapse\" data-parent=\"#collapse\" href=\"#collapse"+i+"\">\n");
					print( DBlist[i] );
				    print( "            </a>\n");
			        print( "        </h4>\n");
                    print( "    </div>\n");
		            print( "    <div id=\"collapse"+i+"\" class=\"panel-collapse collapse\">\n");
		            print( "        <div class=\"panel-body\">\n");
		            for( var j = 0 ; j < TBlist[i].length ; j++){
		                  print( TBlist[i][j]+"\n" );  
		            }
				    //print( "            Lorem ipsum dolor sit amet, consectetur adipiscing elit.\n");
		            print( "        </div>\n");
		            print( "    </div>\n");
	                print( "</div>\n");
                }
                print( "</div>\n");
              ?>
              </div>
              <div class="col-xs-9">
                テキスト１<br />
                テキスト２<br />
                テキスト３<br />
                テキスト４<br />
                テキスト５<br />
                テキスト６<br />
                テキスト７<br />
                テキスト８<br />
                テキスト９<br />
              </div>
            </div>
        </div>


        <div id="footer">フッター</div>
        </body>
</html>