<?
    function toCamelCase(str) {
      ary = str.split('_');
      str2 = ary.join('+');
      return str2;
    };
    // キャメルケースからスネークケースに変換（オブジェクト）.
    d1 = _GET.data;
    d2 = toCamelCase(d1);
?>
<!doctype html>
<html lang="ja">
<head>
  <meta charset="utf-8">
  <title>CaseConv.</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/css/bootstrap.min.css" rel="stylesheet"
    integrity="sha384-rbsA2VBKQhggwzxH7pPCaAqO46MgnOM80zW1RWuH61DGLwZJEdK2Kadq2F9CUG65" crossorigin="anonymous">
  <script src="http://code.jquery.com/jquery-1.12.0.min.js"></script>
  <script src="https://cdnjs.cloudflare.com/ajax/libs/jquery.qrcode/1.0/jquery.qrcode.min.js"></script>
  <script>

    result = <?print(d1);?>
  </script>
</head>

<body>
  <div class="container">
    <h1>Case Converter v0.00</h1>
    <form>
    <div class="row">
      <div class="col-3">
        <label for="casetext" class="form-label">variable name</label>
        <input type="text" name="data" class="form-control" id="casetext" value="<?print(d1);?>">
      </div>
    </div>
    <div class="row">
      <div class="col-3">   
        <label for="result" class="form-label">result</label>
        <input type="text" name="result" class="form-control" id="result" value="<?print(d1);?>">
      </div>
    </div>
     <button type="submit" class="btn btn-primary">Submit</button>
  </div>
  </form>
  <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/js/bootstrap.bundle.min.js"
    integrity="sha384-kenU1KFdBIe4zVF0s0G1M5b4hcpxyD9F7jL+jjXkk+Q2h455rYXK/7HAuoJl+0I4"
    crossorigin="anonymous">
  </script>
</body>

</html>