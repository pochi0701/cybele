<?
    //session_start();
    var getaddr=function(){
        var data = eval(ssdp());
        if( data != undefined && data.length > 0 ){
            //print(data);
            for( var i = 0 ; i < data.length ; i++ ){
                var datum = data[i];
                if( datum.indexOf("hue-bridgeid")>=0){
                    var pos = datum.indexOf("http://");
                    //print( pos );
                    if( pos >= 0 ){
                        //print( datum.length);
                        datum = datum.substring(pos+7,datum.length);
                        pos = datum.indexOf("/");
                        //print( datum );
                        if( pos >= 0 ){
                            datum = datum.substr(0,pos);
                            return datum;
                        }
                    }
                }
            }
            //http://192.168.0.4:80/description.xml
        }
        return "";
    };
    var addr;//     = _SESSION["addr"];
    var username;// = _SESSION["username"];
    var enable   = 0;
    if( addr == undefined ){
        addr = getaddr();
        //_SESSION["addr"]     = addr;
    }
    if( username == undefined ){
        var rest = eval(restful("POST","http://"+addr+"/api","{\"devicetype\":\"devicetype\"}"));
        print( rest );
        if( rest[0]['error'] != undefined ){
            if( rest[0]['error']['type'] == 101){
                print( "PUSH LINK BUTTON AND RELOAD.");
            }else{
                print( "??ERROR??");
            }
        }else{
            username = rest[0]['success']['username'];
            print( username );
            enable = 1;
            //_SESSION["username"] = username;
        }
    }
    //設定
    if( enable == 1 ){
        var dt = "http://"+addr+"/api/"+username+"/lights/1/state";
        //print( dt+"\n");
        var rest2 = restful("PUT",dt,"{\"on\":false}");
        //rest2.dump();
        //print( rest2 );
    }
    
?>