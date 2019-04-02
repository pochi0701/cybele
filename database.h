#ifndef DATABASEH
#define DATABASEH
//---------------------------------------------------------------------------
#include <windows.h>
#include <vector>
#include <String>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <io.h>
#include <stdio.h>
#include "node.hpp"
using namespace std;
//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
//カラムクラス
class Column
{
public:
    string   table;//取得元テーブル名
    string   name; //カラム名
    string   alias;//エイリアス
    dataType type; //データ型
    //カラムコンストラクタ
    Column(void);
    Column(const string tbl, const string nam, const dataType typ=STRING);
    Column(const Column* clm);
    //void dump(void);
    //テーブル名有無を考慮した合致選択 select = TableName.ColumnName
    int Compare( string select);
    int SaveToFile(int fd);
    int LoadFromFile(int fd);
};
/////////////////////////////////////////////////////////////////////////////
//テーブルクラス
class Table
{
public:
    string          name;         //テーブル名
    string          alias;        //テーブルエイリアス
    vector<Column*> column;       //カラム
    vector<Node*>   node;         //各ノード
    vector<int>     index;        //orderby等のフィルタ
    int             ref;          //read thread数
#ifdef linux
    pthread_mutex_t mutex;        //クリティカルセッションmutex
#else
    CRITICAL_SECTION cs;          //クリティカルセクション
#endif
    //コンストラクタ
    //myname:テーブル名
    //mycolumn:カラム情報
    //void cd(void);
    //セッション管理
    void readStart();
    void readEnd();
    void writeStart();
    void writeEnd();
    //テンプレート
    Table(void);
    //CSVからテーブル作成
    Table(char* myname, char* mycolumn);
    //String配列からテーブル作成
    Table(string myname, vector<string> mycolumn);
    //カラム情報からテーブル作成
    Table(string myname, vector<Column*> mycolumn);
    void copy(const Table* tbl);
    //コピーコンストラクタ
    //Table(const Table& tbl);
    //デストラクタ
    ~Table();
    //テーブルデータ取得
    vector<Node*> getTable(void);
    //テーブルインサート(CSV)
    //カラム数が合ってない等チェックしてないので残りはNULLを追加
    int Insert(const char* data);
    //カラムとデータを指定
    int Table::Insert(const vector<string>& clm,const vector<string>& data);
    //テーブルインサート
    int Insert(const vector<char*>& data);
    //テーブル選択
    Table* Select(const char* selection);
    //テーブル選択
    Table* Select(const vector<string>& colnums);
    //並べ替え
    Table* OrderBy(string targetColumn,orderType type=ASC);
    //テーブルにカラム追加
    void AddColumn(vector<Column*> addColumn);
    //左結合Left join
    Table* LeftJoin(Table* joinTable, string arg1, char* op, string arg2);
    //intersection
    Table* Intersection(Table* Table2);
    //テーブル絞込条件
    Table* Where(const string arg1, const char* op, const string arg2);
    //比較
    int compare(const string arg1, const string op , const string arg2, const dataType type );
    //テーブル更新
    int Table::Update(const vector<string> colnams,const vector<string> values,const string arg1, const char* op, const string arg2);
    //テーブル削除
    int Table::Delete(const string arg1, const char* op, const string arg2);
    //シリアライズ
    string toString(void);
    //復帰
    int LoadFromFile(int fd);
    //保存
    int SaveToFile(int fd);
};
/////////////////////////////////////////////////////////////////////////////
typedef enum     {TXSELECT,TXCREATE,TXINSERT,TXUPDATE,TXDELETE,TXFROM,TXINTO,TXWHERE,TXSHOW,TXORDER,TXBY,TXSET,TXTABLES,TXTBL  ,TXDATABASE,TXVALUES,TXQUIT,TXUSE,TXAND,TXOR,TXPS,TXPE,TXCM,TXED,TXARG=99,TXNONE=-1} CTYP;
char   ccmd[][9]={"select","create","insert","update","delete","from","into","where","show","order","by","set","tables","table","database","values","quit","use","and","or","(" , ")", ",", ";",};
CTYP   ctyp[]   ={TXSELECT,TXCREATE,TXINSERT,TXUPDATE,TXDELETE,TXFROM,TXINTO,TXWHERE,TXSHOW,TXORDER,TXBY,TXSET,TXTABLES,TXTBL  ,TXDATABASE,TXVALUES,TXQUIT,TXUSE,TXAND,TXOR,TXPS,TXPE,TXCM,TXED,TXARG,TXNONE};
int    clen[]   ={       6,       6,       6,       6,       6,     4,     4,      5,     4,      5,   2,    3,       6,      5,         8,       6,     4,    3,    3,   2,   1,   1,   1,   1};  
size_t flen = sizeof(clen)/sizeof(clen[0]);
extern int getToken(unsigned char* sql, unsigned char* token);
extern int chkToken(unsigned char* sql, unsigned char* token, CTYP cmd);

/////////////////////////////////////////////////////////////////////////////
class Database
{
public:
    string name;
    map<string,Table*> tbllist;
    int ref;
    ////////////////////////////////////////////////////////////////////////////
    Database(string myname);
    ////////////////////////////////////////////////////////////////////////////
    ~Database(void);
    ////////////////////////////////////////////////////////////////////////////
    int SQL(unsigned char* sql);
    ////////////////////////////////////////////////////////////////////////////
    int LoadFromFile(string file);
    ////////////////////////////////////////////////////////////////////////////
    int SaveToFile(string file);
};
#define CATFILE "_cybeleDBcat"
/////////////////////////////////////////////////////////////////////////////
class DBCatalog
{
public:
    int refs;
    map<string,Database*> dblist;
    ////////////////////////////////////////////////////////////////////////////
    //シングルトン取得
    static DBCatalog& getInstance(void);
    ////////////////////////////////////////////////////////////////////////////
    //データベースに接続する
    Database* DBConnect(char* DBName);
    ////////////////////////////////////////////////////////////////////////////
    //新規にDB作成
    Database* DBCreate(char* DBName);
    ////////////////////////////////////////////////////////////////////////////
    //DBクローズ
    void DBClose(char* DBName);
    ////////////////////////////////////////////////////////////////////////////
    size_t ShowCatalog(void);
    ////////////////////////////////////////////////////////////////////////////
    int LoadFromFile(void);
    ////////////////////////////////////////////////////////////////////////////
    int SaveToFile(void);
    ////////////////////////////////////////////////////////////////////////////
    DBCatalog(void);
    ////////////////////////////////////////////////////////////////////////////
    ~DBCatalog(void);
};
static DBCatalog &catalog = DBCatalog::getInstance();
#endif