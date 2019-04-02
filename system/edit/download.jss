<?
/*
    引数はターゲットファイルへの相対又は絶対パス
*/
function download_file(path_file)
{
    /* ファイルの存在確認 */
    if (!file_exists(path_file)) {
        die("Error: File(".path_file.") does not exist");
    }

    /* オープンできるか確認 */
    if (!(fp = fopen(path_file, "r"))) {
        die("Error: Cannot open the file(".path_file.")");
    }
    fclose(fp);

    /* ファイルサイズの確認 */
    if ((content_length = filesize(path_file)) == 0) {
        die("Error: File size is 0.(".path_file.")");
    }

    /* ダウンロード用のHTTPヘッダ送信 */
    header("Content-Disposition: attachment; filename=\"".basename(path_file)."\"");
    //header("Content-Disposition: inline; filename=\"".basename(path_file)."\"");
    header("Content-Length: ".content_length);
    header("Content-Type: application/octet-stream");

    /* ファイルを読んで出力 */
    if (!readfile(path_file)) {
        die("Cannot read the file(".path_file.")");
    }
}
?>
