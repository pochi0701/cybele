<?
//サイズ変換
function size_num_read(size) {
    var bytes = ["B", "KB", "MB", "GB", "TB"];
    for (var i = 0; i < 5; i++) {
        if (size > 1024) {
            size = size / 1024;
        } else {
            break;
        }
    }
    return (round(size * 100) / 100) + bytes[i];
}
//変数取得
var base = _SERVER.DOCUMENT_ROOT;
var root = _GET.root;
var newDir = _GET.dirName;
var newFile = _GET.FileName;
var del = _GET.del;
var renf = _GET.renf;
var rent = _GET.rent;
var search = _GET.search;
//ユーザ限定処理
//初回の処理
if (root === undefined || root.length == 0) {
    root = _SERVER.DOCUMENT_ROOT;
}
//右端の/をなくす
while (root[root.length - 1] == "/") {
    root = root.substring(0, root.length - 1);
}

//ファイル削除
if (del !== undefined && del.length > 0) {
    if (file_exists(del)) {
        unlink(del);
    } else if (dir_exists(del)) {
        if( rmdir(del)){
            print("<p class=\"text-danger\">"+del+"は削除できませんでした。</p>");        
        }
    }
// リネーム
} else if (renf !== undefined && rent !== undefined && renf.length > 0 && rent.length > 0) {
    if (file_exists(renf) && (!file_exists(rent))) {
        if(!rename(renf, rent)){
            print("<p class=\"text-danger\">"+renf+"は"+rent+"に変更できませんでした。</p>");
        }
    } else if (dir_exists(renf) && (!dir_exists(rent))) {
        if(!rename(renf, rent)){
            print("<p class=\"text-danger\">"+renf+"は"+rent+"に変更できませんでした。</p>");
        }
    } else if (file_exists(rent)) {
        print("<p class=\"text-danger\">すでに同名のファイルが存在します</p>");
    }
// パス名取得、作成
} else if (dir_exists(root) && newDir !== undefined && newDir.length > 0) {
    var path = root + "/" + newDir;
    if (!file_exists(path)) {
        if(!mkdir(path)){
            print("<p class=\"text-danger\">"+newDir+"は作成できませんでした。</p>");
        }
    }
    root = path;
    //ファイル名取得、作成
} else if (dir_exists(root) && newFile !== undefined && newFile.length > 0) {
    var path = root + "/" + newFile;
    if (!file_exists(path)) {
        if( !touch(path)){
            print("<p class=\"text-danger\">"+newFile+"は作成できませんでした。</p>");
        }
    }
}
var sf = root.substring(base.length, root.length);
if (sf == "") {
    sf = "/";
}
var me = _SERVER.SCRIPT_NAME;
print("<div>\n");
print("[" + sf + "]\n");
print("<table>\n");
//親ディレクトリ
var filePath = dirname(root);
if (filePath != "" && filePath.indexOf(base) >= 0) {
    var url = filePath;
    print("<tr><td><a href=\"#\" onClick='openDir(\"" + url + "\");'>[<i class=\"fas fa-angle-double-up fa-lg\"></i>]</a></td><td></td><td></td></tr>\n");
}
//ディレクトリの場合
if (dir_exists(root)) {
    //ディレクトリ読み込み
    var files = eval(scandir(root));
    for (var i = files.length - 1; i >= 0; i--) {
        if (basename(files[i]) == "." || basename(files[i]) == "..") {
            files.remove(files[i]);
        }
    }
    if (files.length > 0) {
        //check each folders
        for (var i = 0; i < files.length; i++) {
            var file = files[i];
            var filePath = root + "/" + basename(file);
            var filePathe = filePath;
            if (dir_exists(file)) {
                //make link tag
                var stat = eval(file_stat(file));
                var title = stat.date + " " + stat.permission;
                var url1 = filePathe;
                var url3 = root;
                print("<tr><td>\n");
                print("<div class=\"dropdown\">\n");
                print("<a href=\"#\" data-bs-toggle=\"dropdown\" class=\"dropdown-item\">[" + basename(file) + "]</a>\n");
                print("<ul class=\"dropdown-menu\">\n");
                print("  <li><a class=\"dropdown-item\" href=\"#\" onClick='openDir(\"" + url1 + "\");'>open</a></li>\n");
                print("  <li><a class=\"dropdown-item\" href=\"#\" onClick='renameFile(\"" + url3 + "\",\"" + basename(file) + "\");'>Rename</a></li>\n");
                print("  <li><a class=\"dropdown-item\" href=\"#\" onClick='deleteFile(\"" + url3 + "\",\"" + basename(file) + "\");'>Delete</a></li>\n");
                print("</ul>\n");
                print("</div>\n");
                print("</td></tr>\n");
            }
        }
        //check each files.
        for (var i = 0; i < files.length; i++) {
            var file = files[i];
            var filePath = root + "/" + basename(file);
            var filePathe = filePath;
            var filePathf = filePath.substring(base.length, filePath.length);
            if (file_exists(filePath)) {
                var ext = extractFileExt(file);
                var disp = 1;
                if (ext.toLowerCase() == "bak") {
                    disp = 1;
                } else if (ext == "swf" || ext == "jpg" || ext == "gif" || ext == "png") {
                    disp = 2;
                } else if (ext == "mp3") {
                    disp = 3;
                } else if (search.length > 0) {
                    var fil = loadFromFile(filePath);
                    if (fil.indexOf(search) < 0) {
                        disp = 0;
                    }
                }
                if (disp) {
                    //make link tag
                    var stat = eval(file_stat(filePath));
                    var title = stat.date + " " + stat.permission;
                    var url3 = root;
                    var wte = "";
                    if (disp == 1) {
                        wte = "loadFile(\"" + filePathe + "\");";
                    } else if (disp == 2) {
                        wte = "viewDataImage(\"" + filePathf + "\");";
                    } else if (disp == 3) {
                        wte = "viewDataSound(\"" + filePathf + "\");";
                    }
                    print("<tr><td>\n");
                    print("<div class=\"dropdown\">\n");
                    print("<a href=\"#\" data-bs-toggle=\"dropdown\" class=\"dropdown-item\">" + basename(file) + "</a>\n");
                    print("<ul class=\"dropdown-menu\">\n");
                    print("     <li><a class=\"dropdown-item\" href=\"#\" onClick='" + wte + "'>Edit</a></li>\n");
                    print("     <li><a class=\"dropdown-item\" href=\"#\" onClick='viewCode(\"" + filePathe + "\");'>View</a></li>\n");
                    print("     <li><a class=\"dropdown-item\" href=\"#\" onClick='renameFile(\"" + url3 + "\",\"" + basename(file) + "\");'>Rename</a></li>\n");
                    print("     <li><a class=\"dropdown-item\" href=\"#\" onClick='deleteFile(\"" + url3 + "\",\"" + basename(file) + "\");'>Delete</a></li>\n");
                    print("     <li><a class=\"dropdown-item\" href=\"#\" onClick='QRCode(\"" + filePathe + "\");'>QRCode</a></li>\n");
                    print(" </ul>\n");
                    print("</div>\n");
                    print("</td></tr>\n");
                }
            }
        }
    }
}
print("</table>\n");
print("</div>");
?>