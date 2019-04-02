<html>
<head>
<script>
// ツリー構造用データ
// value    : ノードが保持するデータ
// children : 子ノード
// now      : 作成日付
function Node(value, children) {
    this.value    = value;
    this.children = children;
    this.now      = new Date();
    //子供の親は自分
    if (children) {
        for (var i = 0; i < children.length; i++) {
            children[i].parent = this;
        }
    } 
}
function addChild(root,node)
{
    if( root.children ){
        root.children.push(node);
    }else{
        root.children = [node];
    }
    node.parent = root;
    return node;
}

function addBrother(root,node)
{
    //親の子供のthisの次に追加
}
// ツリー構造
function test(a,b)
{
    this.a = a;
    this.b = b;
}
var r = new test("a",new test("b"));
//document.write(JSON.stringify(a));



var root = new Node("Top");
var crnt;
var crnt2;
var crnt3;
var crnt4;
var crnt5;
crnt = addChild(root,new Node("Level1"));
//document.write(JSON.stringify(root));
crnt2 = addChild(crnt,new Node("Level2-1"));
crnt2 = addChild(crnt,new Node("Level2-2"));
crnt3 = addChild(crnt2,new Node("Level3"));
crnt2 = addChild(crnt,new Node("Level2-3"));
crnt2 = addChild(crnt,new Node("Level2-4"));
crnt3 = addChild(crnt2,new Node("Level3"));
crnt4 = addChild(crnt3,new Node("Level4"));
crnt5 = addChild(crnt4,new Node("Level5"));
/*
var root = new Node("Top", [
    new Node("Lebel1", [
        new Node("Lebel2-1"),
        new Node("Lebel2-2", [
            new Node("Lebel3")
        ]),
        new Node("Lebel2-3"),
        new Node("Lebel2-4", [
            new Node("Lebel3", [
                new Node("Lebel4")
            ])
        ])
    ])
]);
*/
// ツリー構造を検索
writeTree(root, 0);
var seek = seekTree(root,"Level2-4");
writeTree(seek,0);
function seekTree(target,label)
{
    if( target.value == label ){
        return target;
    }else if (target.children) {
        for (var i = 0; i < target.children.length; i++) {
            var ret = seekTree(target.children[i], label);
            if( ret != null ){
                return ret;
            }
        }
    } 
    return null;
}

/*
ツリー構造を表示
 
root : Node
indent : 表示のインデント
*/
function writeTree(root, indent) {
     
    // インデントを表示
    for (var i = 0; i < indent; i++) {
        document.write("-");
    }
 
    // 現在のツリー要素の値を表示
    document.writeln(root.value + "<br>");
 
    // 子ノードがあれば子ノードを表示
    if (root.children) {
        for (var i = 0; i < root.children.length; i++) {
            writeTree(root.children[i], indent + 1);
        }
    }
}
</script>
</head>
<body>
</body>
</html>

