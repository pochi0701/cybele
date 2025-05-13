<?
path = _GET.path;
// ヘッダ
//header("Content-Type: application/json");
//読み込み処理
if (file_exists(path)) {
    code = loadFromFile(path);
    print(code);
}
?>