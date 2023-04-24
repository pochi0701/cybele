<?
    var file="/sample/multimedia/musicPlayer/mailmonster.mp3";
    var name = JSON.mp3id3tag(_SERVER["DOCUMENT_ROOT"]+file);
    print(name);
?>