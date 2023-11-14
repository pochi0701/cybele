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
//extern int compare(const wString arg1, const wString op, const wString arg2, const dataType type);
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
	Column(const wString& tbl, const wString& nam, const dataType typ = dataType::STRING);
	Column(const Column* clm);
	//テーブル名有無を考慮した合致選択 select = TableName.ColumnName
	int  Compare(wString select, map<wString, wString>& clmalias, map<wString, wString>& tblalias);
	int  GetAlias(map<wString, wString>& clmalias, map<wString, wString>& tblalias, wString& alias);
	void SaveToFile(bufrd* br);
	int  LoadFromFile(bufrd* br);
};
/////////////////////////////////////////////////////////////////////////////
enum class CMDS {
	TXSELECT = 1,
	TXCREATE = 2,
	TXINSERT = 3,
	TXUPDATE = 4,
	TXDELETE = 5,
	TXDROP = 6,
	TXFROM = 7,
	TXINTO = 8,
	TXWHERE = 9,
	TXSHOW = 10,
	TXORDER = 11,
	TXBY = 12,
	TXSET = 13,
	TXLIMIT = 14,
	TXTABLES = 15,
	TXTBL = 16,
	TXDATABASES = 17,
	TXDATABASE = 18,
	TXVALUES = 19,
	TXQUIT = 20,
	TXUSE = 21,
	TXAND = 22,
	TXOR = 23,
	TXAS = 24,
	TXHELP = 25,
	TXASC = 26,
	TXDESC = 27,
	TXPS = 28,
	TXPE = 29,
	TXCM = 30,
	TXED = 31,
	TXOP = 32,
	TXALTER = 33,
	TXADD = 34,
	TXAFTER = 35,
	TXTO = 36,
	TXMODIFY = 37,
	TXRENAME = 38,
	TXCOLUMN = 39,
	TXARG = 99,
	TXPRM = 98,
	TXNONE = -1,
	TXOTHER = -2
};
extern  CMDS  getToken(unsigned char* sql, unsigned char* token);
extern  CMDS  getData(unsigned char* data, unsigned char* token);
extern  int   chkToken(unsigned char* sql, unsigned char* token, CMDS& ret, CMDS cmd1, CMDS cmd2 = CMDS::TXOTHER);
/////////////////////////////////////////////////////////////////////////////
//条件クラス
class condition
{
public:
	vector<wString>      cond;
	vector<CMDS>         type;
	map<wString, wString> clmalias;
	map<wString, wString> tblalias;
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="">なし</param>
	condition(void) {
		cond.clear();
		type.clear();
		clmalias.clear();
		tblalias.clear();
	}
	void copy(condition& _cond) {
		//for cond
		cond.resize(_cond.cond.size());
		for (unsigned int i = 0; i < _cond.cond.size(); i++) {
			cond[i] = _cond.cond[i];
		}
		//for type
		type.resize(_cond.type.size());
		for (unsigned int i = 0; i < _cond.type.size(); i++) {
			type[i] = _cond.type[i];
		}
		//clmalias;
		clmalias = _cond.clmalias;
		tblalias = _cond.tblalias;
	}
	void put(char* arg, CMDS typ) {
		cond.push_back(arg);
		type.push_back(typ);
	}
};
/////////////////////////////////////////////////////////////////////////////
//テーブルクラス
class Table
{
private:
	int				ref;          //read thread数
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
	bool            changed;		// 変更されたか
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
	Table(const wString& myname, const vector<wString>& mycolumn);
	//カラム情報からテーブル作成
	Table(const wString& myname, const vector<Column*>& mycolumn);
	void copy(Table* tbl);
	//デストラクタ
	~Table();
	//テーブルデータ取得
	vector<Node*> getTable(void);
	//テーブルインサート(CSV)
	//カラム数が合ってない等チェックしてないので残りはNULLを追加
	int Insert(const char* data);
	//カラムとデータを指定
	int Insert(const vector<wString>& clm, const vector<wString>& data);
	//テーブルインサート
	int Insert(const vector<wString>& data);
	//条件配列生成
	int condition_mat(condition& cond, vector<char>& mat);
	//テーブル更新
	int Update(const vector<wString>& colnams, const vector<wString>& values, condition& cond);
	//テーブル削除
	int Delete(condition& cond);
	//復帰
	int  LoadFromFile(bufrd* br);
	//保存
	void SaveToFile(bufrd* br);
	//変更しらべ
	bool isChanged(void);
};

/////////////////////////////////////////////////////////////////////////////
class Database
{
private:
	wString name;
	map<wString, Table*> tblList;
	bool changed;
public:
	int ref;
	////////////////////////////////////////////////////////////////////////////
	Database(const wString& myname);
	////////////////////////////////////////////////////////////////////////////
	~Database(void);
	////////////////////////////////////////////////////////////////////////////
	void Save(void);
	////////////////////////////////////////////////////////////////////////////
	int SQL(const wString& sqltext, wString& retStr);
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
	////////////////////////////////////////////////////////////////////////////
	//データベースリスト
	////////////////////////////////////////////////////////////////////////////
	map<wString, Database*> dblist;
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
	Database* DBCreate(const wString& DBName);
	////////////////////////////////////////////////////////////////////////////
	//DBクローズ
	void DBClose(const wString& DBName);
	////////////////////////////////////////////////////////////////////////////
	unsigned int ShowCatalog(wString& retStr);
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
