<?
    session_start();
    var userName = _GET.username;
    var grade = _GET.grade;
    var student;

    var database;
    //student検索
    if( userName !== undefined && grade !== undefined ){
        database = DBConnect("lesson");
        student=eval(database.SQL("select id from student order by id desc limit 1"));
        maxid = student[0]["id"]+1;
        str = "insert into student(id,grade,name) values("+maxid+","+grade+","+"'"+userName+"\');";
        ret = database.SQL(str);
        database.DBDisConnect();
        if( ret == "OK" )
        {
            header('Location: ./index.jss');
        }
    }
?>
<!DOCTYPE html>
<html lang="ja">
<head>
    <title>Add Student</title>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">    
    <!-- Bootstrap CSS -->
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-rbsA2VBKQhggwzxH7pPCaAqO46MgnOM80zW1RWuH61DGLwZJEdK2Kadq2F9CUG65" crossorigin="anonymous">
</head>
<body>

    <div class="container">
        <div class="row">
            <div class="col-md-4 offset-md-4">
                <div class="login-form bg-light mt-4 p-4">
                    <form action="" method="" class="row g-3">
                        <h4>Add Student</h4>
                        <div class="col-12">
                            <label>grade</label>
                            <select name="grade" class="form-control" aria-label="Default select">
                            <option value="1">Grade One</option>
                            <option value="2">Grade Two</option>
                            <option value="3">Grade Three</option>
                            <option value="4">Grade Four</option>
                            <option value="5">Grade Five</option>
                            <option value="6">Grade Six</option>
                            </select>
                        </div>
                        <div class="col-12">
                            <label>Username</label>
                            <input type="text" name="username" class="form-control" placeholder="Username" <?if(userName!=undefined){print("value=\""+userName+"\"");}?>>
                        </div>
                        <div class="col-12">
                            <button type="submit" class="btn btn-primary float-end">Add</button>
                        </div>
                    </form>
                    <hr class="mt-4">
                    <div class="col-12">
                        <p class="text-center mb-0">Delete student <a href="del.jss">Delete student</a></p>
                    </div>
                </div>
            </div>
        </div>
    </div>
    
    <!-- Bootstrap JS -->
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/js/bootstrap.bundle.min.js" integrity="sha384-kenU1KFdBIe4zVF0s0G1M5b4hcpxyD9F7jL+jjXkk+Q2h455rYXK/7HAuoJl+0I4" crossorigin="anonymous"></script>
</body>
</html>