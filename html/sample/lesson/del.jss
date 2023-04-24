<?
    session_start();
    var userName = _GET.username;
    var student;

    var database;
    //student検索
    if( userName != undefined ){
        database = DBConnect("lesson");
        str = "delete from student where name='"+userName+"'";
        ret = database.SQL(str);
        database.DBDisConnect();
        if( ret == "OK" ){
            header('Location: ./index.jss');
        }
    }
?>
<!DOCTYPE html>
<html lang="ja">
<head>
    <title>Delete Student</title>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">    
    <!-- Bootstrap CSS -->
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-0evHe/X+R7YkIZDRvuzKMRqM+OrBnVFBL6DOitfPri4tjfHxaWutUpFmBp4vmVor" crossorigin="anonymous">
</head>
<body>

    <div class="container">
        <div class="row">
            <div class="col-md-4 offset-md-4">
                <div class="login-form bg-light mt-4 p-4">
                    <form action="" method="" class="row g-3">
                        <h4>Delete Student</h4>
                        <div class="col-12">
                            <label>Username</label>
                            <input type="text" name="username" class="form-control" placeholder="Username" <?if(userName!=undefined){print("value=\""+userName+"\"");}?>>
                        </div>
                        <div class="col-12">
                            <button type="submit" class="btn btn-primary float-end">Delete</button>
                        </div>
                    </form>
                    <hr class="mt-4">
                    <div class="col-12">
                        <p class="text-center mb-0">Add student <a href="add.jss">Add student</a></p>
                    </div>
                </div>
            </div>
        </div>
    </div>
    
    <!-- Bootstrap JS -->
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/js/bootstrap.bundle.min.js" integrity="sha384-pprn3073KE6tl6bjs2QrFaJGz5/SUsLqktiwsUTF55Jfv3qYSDhgCecCxMW52nD2" crossorigin="anonymous"></script>
</body>
</html>