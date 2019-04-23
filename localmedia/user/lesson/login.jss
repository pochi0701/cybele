<!DOCTYPE html>
<html lang="ja">
<? var msg=""; ?>
<head>
    <meta charset="utf-8">
    <title>nyanko Press</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="http://netdna.bootstrapcdn.com/bootstrap/3.3.6/css/bootstrap.min.css" rel="stylesheet">
    <style type="text/css">
body{
    background-color:#f5f5f5;
}
.form-signin
{
    max-width: 330px;
    padding: 15px;
    margin: 0 auto;
}
.form-signin .form-control
{
    position: relative;
    font-size: 16px;
    height: auto;
    padding: 10px;
    -webkit-box-sizing: border-box;
    -moz-box-sizing: border-box;
    box-sizing: border-box;
}
.form-signin .form-control:focus
{
    z-index: 2;
}
.form-signin input[type="text"]
{
    margin-bottom: -1px;
    border-bottom-left-radius: 0;
    border-bottom-right-radius: 0;
}
.form-signin input[type="password"]
{
    margin-bottom: 10px;
    border-top-left-radius: 0;
    border-top-right-radius: 0;
}
.account-wall
{
    margin-top: 80px;
    padding: 40px 0px 20px 0px;
    background-color: #ffffff;
    box-shadow: 0 2px 10px 0 rgba(0, 0, 0, 0.16);
}
.profile-img
{
    width: 250px;
    margin: 0 auto 10px;
    display: block;
}
    </style>
    <script src="http://code.jquery.com/jquery-1.12.0.min.js"></script>
    <script src="http://netdna.bootstrapcdn.com/bootstrap/3.3.6/js/bootstrap.min.js"></script>
</head>
<body>
<div class="container">
    <div class="row">
        <div class="col-sm-6 col-md-4 col-md-offset-4">
            <div class="account-wall">
                <div id="my-tab-content" class="tab-content">
                    <div class="tab-pane active" id="login">
                        <img class="profile-img" src="system/img/img_title1.jpg" alt="">
                        <? if( msg.length>0){ print("<span class=\"text-warning\">"+msg+"</span>\n");} ?>
                        <form class="form-signin" action="" method="POST">
                            <input type="text"     class="form-control" name="username" placeholder="Username" required autofocus>
                            <input type="password" class="form-control" name="password" placeholder="Password" required>
                            <input type="submit" class="btn btn-lg btn-primary btn-block" value="Sign In" />
                        </form>
                        <div id="tabs" data-tabs="tabs">
                            <p class="text-center"><a href="#register" data-toggle="tab">アカウント登録</a></p>
                        </div>
                    </div>
                    <div class="tab-pane" id="register">
                        <form class="form-signin" action="" method="POST">
                            <input type="text"     class="form-control" name="username" placeholder="User Name ..." required autofocus>
                            <input type="email"    class="form-control" name="email"    placeholder="Email Address ..." required>
                            <input type="password" class="form-control" name="password" placeholder="Password ..." required>
                            <input type="submit" class="btn btn-lg btn-primary btn-block" value="Sign Up" />
                        </form>
                        <div id="tabs" data-tabs="tabs">
                            <p class="text-center"><a href="#login" data-toggle="tab">ログイン</a></p>
                        </div>
                    </div>
                </div>
            </div>
        </div>
    </div>
</div>
<script type="text/javascript">

</script>
</body>
</html>