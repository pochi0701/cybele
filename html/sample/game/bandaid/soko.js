//ウィンドウサイズ
var WIDTH=420;
var HEIGHT=290;

var xmlHttp;
var mX;
var mY;
var Fd;
var xml;
var mB=0;
var X;
var Y;
var gl;
var bg;
var wGap=20;
var hGap=20;
var Cood = function(xx, yy) {
    this.x = xx;
    this.y = yy;
};
let ctx;
var GoalCHK=0;
var change=0;

var img;
        // onload イベント
        window.onload = function() {
    ctx = document.getElementById('mainWindow').getContext('2d'); //描画するコンテキスト
    img = new Image();
    img.src = "./game.gif";
            img.onload = function() {
              loadText();
            };
            img.onerror = function() {
                alert("画像読み込みに失敗しました。");
            };
                
        };

        // mousemove イベント
        document.addEventListener('mousemove', function(event) {
            mouseMove(event.clientX, event.clientY);
        });

        // mousedown イベント
        document.addEventListener('mousedown', function(event) {
            mouseDown();
        });
        
function loadText(){
    GoalCHK = 0;
    if (window.XMLHttpRequest){
        xmlHttp = new XMLHttpRequest();
    }else{
        if (window.ActiveXObject){
            xmlHttp = new ActiveXObject("Microsoft.XMLHTTP");
        }else{
            xmlHttp = null;
        }
    }
    xmlHttp.onreadystatechange = checkStatus;
    xmlHttp.open("GET", "./screen/screen.2", true);
    xmlHttp.send(null);
}
//読み込み完了した
function checkStatus(){
    var i;
    var j;
    if (xmlHttp.readyState == 4 && xmlHttp.status == 200){
        xml = xmlHttp.responseText.split("\n");
        mX = Math.floor(xml[0]);
        mY = Math.floor(xml[1]);
        //alert(mY);
        
        Fd = new Array(mY);
        for (i = 0; i < mY; i++) {
            Fd[i] = new Array(mX);
        }
        
        for( i = 0; i < mY; i++){
            for( j= 0; j < mX; j++){
                Fd[i][j] = 1;
            }
        }
        
        for( i = 0; i < mY; i++){
            var s2 = xml[i+2];
            for( j= 0; j < mX; j++){
                var tch = s2.charAt(j);
                switch(tch){
                    case ' '://ROAD
                    Fd[i][j] = 0;
                    break;
                    case '#'://WALL
                    Fd[i][j] = 1;
                    break;
                    case '$'://BALL
                    Fd[i][j] = 2;
                    mB++;
                    break;
                    case '@'://ME
                    Fd[i][j] = 3;
                    X = i;
                    Y = j;
                    break;
                    case '.'://GOAL
                    Fd[i][j] = 4;
                    break;
                }
            }
        }
        
        gl = new Array(mB);
        bg = new Array(mB);
        for(i = 0 ; i < mB ; i++ ){
            gl[i] =  new Cood(0,0);
            bg[i] =  new Cood(0,0);
        }
        var j1 = 0;
        var k1 = 0;
        for(var l1 = 0; l1 < mX; l1++){
            for(var i2 = 0; i2 < mY; i2++){
                //GOAL
                if(Fd[i2][l1] == 4){
                    gl[j1].y = i2;
                    gl[j1].x = l1;
                    j1++;
                }
                //BALL
                if(Fd[i2][l1] == 2){
                    bg[k1].y = i2;
                    bg[k1].x = l1;
                    k1++;
                }
            }
            
        }
        draw();
        if(ctx){
            setInterval(main,500); //コンテキストが取得できたならmain()関数を 繰り返し実行する
        }
    }else{
        if(xmlHttp.status > 0 && xmlHttp.status != 200){
            alert("reload. this shuould be rewrite use axios."+xmlHttp.status);
        }
    }
}

