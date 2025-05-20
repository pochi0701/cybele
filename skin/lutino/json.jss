<?
var base    = _SERVER.DOCUMENT_ROOT;
var _url = _GET['_url'];
var jsona = eval(loadFromFile(base+_url));
var s = Object.keys(_GET);
var ret = undefined;
if (jsona.length === undefined){
    var aaa = Object.keys(jsona);
    print(aaa[0]);
    print(jsona[aaa[0]]);
}
else
{
    print(jsona.length);
    for(var i = 0 ; i < jsona.length ; i++)
    {
        var eq = true;
        for( var j = 0 ; j < s.length ; j++ ){
            if(s[j].startsWith("_")<=0){
               if( jsona[i][s[j]] != _GET[s[j]]){
                   eq = false;
                   break;
               }
            }
        }
        if( eq )
        {
            ret = jsona[i];
            break;
        }
    }
    print(JSON.stringify(ret,0));
}
?>