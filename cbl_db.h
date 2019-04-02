#ifndef CBDBH
#define CBDBH
//---------------------------------------------------------------------------
#include <vector>
#include <map>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include "node.hpp"
#include "cbl_String.h"
using namespace std;
//#define O_BINARY 0
//---------------------------------------------------------------------------
//比較
extern int compare(const wString arg1, const wString op , const wString arg2, const dataType type );
class view;
/////////////////////////////////////////////////////////////////////////////
//カラムクラス
class Column
{
private:
public:
    wString  table;//取得元テーブル名
    wString  name; //カラム名
    dataType type; //データ型
    //カラムコンストラクタ
    Column(void);
    Column(const wString& tbl, const wString& nam, const dataType typ=STRING);
    Column(const Column* clm);
    //テーブル名有無を考慮した合致選択 select = TableName.ColumnName
    int  Compare( wString select, map<wString,wString>& clmalias, map<wString,wString>& tblalias);
    int  GetAlias( map<wString,wString>& clmalias, map<wString,wString>& tblalias, wString& alias);
    void SaveToFile(bufrd& br);
    int  LoadFromFile(bufrd& br);
};
/////////////////////////////////////////////////////////////////////////////
typedef enum  {TXSELECT,TXCREATE,TXINSERT,TXUPDATE,TXDELETE,TXDROP,
               TXFROM,TXINTO,TXWHERE,TXSHOW,TXORDER,TXBY,TXSET,
               TXLIMIT,TXTABLES,TXTBL  ,TXDATABASES,TXDATABASE,
               TXVALUES,TXQUIT,TXUSE,TXAND,TXOR,TXAS,TXHELP,TXASC,
               TXDESC,TXPS,TXPE,TXCM,TXED,TXOP,TXALTER,TXADD,TXAFTER,
               TXTO,TXMODIFY,TXRENAME,TXCOLUMN,
               TXARG=99,TXPRM=98,TXNONE=-1,TXOTHER=-2} CTYP;
extern  CTYP  getToken(unsigned char* sql, unsigned char* token);
extern  CTYP  getData(unsigned char* data, unsigned char* token);
extern  int   chkToken(unsigned char* sql, unsigned char* token, CTYP& ret, CTYP cmd1,CTYP cmd2=TXOTHER);
/////////////////////////////////////////////////////////////////////////////
//条件クラス
class condition
{
public:
    vector<wString>      cond;
    vector<CTYP>         type;
    map<wString,wString> clmalias;
    map<wString,wString> tblalias;
    condition(void){
       cond.clear();
       type.clear();
       clmalias.clear();
       tblalias.clear();
    }
    void copy(condition& _cond){
        //for cond
        cond.resize(_cond.cond.size());
        for( size_t i = 0 ; i < _cond.cond.size() ; i++ ){
            cond[i] = _cond.cond[i];
        }
        //for type
        type.resize(_cond.type.size());
        for( size_t i = 0 ; i < _cond.type.size() ; i++ ){
            type[i] = _cond.type[i];
        }
        //clmalias;
        clmalias = _cond.clmalias;
        tblalias = _cond.tblalias;
    }
    void put(char* arg,CTYP typ){
        cond.push_back(arg);
        type.push_back(typ);
    }
};
/////////////////////////////////////////////////////////////////////////////
//テーブルクラス
class Table
{
private:
    int             ref;          //read thread数
#ifdef linux
    pthread_mutex_t mutex;        //クリティカルセッションmutex
#else
    CRITICAL_SECTION cs;          //クリティカルセクション
#endif
public:
    wString         name;         //テーブル名
    vector<Column*> column;       //カラム
    vector<Node*>   node;         //各ノード
    vector<int>     index;        //orderby等のフィルタ
    //セッション管理
    void readStart();
    void readEnd();
    void writeStart();
    void writeEnd();
    //テンプレート
    Table(void);
    //CSVからテーブル作成
    Table(const char* myname, const char* mycolumn);
    //String配列からテーブル作成
    Table(const wString myname, const vector<wString>& mycolumn);
    //カラム情報からテーブル作成
    Table(const wString myname, const vector<Column*> mycolumn);
    void copy(Table* tbl);
    //デストラクタ
    ~Table();
    //テーブルデータ取得
    vector<Node*> getTable(void);
    //テーブルインサート(CSV)
    //カラム数が合ってない等チェックしてないので残りはNULLを追加
    int Insert(const char* data);
    //カラムとデータを指定
    int Insert(const vector<wString>& clm,const vector<wString>& data);
    //テーブルインサート
    int Insert(const vector<wString>& data);
    //条件配列生成
    int condition_mat(condition& cond, vector<char>& mat);
    //テーブル更新
    int Update(const vector<wString>& colnams,const vector<wString>& values,condition& cond);
    //テーブル削除
    int Delete(condition& cond);
    //復帰
    int  LoadFromFile(bufrd& br);
    //保存
    void SaveToFile(bufrd& br);
    //変更しらべ
    int  changed(void);
};

/////////////////////////////////////////////////////////////////////////////
class Database
{
private:
    wString name;
    map<wString,Table*> tbllist;
public:
    int ref;
    ////////////////////////////////////////////////////////////////////////////
    Database(wString myname);
    ////////////////////////////////////////////////////////////////////////////
    ~Database(void);
    ////////////////////////////////////////////////////////////////////////////
    int SQL(const wString& sqltext,wString& retStr);
    ////////////////////////////////////////////////////////////////////////////
    int LoadFromFile(wString file);
    ////////////////////////////////////////////////////////////////////////////
    int SaveToFile(wString file);
};
#define CATFILE "database/_cybeleDBcat"
/////////////////////////////////////////////////////////////////////////////
class DBCatalog
{
private:
    int refs;
#ifdef linux
    pthread_mutex_t mutex;        //クリティカルセッションmutex
#else
    CRITICAL_SECTION cs;          //クリティカルセクション
#endif
public:
    map<wString,Database*> dblist;
    ////////////////////////////////////////////////////////////////////////////
    //シングルトン取得
    //static DBCatalog& getInstance(void);
    ////////////////////////////////////////////////////////////////////////////
    //データベースに接続する
    Database* DBConnect(const wString& DBName);
    ////////////////////////////////////////////////////////////////////////////
    //データベースを閉じる
    int DBClose(Database* db);
    ////////////////////////////////////////////////////////////////////////////
    //新規にDB作成
    Database* DBCreate(const wString DBName);
    ////////////////////////////////////////////////////////////////////////////
    //DBクローズ
    void DBClose(const wString& DBName);
    ////////////////////////////////////////////////////////////////////////////
    size_t ShowCatalog(wString& retStr);
    ////////////////////////////////////////////////////////////////////////////
    int LoadFromFile(void);
    ////////////////////////////////////////////////////////////////////////////
    int SaveToFile(void);
    ////////////////////////////////////////////////////////////////////////////
    //コンストラクタ
    DBCatalog(void);
    ////////////////////////////////////////////////////////////////////////////
    //デストラクタ
    ~DBCatalog(void);
};
extern DBCatalog* catalog;
#endif