// 一度描画したものは残りつづける為、毎回初期化する
function draw(){
    var i;
    var j;
    //画面の初期化
    //ctx.clearRect(0,0,WIDTH,HEIGHT);
    //ctx.save();
    //ctx.beginPath();
    for(i = 0; i < mY; i++){
        for(j = 0; j < mX; j++){
            ctx.drawImage(img, Fd[i][j]*wGap, 0, wGap, hGap,   j*wGap, i*wGap, wGap, hGap);
        }
    }
    for(var k = 0; k < mB; k++){
        if(Fd[gl[k].y][gl[k].x] == 0){
            ctx.drawImage(img, 4*wGap, 0, wGap, hGap, gl[k].x * wGap, gl[k].y * hGap,wGap,hGap);
        }else if(Fd[gl[k].y][gl[k].x] == 2){
            ctx.drawImage(img, 5*wGap, 0, wGap, hGap, gl[k].x * wGap, gl[k].y * hGap,wGap,hGap);
        }else if(Fd[gl[k].y][gl[k].x] == 3){
            ctx.drawImage(img, 6*wGap, 0, wGap, hGap, gl[k].x * wGap, gl[k].y * hGap,wGap,hGap);
        }
    }
    
    if(GoalCHK == 1){
        change = 1;
    }
    //ctx.save();
    
    //ここにアニメーション
    //ctx.beginPath();
    //ctx.strokeStyle='rgb(0,0,0)';
    //ctx.arc(character.x,character.y,10,0, Math.PI*2 ,true);
    //ctx.stroke();
    
    //空画面を復元
    //ctx.restore();
    //ctx.closePath();
}
function main()
{
    if( change ){
        change = 0;
        for(var i = 0; i < mB; i++){
            if(Fd[gl[i].y][gl[i].x] == 2)
            continue;
            GoalCHK = 1;
            break;
        }
        draw();
    }
}
function mouseMove(x, y){
    mouseX = x;
    mouseY = y;
}
function mouseDown(){
    var k = Math.floor((mouseY / hGap)-0.5);
    var l = Math.floor((mouseX / wGap)-0.5);
    if(Math.abs(l-Y)<Math.abs(k-X)){
        if(k < X){
            if(Fd[X - 1][Y] == 0 || Fd[X - 1][Y] == 4){
                Fd[X][Y] = 0;
                X--;
                Fd[X][Y] = 3;
                change++;
            } else if(Fd[X - 1][Y] == 2 && (Fd[X - 2][Y] == 0 || Fd[X - 2][Y] == 4)){
                Fd[X][Y] = 0;
                X--;
                Fd[X][Y] = 3;
                Fd[X - 1][Y] = 2;
                change++;
            }
        }else{
            if(Fd[X + 1][Y] == 0 || Fd[X + 1][Y] == 4){
                Fd[X][Y] = 0;
                X++;
                Fd[X][Y] = 3;
                change++;
            } else if(Fd[X + 1][Y] == 2 && (Fd[X + 2][Y] == 0 || Fd[X + 2][Y] == 4)){
                Fd[X][Y] = 0;
                X++;
                Fd[X][Y] = 3;
                Fd[X + 1][Y] = 2;
                change++;
            }
        }
    }else{
        if(l < Y){
            if(Fd[X][Y - 1] == 0 || Fd[X][Y - 1] == 4){
                Fd[X][Y] = 0;
                Y--;
                Fd[X][Y] = 3;
                change++;
            } else if(Fd[X][Y - 1] == 2 && (Fd[X][Y - 2] == 0 || Fd[X][Y - 2] == 4)){
                Fd[X][Y] = 0;
                Y--;
                Fd[X][Y] = 3;
                Fd[X][Y - 1] = 2;
                change++;
            }
        }else{
            if(Fd[X][Y + 1] == 0 || Fd[X][Y + 1] == 4){
                Fd[X][Y] = 0;
                Y++;
                Fd[X][Y] = 3;
                change++;
            } else if(Fd[X][Y + 1] == 2 && (Fd[X][Y + 2] == 0 || Fd[X][Y + 2] == 4)){
                Fd[X][Y] = 0;
                Y++;
                Fd[X][Y] = 3;
                Fd[X][Y + 1] = 2;
                change++;
            }
        }
    }
    main();
}

