#include "stdafx.h"
// ==========================================================================
//code=UTF8	tab=4
//
// Cybele:	Application SErver.
//
// 		cbl_db.cpp
//		$Revision: 1.0 $
//		$Date: 2018/02/12 21:11:00 $
//
// ==========================================================================
#include <vector>
#include <map>
#include <string>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#ifdef linux
#include <unistd.h>
#include <sys/time.h>
#define stricmp strcasecmp
#else
#include <io.h>
#include <windows.h>
//#include <dir.h>
#include <process.h>
#endif
#include "node.hpp"
#include "cbl_db.h"
#include "cbl_String.h"
#include "cbl_tools.h"
using namespace std;
/////////////////////////////////////////////////////////////////////////////
//#ifndef IGNNRE_PARAMETER
//#define IGNORE_PARAMETER(n) ((void)n)
//#endif
//#define CMDLINE
/////////////////////////////////////////////////////////////////////////////
void err(const char* msg)
{
#if 0
	//perror( msg );
	//omsg = msg;
#else
	debug_log_output(msg);
#endif
}
/////////////////////////////////////////////////////////////////////////////
/// <summary>
/// DB用コマンド
/// </summary>
const char* ccmd[] = { "select","create",   "insert",  "update","delete",
				   "drop",  "from",     "into",    "where", "show",
				   "order", "by",       "set",     "limit", "tables",
				   "table", "databases","database","values","quit",
				   "use",   "and",      "or",      "as",    "help",
				   "asc",   "desc",     "alter",   "add",   "after",
				   "to",    "modify",   "rename",  "column","like" };
/////////////////////////////////////////////////////////////////////////////
/// <summary>
/// ENUMでの表現
/// </summary>
CMDS   cmds[] = { CMDS::TXSELECT,CMDS::TXCREATE,  CMDS::TXINSERT,  CMDS::TXUPDATE,CMDS::TXDELETE,
				   CMDS::TXDROP, CMDS::TXFROM,     CMDS::TXINTO,    CMDS::TXWHERE, CMDS::TXSHOW,
				   CMDS::TXORDER,CMDS::TXBY,       CMDS::TXSET,     CMDS::TXLIMIT, CMDS::TXTABLES,
				   CMDS::TXTBL,  CMDS::TXDATABASES,CMDS::TXDATABASE,CMDS::TXVALUES,CMDS::TXQUIT,
				   CMDS::TXUSE,  CMDS::TXAND,      CMDS::TXOR,      CMDS::TXAS,    CMDS::TXHELP,
				   CMDS::TXASC,  CMDS::TXDESC,     CMDS::TXALTER,   CMDS::TXADD,   CMDS::TXAFTER,
				   CMDS::TXTO,   CMDS::TXMODIFY,   CMDS::TXRENAME,  CMDS::TXCOLUMN,CMDS::TXOP };
/////////////////////////////////////////////////////////////////////////////
/// <summary>
/// コマンド数
/// </summary>
unsigned int cmdNum = sizeof(cmds) / sizeof(cmds[0]);
/////////////////////////////////////////////////////////////////////////////
//比較
map<wString, Database*>* connects;
/////////////////////////////////////////////////////////////////////////////
//カタログ
DBCatalog* catalog;
/////////////////////////////////////////////////////////////////////////////
//比較
int compare(const wString arg1, const wString op, const wString arg2, const dataType type1, const dataType type2) {
	if (type1 == dataType::STRING && type2 == dataType::STRING) {
		if (op == "=") return arg1 == arg2;
		else if (op == ">") return arg1 > arg2;
		else if (op == ">=") return arg1 >= arg2;
		else if (op == "<") return arg1 < arg2;
		else if (op == "<=") return arg1 <= arg2;
		else if (op == "like") {
			//部分一致または後方一致
			if (arg2[0] == '%') {
				//部分一致
				if (arg2[arg2.length() - 1] == '%') {
					int res = arg1.find(arg2.substr(1, arg2.length() - 2));
					return res != wString::npos;
				}
				//後方一致
				else
				{
					//resが所定位置である検算が飛鳥
					int res = arg1.find(arg2.substr(1, arg2.length() - 1));
					return res != wString::npos;
				}
				//前方一致または後方一致
			}
			else {
				//前方一致
				if (arg2[arg2.length() - 1] == '%') {
					int res = arg1.find(arg2.substr(1, arg2.length() - 1));
					return (res == 0);
				}
				//ワイルドカードなし
				else
				{
					//とりあえず
					return arg1 == arg2;
				}
			}
		}
	}
	else if (type1 == dataType::NUMBER && type2 == dataType::NUMBER) {
		int iarg1 = atoi(arg1.c_str());
		int iarg2 = atoi(arg2.c_str());
		if (op == "=") return iarg1 == iarg2;
		else if (op == ">") return iarg1 > iarg2;
		else if (op == ">=") return iarg1 >= iarg2;
		else if (op == "<") return iarg1 < iarg2;
		else if (op == "<=") return iarg1 <= iarg2;
	}
	else {
		wString targ1 = arg1;
		wString targ2 = arg2;
		if (type1 == dataType::NUMBER) targ1 = atoi(targ1.c_str());
		if (type2 == dataType::NUMBER) targ2 = atoi(targ2.c_str());
		if (op == "=") return targ1 == targ2;
		else if (op == ">") return targ1 > targ2;
		else if (op == ">=") return targ1 >= targ2;
		else if (op == "<") return targ1 < targ2;
		else if (op == "<=") return targ1 <= targ2;
	}
	return 0;
}

class view {
private:
	vector<Table*>         Tables;
	vector<int>            RowNum;//テーブル毎のノード数(行数)
	//カラムは順番がテーブル順ではないのでpairでテーブル参照できるようにする
	vector<pair<int, int> > Column;//カラム情報(順番、テーブル番号)
	vector<int>            Node;
public:
	int                    flag;
	condition              cond;
	//コンストラクタ
	view(condition& _cond) {
		flag = 0;
		cond.copy(_cond);
	}
	//デストラクタ。クリティカルセクション抜ける
	~view(void) {
		for (unsigned int i = 0; i < Tables.size(); i++) {
			Tables[i]->readEnd();
		}
	}
	//テーブル参照を記録
	int Copy(Table* tbl)
	{
		//テーブル設定
		Tables.clear();
		Column.clear();
		Node.clear();
		tbl->readStart();
		Tables.push_back(tbl);

		int idx = (int)Tables.size() - 1;
		//カラムコピー
		Column.clear();
		for (unsigned int i = 0; i < tbl->column.size(); i++) {
			pair<int, int> clm;
			clm.first = i;
			clm.second = idx;
			Column.push_back(clm);
		}
		RowNum.push_back(tbl->node[0]->size());
		//条件を取得
		vector<char> mat;
		tbl->condition_mat(cond, mat);
		//ノードコピー
		if (tbl->node.size()) {
			for (unsigned int i = 0; i < tbl->node[0]->size(); i++) {
				if (mat[i]) {
					Node.push_back(i);
				}
			}
		}
		return 0;
	}
	/////////////////////////////////////////////////////////////////////////////
	//条件配列生成
	int conditionMatTables(Table* tbl, vector<char>& mat)
	{
		wString arg1;
		wString arg2;
		CMDS   typ1 = CMDS::TXAND;
		CMDS   typ2 = CMDS::TXAND;
		wString op;
		wString rarg1;
		wString rarg2;
		int  Tblno1 = -1;
		int  Tblno2 = -1;
		Table* colTbl1 = NULL;
		Table* colTbl2 = NULL;
		dataType dtyp1 = dataType::STRING;
		dataType dtyp2 = dataType::STRING;
		//新テーブルひな形
		if (cond.cond.size() % 4 != 0) {
			err("SELECT:Illigal condition setting");
			return -1;
		}
		vector<char> mat2;
		//！！ここで使った条件はなくなる
		tbl->condition_mat(cond, mat2);
		//連結行の分1を設定
		mat.clear();
		mat.resize(Node.size() * tbl->node[0]->size());
		int cnt = 0;
		for (unsigned int i = 0; i < Node.size(); i++) {
			for (unsigned int j = 0; j < tbl->node[0]->size(); j++) {
				mat[cnt++] = mat2[j];
			}
		}
		//各条件についてAND,a,>,4等
		for (unsigned int ptr = 0; ptr < cond.cond.size(); ) {
			CMDS lop;
			auto col1 = -1;
			auto col2 = -1;
			if (cond.type[ptr] == CMDS::TXOR || cond.type[ptr] == CMDS::TXAND) {
				lop = cond.type[ptr];
			}
			else {
				err("SELECT:Logical Operation Error");
				return -1;
			}
			//条件のコピー
			arg1 = cond.cond[ptr + 1];
			typ1 = cond.type[ptr + 1];
			op = cond.cond[ptr + 2];
			arg2 = cond.cond[ptr + 3];
			typ2 = cond.type[ptr + 3];
			//自テーブルカラムチェック
			for (unsigned int i = 0; i < Column.size(); i++) {
				int    idx = Column[i].first;
				int    tno = Column[i].second;
				Table* mytbl = Tables[tno];
				if (mytbl->column[idx]->Compare(arg1, cond.clmalias, cond.tblalias)) {
					if (col1 >= 0) { err("Column name arg1 is ambigous"); return -1; }
					col1 = idx;
					Tblno1 = tno;
					colTbl1 = mytbl;
					dtyp1 = mytbl->column[idx]->type;
				}
				if (mytbl->column[idx]->Compare(arg2, cond.clmalias, cond.tblalias)) {
					if (col2 >= 0) { err("Column name arg1 is ambigous"); return -1; }
					col2 = idx;
					Tblno2 = tno;
					colTbl2 = mytbl;
					dtyp2 = mytbl->column[i]->type;
				}
			}
			//参照テーブルカラムチェック
			for (unsigned int i = 0; i < tbl->column.size(); i++) {
				if (tbl->column[i]->Compare(arg1, cond.clmalias, cond.tblalias)) {
					if (col1 >= 0) { err("Column name arg1 is ambigous."); return -1; }
					col1 = i;
					Tblno1 = -1;
					colTbl1 = tbl;
					dtyp1 = tbl->column[i]->type;
				}
				if (tbl->column[i]->Compare(arg2, cond.clmalias, cond.tblalias)) {
					if (col2 >= 0) { err("Column name arg2 is ambigous."); return -1; }
					col2 = i;
					Tblno2 = -1;
					colTbl2 = tbl;
					dtyp2 = tbl->column[i]->type;
				}
			}
			//カラムがテーブルと合致しなくて、かつ文字列ではない場合
			if ((col1 < 0 && typ1 == CMDS::TXARG) || (col2 < 0 && typ2 == CMDS::TXARG)) {
				ptr += 4;
				continue;
			}
			//値は取れた
			rarg1 = arg1;
			rarg2 = arg2;
			//自テーブル全行について
			int ncnt = 0;
			wString work;
			for (unsigned int i = 0; i < Node.size(); i++) {
				//ノードの番号でなくて捕まえている値
				int ii = Node[i];
				if (Tblno1 >= 0 && col1 >= 0) rarg1 = colTbl1->node[col1]->getNodeNative(getno(ii, Tblno1));
				if (Tblno2 >= 0 && col2 >= 0) rarg2 = colTbl2->node[col2]->getNodeNative(getno(ii, Tblno2));
				//参照テーブル全行について
				for (unsigned int j = 0; j < tbl->node[0]->size(); j++) {
					if (Tblno1 < 0 && col1 >= 0) rarg1 = tbl->node[col1]->getNodeNative(j);
					if (Tblno2 < 0 && col2 >= 0) rarg2 = tbl->node[col2]->getNodeNative(j);
					//条件が合致するなら
					if (lop == CMDS::TXOR) {//OR
						if (mat[ncnt] == 0) {
							if (compare(rarg1, op, rarg2, dtyp1, dtyp2)) {
								mat[ncnt] = 1;
							}
						}
					}
					else {        //AND
						if (mat[ncnt] == 1) {
							if (!compare(rarg1, op, rarg2, dtyp1, dtyp2)) {
								mat[ncnt] = 0;
							}
						}
					}
					ncnt++;
				}
			}
			//４つ分消す
			cond.cond.erase(cond.cond.begin() + ptr, cond.cond.begin() + ptr + 4);
			cond.type.erase(cond.type.begin() + ptr, cond.type.begin() + ptr + 4);
		}
		return 0;
	}
	/////////////////////////////////////////////////////////////////////////////
	//テーブルintersection&条件により絞込
	int Add(Table* tbl)
	{
		vector<char> mat;
		tbl->readStart();
		conditionMatTables(tbl, mat);
		//ノード追加
		vector<int> node;
		int cnt = 0;
		for (unsigned int i = 0; i < Node.size(); i++) {
			for (unsigned int j = 0; j < tbl->node[0]->size(); j++) {
				if (mat[cnt++]) {
					node.push_back(makeno(Node[i], j, tbl->node[0]->size()));
				}
			}
		}
		//ベクターコピー
		Node.resize(node.size());
		copy(node.begin(), node.end(), Node.begin());

		//テーブル追加
		Tables.push_back(tbl);
		int idx = (int)Tables.size() - 1;
		RowNum.push_back(tbl->node[0]->size());
		//カラム追加
		for (unsigned int i = 0; i < tbl->column.size(); i++) {
			pair<int, int> clm;
			clm.first = i;
			clm.second = idx;
			Column.push_back(clm);
		}
		return 0;
	}
#if 0
	/////////////////////////////////////////////////////////////////////////////
	//テーブル絞込条件(他でやってるのでここではやることなさそう）
	void Where(condition& cond)
	{
		vector<char> mat;
		//条件配列生成
		if (conditionMatTables(cond, mat)) return;
		//転記ベクターに従って転記
		vector<int> node;
		for (unsigned int j = 0; j < Node.size(); j++) {
			if (mat[j]) {
				node.push_back(Node[i]);
			}
		}
		//ベクターコピー
		Node.resize(node.size());
		copy(node.begin(), node.end(), Node.begin());
	}
#endif
	/////////////////////////////////////////////////////////////////////////////
	//選択
	void Select(const vector<wString>& Clm)
	{
		//vector<int>    select;
		vector<pair<int, int> > tmpclm;
		for (auto i = 0U; i < (unsigned int)Clm.size(); i++) {
			for (int j = static_cast<int>(Column.size()) - 1; j >= 0; j--) {
				int tblno = Column[j].second;
				Table* tbl = Tables[tblno];
				unsigned int k = Column[j].first;
				if (tbl->column[k]->Compare(Clm[i], cond.clmalias, cond.tblalias)) {
					tmpclm.push_back(Column[j]);
					break;
				}
			}
		}
		//ベクターコピー
		Column.resize(tmpclm.size());
		copy(tmpclm.begin(), tmpclm.end(), Column.begin());
	}
	/////////////////////////////////////////////////////////////////////////////
	//リミット
	void Limit(vector<int>& limit)
	{
		unsigned int st = 0;
		unsigned int cnt;
		//設定
		if (limit.size() == 0) {
			return;
		}
		else if (limit.size() > 1) {
			st = limit[0];
			cnt = limit[1];
		}
		else {
			cnt = limit[0];
		}
		//Nodeを設定
		if (Node.size() > st + cnt) {
			Node.resize(st + cnt);
		}
		if (st > 0) {
			Node.erase(Node.begin(), Node.begin() + st - 1);
		}
		return;
	}
	/////////////////////////////////////////////////////////////////////////////
	//並べ替え
	void OrderBy(wString targetColumn, orderType type) {
		//Nodeから順列を抽出し、indexを作り反映する
		for (unsigned int i = 0; i < Column.size(); i++) {
			int tblno = Column[i].second;
			Table* tbl = Tables[tblno];
			unsigned int j = Column[i].first;

			//合致するカラム
			if (tbl->column[j]->Compare(targetColumn, cond.clmalias, cond.tblalias)) {
				vector<int> indexdt;
				vector<int> indexno;
				vector<int> node;
				//番号を取得
				for (unsigned int k = 0; k < Node.size(); k++) {
					indexdt.push_back(getno(Node[k], tblno));
					indexno.push_back(k);
				}
				tbl->node[j]->sort(indexdt, indexno, type);
				//番号に沿ってNodeを入れ替え
				node.resize(Node.size());
				for (unsigned int k = 0; k < indexno.size(); k++) {
					node[k] = Node[indexno[k]];
				}
				copy(node.begin(), node.end(), Node.begin());
				break;
			}
		}
	}
	//
	int makeno(int base, int add, int radix)
	{
		return base * radix + add;
	}
	int getno(int num, int idx)
	{
		for (auto i = (int)RowNum.size() - 1; i > idx; i--) {
			num /= RowNum[i];
		}
		return num % RowNum[idx];
	}
#if 0
	//結合join(inner,left,right)
	int Join(Table* jtbl) {
		jtbl->readStart();
		vector<char> mat;
		conditionMatTables(tbl, cond, mat);


		//ノード追加
		vector<int> node;
		int cnt = 0;
		int flag;
		if (LEFTJOIN) {
			for (unsigned int i = 0; i < Node.size(); i++) {
				flag = 1;
				for (unsigned int j = 0; j < tbl->node[0]->size(); j++) {
					if (mat[cnt++]) {
						node.push_back(makeno(i, j, tbl->node[0]->size()));
						flag = 0;
					}
				}
				//ないものを追加するにはどうするか？
				if (flag) {
				}
			}
		}
		else if (RIGHTJOIN) {
			//マトリックスの参照具合が違う
			for (unsigned int j = 0; j < tbl->node[0]->size(); j++) {
				flag = 1;
				for (unsigned int i = 0; i < Node.size(); i++) {
					if (mat[cnt++]) {
						node.push_back(makeno(i, j, tbl->node[0]->size()));
						flag = 0;
					}
				}
				//ないものを追加するにはどうするか？
				if (flag) {
				}

			}
			//ADDと同じ
		}
		else if (INNERJOIN) {
			for (unsigned int i = 0; i < Node.size(); i++) {
				flag = 1;
				for (unsigned int j = 0; j < tbl->node[0]->size(); j++) {
					if (mat[cnt++]) {
						node.push_back(makeno(i, j, tbl->node[0]->size()));
						flag = 0;
					}
				}
			}
		}
		//ベクターコピー
		Node.resize(node.size());
		copy(node.begin(), node.end(), Node.begin());

		//テーブル追加
		Tables.push_back(jtbl);
		int idx = Tables.size() - 1;
		RowNum.push_back(jtbl->node[0]->size());
		//カラム追加
		for (unsigned int i = 0; i < jtbl->column.size(); i++) {
			pair<int, int> clm;
			clm.first = i;
			clm.second = idx;
			Column.push_back(clm);
		}
		return 0;
	}
#endif
	/////////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// ノードサイズ
	/// </summary>
	/// <returns>ノードのサイズ</returns>
	int size(void)
	{
		return (int)Node.size();
	}

	/////////////////////////////////////////////////////////////////////////////
#ifdef CMDLINE
	wString toJSON(int row = -2)
	{
		wString temp;
		wString alias;

		if (row == -2) {
			//count(*)でないとき
			if (flag == 0) {
				temp.resize(Column.size() * Node.size() * 8);
				//復数DBの場合はtblも表示
				if (Tables.size() > 1) {
					for (auto i = 0; i < Column.size(); i++) {
						Table* tbl = Tables[Column[i].second];
						auto j = Column[i].first;
						if (tbl->column[j]->GetAlias(cond.clmalias, cond.tblalias, alias)) {
							temp.cat_sprintf("%s%s", (i ? "|" : ""), alias.c_str());
						}
						else {
							temp.cat_sprintf("%s%s.%s", (i ? "|" : ""), tbl->column[j]->table.c_str(), tbl->column[j]->name.c_str());
						}
					}
					//単一DBの場合はカラム名のみ
				}
				else {
					for (auto i = 0; i < Column.size(); i++) {
						Table* tbl = Tables[Column[i].second];
						auto j = Column[i].first;
						if (tbl->column[j]->GetAlias(cond.clmalias, cond.tblalias, alias)) {
							temp.cat_sprintf("%s%s", (i ? "|" : ""), alias.c_str());
						}
						else {
							temp.cat_sprintf("%s%s", (i ? "|" : ""), tbl->column[j]->name.c_str());
						}
					}
				}
				temp += "\n";
				//ノード表示
				if (Node.size()) {
					//各行
					for (auto i = 0; i < Node.size(); i++) {
						int ii = Node[i];
						//各カラム
						int cnt = 0;
						for (auto j = 0; j < Column.size(); j++) {
							int    ntbl = Column[j].second;
							Table* tbl = Tables[ntbl];
							auto k = Column[j].first;
							int no = getno(ii, ntbl);
							char work[1024];
							sprintf(work, "%s%s", (cnt++ ? "\t" : ""), tbl->node[k]->getNode(no).c_str());
							temp += work;
						}
						temp += "\n";
					}
				}
			}
			else {
				alias = "count";
				map<wString, wString>::iterator it;
				for (it = cond.clmalias.begin(); it != cond.clmalias.end(); it++) {
					wString select = it->second;
					if (select == "count(*)") {
						alias = it->first;
						break;
					}
				}
				temp.sprintf("%s\n%d\n", alias.c_str(), Node.size());
			}
			return temp;
			//カラムだけ表示
		}
		else if (row == -1) {
			if (Tables.size() > 1) {
				for (i = 0; i < Column.size(); i++) {
					Table* tbl = Tables[Column[i].second];
					j = Column[i].first;
					if (tbl->column[j]->GetAlias(cond.clmalias, cond.tblalias, alias)) {
						temp.cat_sprintf("%s%s", (i ? "|" : ""), alias.c_str());
					}
					else {
						temp.cat_sprintf("%s%s.%s", (i ? "|" : ""), tbl->column[j]->table.c_str(), tbl->column[j]->name.c_str());
					}
				}
			}
			else {
				for (i = 0; i < Column.size(); i++) {
					Table* tbl = Tables[Column[i].second];
					j = Column[i].first;
					if (tbl->column[j]->GetAlias(cond.clmalias, cond.tblalias, alias)) {
						temp.cat_sprintf("%s%s", (i ? "|" : ""), alias.c_str());
					}
					else {
						temp.cat_sprintf("%s%s", (i ? "|" : ""), tbl->column[j]->name.c_str());
					}
				}
			}
			temp += "\n";
			return temp;
			//ノードだけ表示
		}
		else if (row >= 0) {
			if (Node.size() > (unsigned int)row) {
				int ii = Node[row];
				//各カラム
				int cnt = 0;
				for (j = 0; j < Column.size(); j++) {
					int    ntbl = Column[j].second;
					Table* tbl = Tables[ntbl];
					k = Column[j].first;
					int no = getno(ii, ntbl);
					//char tmp[1024];
					sprintf(work, "%s%s", (cnt++ ? "\t" : ""), tbl->node[k]->getNode(no).c_str());
					temp += work;
				}
				temp += "\n";
			}
			return temp;
		}
		return temp;
	}
#else
	wString toJSON(void)
	{
		wString temp;
		wString alias;
		vector<wString> clm;
		if (flag == 0) {
			//復数DBの場合はtblも表示
			if (Tables.size() > 1) {
				for (auto i = 0; i < Column.size(); i++) {
					Table* tbl = Tables[Column[i].second];
					auto j = Column[i].first;
					//エイリアスあれば、エイリアス表示
					if (tbl->column[j]->GetAlias(cond.clmalias, cond.tblalias, alias)) {
						clm.push_back(alias);
					}
					else {
						clm.push_back(tbl->column[j]->table + "." + tbl->column[j]->name);
					}
				}
				//単一DBの場合はカラム名のみ
			}
			else {
				for (auto i = 0; i < Column.size(); i++) {
					Table* tbl = Tables[Column[i].second];
					auto j = Column[i].first;
					//エイリアスあれば、エイリアス表示
					if (tbl->column[j]->GetAlias(cond.clmalias, cond.tblalias, alias)) {
						clm.push_back(alias);
					}
					else {
						clm.push_back(tbl->column[j]->name);
					}
				}
			}
			//データ作る[{"aaa":1,"bbb":2},{"aaa":1,"bbb":2}]
			//ノード表示
			temp.resize(static_cast<int>(Column.size()) * static_cast<int>(Node.size()) * 8);
			temp = "[";
			if (Node.size()) {
				//各行
				for (auto i = 0; i < Node.size(); i++) {
					temp.cat_sprintf("%s{", (i ? "," : ""));
					int ii = Node[i];
					//各カラム
					for (auto j = 0; j < Column.size(); j++) {
						int    ntbl = Column[j].second;
						Table* tbl = Tables[ntbl];
						auto k = Column[j].first;
						int no = getno(ii, ntbl);
						char work[1024];
						sprintf(work, "%s\"%s\":%s", (j ? "," : ""), clm[j].c_str(), tbl->node[k]->getNode(no).c_str());
						temp += work;
					}
					temp += "}";
				}
			}
			temp += "]";
		}
		else {
			alias = "count";
			map<wString, wString>::iterator it;
			for (it = cond.clmalias.begin(); it != cond.clmalias.end(); it++) {
				wString select = it->second;
				if (select == "count(*)") {
					alias = it->first;
					break;
				}
			}
			temp.sprintf("{\"%s\":%d}", alias.c_str(), Node.size());
		}
		return temp;
	}
#endif
};

#ifndef linux
#define SJIS
#endif
#ifdef SJIS
/************************************************************
** <モジュールＩＤ> ：iskanji.h
** <モジュール名称> ：シフトJIS漢字判定マクロ
** <機能概要> ：シフトJIS漢字第１バイトまたは第２バイト
** を判定する
**
** <Creator> ：k.kanamuro 2002/05/02
*************************************************************/
/** シフトJIS漢字第１バイト判定マクロ */

#ifndef ISKANJI1
#define ISKANJI1(c) \
(((unsigned char)(c) > 0x80 && (unsigned char)(c) < 0xA0)\
|| ((unsigned char)(c) > 0xCF && (unsigned char)(c) < 0xF0))
#endif

/*************************************************************/
/** シフトJIS漢字第２バイト判定マクロ */

#ifndef ISKANJI2
#define ISKANJI2(c) \
((unsigned char)(c) > 0x3F && (unsigned char)(c) < 0xFE \
&& (unsigned char)(c) != 0x7F)
#endif
#endif
////////////////////////////////////////////////////////////////////////////
CMDS getToken(unsigned char* sql, unsigned char* token)
{
	unsigned char* p = sql;
	unsigned char* q = token;
	unsigned char  ch;
	CMDS ret;
	//TRIM
	while (*p && *p <= ' ') p++;
	//token
	ret = CMDS::TXARG;
	switch (*p) {
	case 0:
		*q = *p;
		return CMDS::TXNONE;
	case '`':
	case '\"':
	case '\'':
		ch = *p++;
		//バッククオート以外は文字列を指定しているとする
		if (ch == '\"' || ch == '\'') ret = CMDS::TXPRM;
		while (*p && *p != ch) {
			if (*p == '\\') {
				p++;
				*q++ = *p++;
			}
			else {
				*q++ = *p++;
			}
		}
		if (*p++ == 0) {
			err("Invalid quote");
			ret = CMDS::TXNONE;
		}
		break;
	case '=':
		*q++ = *p++;
		ret = CMDS::TXOP;
		break;
	case '>':
	case '<':
		*q++ = *p++;
		if (*p == '=') *q++ = *p++;
		ret = CMDS::TXOP;
		break;
	case '+':
	case '-':
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '.':
		while (*p && strchr("0123456789.", *p) != NULL) *q++ = *p++;
		ret = CMDS::TXPRM;
		break;
	case ',':*q++ = *p++; ret = CMDS::TXCM; break;
	case ';':*q++ = *p++; ret = CMDS::TXED; break;
	case '(':*q++ = *p++; ret = CMDS::TXPS; break;
	case ')':*q++ = *p++; ret = CMDS::TXPE; break;
	default:
		while (*p && strchr(" ,=><;()", *p) == NULL)    *q++ = *p++;
		*q = 0;
		//cmd?
		for (unsigned int i = 0; i < cmdNum; i++) {
			if (stricmp(reinterpret_cast<char*>(token), ccmd[i]) == 0) {
				strcpy(reinterpret_cast<char*>(token), ccmd[i]);
				ret = cmds[i];
				break;
			}
		}
		break;
	}
	*q = 0;
	//TRIM
	while (*p == ' ')  p++;
	//残りをコピー
	while (*p)         *sql++ = *p++;
	*sql = 0;
	return ret;
}
////////////////////////////////////////////////////////////////////////////
CMDS getData(unsigned char* data, unsigned char* token)
{
	unsigned char* p = data;
	unsigned char* q = token;
	unsigned char  ch;
	CMDS ret;
	//TRIM
	while (*p && *p <= ' ') p++;
	//token
	ret = CMDS::TXPRM;
	switch (*p) {
	case 0:
		*q = *p;
		return CMDS::TXNONE;
	case '\"':
	case '\'':
		ch = *p++;
		//バッククオート以外は文字列を指定しているとする
		while (*p && *p != ch) {
#ifdef SJIS
			if (ISKANJI1(*p)) {
				*q++ = *p++;
				if (ISKANJI2(*p)) {
					*q++ = *p++;
				}
				else {
					err("INVALID KANJI");
					break;
				}
			}
			else if (*p == '\\') {
				p++;
				*q++ = *p++;
			}
			else {
				*q++ = *p++;
			}
#else
			if (*p == '\\') {
				*p++;
				*q++ = *p++;
			}
			else {
				*q++ = *p++;
			}
#endif
		}
		if (*p++ == 0) {
			err("Invalid quote");
			ret = CMDS::TXNONE;
		}
		break;
	case ',':
		*q++ = *p++;
		ret = CMDS::TXCM;
		break;
	default:
		while (*p && *p != ' ' && *p != ',')    *q++ = *p++;
		break;
	}
	*q = 0;
	//TRIM
	while (*p == ' ')  p++;
	//残りをコピー
	while (*p)         *data++ = *p++;
	*data = 0;
	return ret;
}
////////////////////////////////////////////////////////////////////////////
//CMD Check
int chkToken(unsigned char* sql, unsigned char* token, CMDS& ret, CMDS cmd1, CMDS cmd2)
{
	ret = getToken(sql, token);
	return (cmd1 != ret && cmd2 != ret);
}
/////////////////////////////////////////////////////////////////////////////
//カラムクラス
//カラムコンストラクタ
Column::Column(void)
{
	table = "";//取得元テーブル名
	name = ""; //カラム名
	type = dataType::STRING; //データ型
}
/////////////////////////////////////////////////////////////////////////////
//カラムコンストラクタ
Column::Column(const wString& tbl, const wString& nam, const dataType typ)
{
	int cutAt = nam.find(".");
	if (cutAt == nam.npos) {
		table = tbl;
		name = nam;
	}
	else {
		table = nam.substr(0, cutAt);
		name = nam.substr(cutAt + 1);
	}
	type = typ;
}
/////////////////////////////////////////////////////////////////////////////
//カラムコンストラクタ
Column::Column(const Column* clm)
{
	table = clm->table;
	name = clm->name;
	type = clm->type;
}
/////////////////////////////////////////////////////////////////////////////
//テーブル名有無とエイリアスを考慮した合致選択 select = TableName.ColumnName
int Column::Compare(wString select, map<wString, wString>& clmalias, map<wString, wString>& tblalias)
{
	//全体比較
	if (clmalias.count(select)) {
		select = clmalias[select];
	}
	//切り出し
	int cutAt = select.find(".");
	if (cutAt == select.npos) {
		return (select == name);
	}
	else {
		wString ot = select.substr(0, cutAt);
		wString nm = select.substr(cutAt + 1);
		if (tblalias.count(ot)) ot = tblalias[ot];
		return (table == ot && nm == name);
	}
}
/////////////////////////////////////////////////////////////////////////////
//エイリアス名の取得
int Column::GetAlias(map<wString, wString>& clmalias, map<wString, wString>& tblalias, wString& alias)
{
	//clmaliasからsecondでループ比較
	map<wString, wString>::iterator it;
	for (it = clmalias.begin(); it != clmalias.end(); it++) {
		wString select = it->second;
		if (Compare(select, clmalias, tblalias)) {
			alias = it->first;
			return true;
		}
	}
	return false;
}
/////////////////////////////////////////////////////////////////////////////
//カラム保存
void Column::SaveToFile(bufrd* br)
{
	unsigned char  len;
	unsigned char  uctp;
	//取得元テーブル名保存
	len = (unsigned char)table.length();
	br->Write(&len, sizeof(unsigned char));
	br->Write(table.c_str(), len);
	//カラム名保存
	len = (unsigned char)name.length();
	br->Write(&len, sizeof(unsigned char));
	br->Write(name.c_str(), len);
	//タイプ名保存
	uctp = (unsigned char)type;
	br->Write(&uctp, sizeof(uctp));
}
/////////////////////////////////////////////////////////////////////////////
//カラム復帰
int Column::LoadFromFile(bufrd* br)
{
	unsigned char  len;
	unsigned char  uctp;
	char           work[256];

	//取得元テーブル名取得
	if (br->Read(&len, sizeof(unsigned char))) return -1;
	if (br->Read(work, len)) return -1;
	work[len] = 0;
	table = work;

	//カラム名取得
	if (br->Read(&len, sizeof(unsigned char))) return -1;
	if (br->Read(work, len)) return -1;
	work[len] = 0;
	name = work;

	//タイプ名取得
	if (br->Read(&uctp, sizeof(uctp))) return -1;
	type = (dataType)uctp;
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
//コンストラクタ
Table::Table(void)
{
	ref = 0;
	changed = false;
	//クリティカルセクション初期化
#ifdef linux
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init(&mutex, &attr);
#else
	InitializeCriticalSection(&cs);
#endif
}
/////////////////////////////////////////////////////////////////////////////
//CSVからテーブル作成
Table::Table(const char* myname, const char* mycolumn) {
	vector<wString> clmns;
	//クリティカルセクション初期化
	ref = 0;
	changed = false;
#ifdef linux
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init(&mutex, &attr);
#else
	InitializeCriticalSection(&cs);
#endif
	writeStart();
	name = myname;
	//clmnsにCSVインサート
	unsigned char work[4096] = { 0 };
	unsigned char token[256];
	strcpy(reinterpret_cast<char*>(work), mycolumn);
	for (;;) {
		auto ret = getData(work, token);
		if (ret != CMDS::TXPRM) {
			err("INVALID COLUMN NAME");
			writeEnd();
			return;
		}
		clmns.push_back(reinterpret_cast<char*>(token));
		ret = getData(work, token);
		if (ret != CMDS::TXCM) break;
	}
	for (unsigned int i = 0; i < clmns.size(); i++) {
		int ptr = clmns[i].find(":");
		auto typs = dataType::STRING;
		if (ptr != wString::npos) {
			wString tp_ = clmns[i].substr(ptr + 1);
			clmns[i] = clmns[i].substr(0, ptr);
			if (tp_ == "string") {
				typs = dataType::STRING;
			}
			else if (tp_ == "number") {
				typs = dataType::NUMBER;
			}
		}
		Column* clm = new Column(name, clmns[i], typs);
		column.push_back(clm);
		node.push_back(new Node(typs));
	}
	writeEnd();
}
/////////////////////////////////////////////////////////////////////////////
//String配列からテーブル作成
Table::Table(const wString myname, const vector<wString>& mycolumn) {
	//クリティカルセクション初期化
	ref = 0;
	changed = false;
#ifdef linux
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init(&mutex, &attr);
#else
	InitializeCriticalSection(&cs);
#endif
	writeStart();
	name = myname;
	for (unsigned int i = 0; i < mycolumn.size(); i++) {
		Column* clm = new Column(name, mycolumn[i]);
		column.push_back(clm);
		node.push_back(new Node(clm->type));
	}
	writeEnd();
}
/////////////////////////////////////////////////////////////////////////////
//カラム情報からテーブル作成
Table::Table(const wString myname, const vector<Column*> mycolumn) {
	//クリティカルセクション初期化
	ref = 0;
	changed = false;
#ifdef linux
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init(&mutex, &attr);
#else
	InitializeCriticalSection(&cs);
#endif
	writeStart();
	name = myname;
	for (unsigned int i = 0; i < mycolumn.size(); i++) {
		Column* clm = new Column(mycolumn[i]);
		column.push_back(clm);
		node.push_back(new Node(clm->type));
	}
	writeEnd();
}
/////////////////////////////////////////////////////////////////////////////
//デストラクタ
Table::~Table() {
	for (size_t i = 0; i < column.size(); i++) {
		delete column[i];
	}
	for (unsigned int i = 0; i < node.size(); i++) {
		delete node[i];
	}
#ifdef linux
#else
	//クリティカルセクション破棄
	DeleteCriticalSection(&cs);
#endif
}
/////////////////////////////////////////////////////////////////////////////
void Table::readStart(void)
{
	//書き込みセッションはここで待つ
#ifdef linux
	pthread_mutex_lock(&mutex);
#else
	EnterCriticalSection(&cs);
#endif
	ref++;
#ifdef linux
	pthread_mutex_unlock(&mutex);
#else
	LeaveCriticalSection(&cs);
#endif
}
/////////////////////////////////////////////////////////////////////////////
void Table::readEnd(void)
{
	ref--;
}
/////////////////////////////////////////////////////////////////////////////
void Table::writeStart(void)
{
	//読み込み終了待ち。
	//読み込みが半端無く続くようであれば抜けづらいだろうが、まあいいか
	while (ref) { Sleep(10); }
#ifdef linux
	pthread_mutex_lock(&mutex);
#else
	EnterCriticalSection(&cs);
#endif
}
/////////////////////////////////////////////////////////////////////////////
void Table::writeEnd(void)
{
#ifdef linux
	pthread_mutex_unlock(&mutex);
#else
	LeaveCriticalSection(&cs);
#endif
}
/////////////////////////////////////////////////////////////////////////////
//テーブルコピー
void  Table::copy(Table* tbl)
{
	tbl->writeStart();
	writeStart();
	name = tbl->name;             //テーブル名
	column.clear();
	for (unsigned int i = 0; i < tbl->column.size(); i++) {
		column.push_back(new Column(tbl->column[i]));
	}
	node.clear();
	for (unsigned int i = 0; i < tbl->node.size(); i++) {
		node.push_back(new Node(tbl->node[i]));
	}
	writeEnd();
	tbl->writeEnd();
	//vector<int>     index;        //orderby等のフィルタ
}
/////////////////////////////////////////////////////////////////////////////
/// <summary>
/// テーブルインサート(CSV)
/// カラム数が合ってない等チェックしてないので残りはNULLを追加
/// </summary>
/// <param name="data"></param>
/// <returns></returns>
int Table::Insert(const char* data) {
	vector<wString> tmp;
	unsigned char work[4096] = { 0 };
	unsigned char token[1024];
	writeStart();
	//実行開始
	strcpy(reinterpret_cast<char*>(work), const_cast<char*>(data));
	for (;;) {
		auto ret = getData(work, token);
		if (ret == CMDS::TXPRM) {
			tmp.push_back(reinterpret_cast<char*>(token));
		}
		else if (ret == CMDS::TXCM) {
			tmp.push_back("");
			continue;
		}
		else if (ret == CMDS::TXNONE) {
			tmp.push_back("");
			break;
		}
		else {
			err("INVALID DATA");
			break;
		}
		ret = getData(work, token);
		if (ret != CMDS::TXCM) break;
	}
	if (tmp.size() != column.size()) {
		err("Invalid number of column");
		writeEnd();
		return -1;
	}
	Insert(tmp);
	writeEnd();
	return 0;
}
//テーブルインサート
int Table::Insert(const vector<wString>& clm, const vector<wString>& data) {
	if (column.size() != clm.size() || clm.size() != data.size()) {
		err("Illigal number of data");
		return -1;
	}
	writeStart();
	//TODO:カラムと合致したデータを投入すること
	for (unsigned int i = 0; i < data.size(); i++) {
		node[i]->put(data[i].c_str());
	}
	writeEnd();
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
//テーブルインサート
int Table::Insert(const vector<wString>& data) {
	if (column.size() != data.size()) {
		err("Illigal number of data");
		return -1;
	}
	writeStart();
	for (unsigned int i = 0; i < data.size(); i++) {
		node[i]->put(data[i]);
	}
	writeEnd();
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
//条件配列生成
int Table::condition_mat(condition& cond, vector<char>& mat)
{
	//wString arg1;
	//wString arg2;
	//CMDS   typ1;
	//CMDS   typ2;
	//wString op;
	//wString rarg1;
	//wString rarg2;
	dataType dtyp1 = dataType::STRING;
	dataType dtyp2 = dataType::STRING;
	//新テーブルひな形
	if (cond.cond.size() % 4 != 0) {
		err("SELECT:Illigal condition setting");
		return -1;
	}
	//行の分1を設定
	mat.clear();
	mat.resize(node[0]->size());
	for (unsigned int i = 0; i < node[0]->size(); i++) {
		mat[i] = 1;
	}
	//各条件についてAND,a,>,4等
	for (unsigned int ptr = 0; ptr < cond.cond.size(); ) {
		CMDS lop;
		auto col1 = -1;
		auto col2 = -1;
		if (cond.type[ptr] == CMDS::TXOR || cond.type[ptr] == CMDS::TXAND) {
			lop = cond.type[ptr];
		}
		else {
			err("SELECT:Logical Operation Error");
			return -1;
		}
		//条件のコピー
		auto arg1 = cond.cond[ptr + 1];
		auto typ1 = cond.type[ptr + 1];
		auto op = cond.cond[ptr + 2];
		auto arg2 = cond.cond[ptr + 3];
		auto typ2 = cond.type[ptr + 3];
		//カラムチェック
		for (unsigned int i = 0; i < column.size(); i++) {
			if (column[i]->Compare(arg1, cond.clmalias, cond.tblalias)) {
				if (col1 >= 0) { err("Column name arg1 is ambigous"); return -1; }
				col1 = i;
				dtyp1 = column[i]->type;
			}
			if (column[i]->Compare(arg2, cond.clmalias, cond.tblalias)) {
				if (col2 >= 0) { err("Column name arg2 is ambigous"); return -1; }
				col2 = i;
				dtyp2 = column[i]->type;
			}
		}
		//カラムがテーブルと合致しなくて、かつ文字列ではない場合
		if ((col1 < 0 && typ1 == CMDS::TXARG) || (col2 < 0 && typ2 == CMDS::TXARG)) {
			ptr += 4;
			continue;
		}
		//コンディション計算
		auto rarg1 = arg1;
		auto rarg2 = arg2;
		for (unsigned int i = 0; i < node[0]->size(); i++) {
			if (col1 >= 0) rarg1 = node[col1]->getNodeNative(i);
			if (col2 >= 0) rarg2 = node[col2]->getNodeNative(i);
			//条件が合致するなら
			if (lop == CMDS::TXOR) {//OR
				if (mat[i] == 0) {
					if (compare(rarg1, op, rarg2, dtyp1, dtyp2)) {
						mat[i] = 1;
					}
				}
			}
			else {        //AND
				if (mat[i] == 1) {
					if (!compare(rarg1, op, rarg2, dtyp1, dtyp2)) {
						mat[i] = 0;
					}
				}
			}
		}
		//４つ分消す
		cond.cond.erase(cond.cond.begin() + ptr, cond.cond.begin() + ptr + 4);
		cond.type.erase(cond.type.begin() + ptr, cond.type.begin() + ptr + 4);
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
//テーブル更新
int Table::Update(const vector<wString>& colnams, const vector<wString>& values, condition& cond)
{
	//更新するカラムの番号
	vector<int> matcol;
	for (unsigned int i = 0; i < column.size(); i++) {
		for (unsigned int j = 0; j < colnams.size(); j++) {
			if (column[i]->Compare(colnams[j], cond.clmalias, cond.tblalias)) {
				matcol.push_back(i);
				break;
			}
		}
	}
	//合致しない更新カラムがあった場合
	if (matcol.size() != colnams.size()) { err("undefined column name found"); return -1; }
	writeStart();
	//条件配列生成
	vector<char> mat;
	if (condition_mat(cond, mat)) return -1;

	//行更新
	for (unsigned int i = 0; i < node[0]->size(); i++) {
		if (mat[i] == 1) {
			for (unsigned int j = 0; j < matcol.size(); j++) {
				//番号を指定して更新
				node[matcol[j]]->put(values[j].c_str(), i);
			}
		}
	}
	writeEnd();
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
//テーブル削除
int Table::Delete(condition& cond)
{
	//条件配列生成
	vector<char> mat;
	if (condition_mat(cond, mat)) return -1;
	writeStart();
	//後ろから実行
	for (int i = node[0]->size() - 1; i >= 0; i--) {
		//条件が合致するなら
		if (mat[i]) {
			for (unsigned int j = 0; j < node.size(); j++) {
				node[j]->Delete(i);//1行削除
				node[j]->changed = true;
			}
		}
	}
	writeEnd();
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
//復帰
int Table::LoadFromFile(bufrd* br)
{
	unsigned int len;
	char work[1024];
	writeStart();
	//name復帰
	for (;;) {
		if (br->Read(&len, sizeof(unsigned int))) break;
		if (br->Read(work, len)) break;
		work[len] = 0;
		name = work;

		//Column復帰
		column.clear();
		if (br->Read(&len, sizeof(unsigned int)))  break;
		for (unsigned int i = 0; i < len; i++) {
			Column* clm = new Column();
			if (clm->LoadFromFile(br) < 0)  break;
			column.push_back(clm);
		}

		//Node復帰
		node.clear();
		if (br->Read(&len, sizeof(unsigned int))) break;
		for (unsigned int i = 0; i < len; i++) {
			Node* nd = new Node(column[i]->type);
			if (nd->LoadFromFile(br) < 0)  break;
			node.push_back(nd);
		}
		//printf( "%s loaded\n", name.c_str() );
		writeEnd();
		return 0;
	}
	writeEnd();
	return -1;
}
/////////////////////////////////////////////////////////////////////////////
//保存
void Table::SaveToFile(bufrd* br)
{
	//name保存
	readStart();
	auto len = name.length();
	br->Write(&len, sizeof(unsigned int));
	br->Write(name.c_str(), len);

	//Column保存
	len = (int)column.size();
	br->Write(&len, sizeof(unsigned int));
	for (auto i = 0U; i < len; i++) {
		column[i]->SaveToFile(br);
	}

	//Node保存
	len = (int)node.size();
	br->Write(&len, sizeof(unsigned int));
	for (auto i = 0U; i < len; i++) {
		node[i]->SaveToFile(br);
	}
	//printf( "%s saved\n", name.c_str() );
	readEnd();
}
////////////////////////////////////////////////////////////////////////////
//テーブル変更の有無を調べる
bool Table::isChanged(void)
{
	//行更新
	if (this->changed)
	{
		return true;
	}
	for (unsigned int i = 0; i < node.size(); i++) {
		if (node[i]->changed) {
			return true;
		}
	}
	return false;
}
////////////////////////////////////////////////////////////////////////////
//データベースクラス
////////////////////////////////////////////////////////////////////////////
Database::Database(wString myname) {
	name = myname;
	wString path = current_dir + DELIMITER + "database" + DELIMITER;
	LoadFromFile(path + name + ".db");
	ref = 0;
	changed = false;
}
////////////////////////////////////////////////////////////////////////////
Database::~Database(void)
{
	this->Save();
}

/// <summary>
/// データベース、テーブルの永続化
/// </summary>
/// <param name=""></param>
void Database::Save(void)
{
	wString path = current_dir + DELIMITER + "database" + DELIMITER;
	//フォルダ作成
	if (!wString::DirectoryExists(path)) {
		wString::CreateDir(path);
	}

	//変更があればテーブル保存
	map<wString, Table*>::iterator it;
	for (it = tblList.begin(); it != tblList.end(); it++) {
		// テーブルを消された場合、全ファイル更新
		if (it->second->isChanged() || this->changed)
		{
			//bakファイル削除
			if (wString::FileExists((path + name + ".db.bak")) == 0) {
				wString::DeleteFile(path + name + ".db.bak");
			}
			wString::RenameFile(path + name + ".db", path + name + ".db.bak");
			//保存(テーブルも保存される）
			if (SaveToFile(path + name + ".db") == 0) {
			}
			break;
		}
	}
	//テーブル保存
	//map<wString,Table*>::iterator it;
	for (it = tblList.begin(); it != tblList.end(); it++) {
		delete it->second;
	}
}
////////////////////////////////////////////////////////////////////////////
//-1:エラー
int Database::SQL(const wString& sqltext, wString& retStr)
{
	condition        cond;
	unsigned char    token[256];
	unsigned char    token2[256];
	unsigned char    sql[2048] = { 0 };
	vector<Column*>  columns;
	vector<wString>  colnams;
	//　テーブル名
	vector<wString>  tables;
	vector<wString>  values;
	vector<wString>  order;
	orderType        orderTyp = orderType::ASC;
	vector<int>      limit;
	Table* tbl;
	view* vw;
	strcpy(reinterpret_cast<char*>(sql), reinterpret_cast<char*>(sqltext.c_str()));
	CMDS ret = getToken(sql, token);
	if (ret == CMDS::TXNONE || ret == CMDS::TXOTHER) {
		return -1;
	}
	switch (ret) {
		//SELECT Field1,Field2,.. FROM table1,table2.. WHERE ARG1 OP1 ARG2 (AND/OR ARG3 OP2 ARG4) ORDER BY ARG1,ARG2.. ASC/DESC;
	case  CMDS::TXSELECT://SELECT
		//カラム名取得
		for (;;) {
			if (chkToken(sql, token2, ret, CMDS::TXARG)) { err("SELECT NO ARG ERROR");   return -1; }//ARG
			//countの対応
			if (stricmp(reinterpret_cast<char*>(token2), "count") == 0) {
				if (chkToken(sql, token2, ret, CMDS::TXPS)) { err("SELECT NO ARG ERROR");   return -1; }//ARG
				if (chkToken(sql, token2, ret, CMDS::TXARG)) { err("SELECT NO ARG ERROR");   return -1; }//ARG
				if (chkToken(sql, token2, ret, CMDS::TXPE)) { err("SELECT NO ARG ERROR");   return -1; }//ARG
				strcpy(reinterpret_cast<char*>(token2), "count(*)");
			}
			colnams.push_back(reinterpret_cast<char*>(token2));

			ret = getToken(sql, token);
			if (ret == CMDS::TXAS) {
				if (chkToken(sql, token, ret, CMDS::TXARG)) { err("SELECT NO ARG ERROR");   return -1; }//ARG
				//エイリアス登録
				cond.clmalias[reinterpret_cast<char*>(token)] = reinterpret_cast<char*>(token2);
				ret = getToken(sql, token);
			}
			if (ret == CMDS::TXFROM)               break;
			else if (ret != CMDS::TXCM) { err("SELECT SYNTAX ERROR");   return -1; }//","
		}
		//テーブル名取得
		for (;;) {
			if (chkToken(sql, token2, ret, CMDS::TXARG)) { err("SELECT NO TABLE ERROR"); return -1; }//table
			tables.push_back(reinterpret_cast<char*>(token2));
			ret = getToken(sql, token);
			if (ret == CMDS::TXAS) {
				if (chkToken(sql, token, ret, CMDS::TXARG)) { err("SELECT NO ARG ERROR");   return -1; }//ARG
				//テーブルエイリアス登録
				cond.tblalias[reinterpret_cast<char*>(token)] = reinterpret_cast<char*>(token2);
				ret = getToken(sql, token);
			}
			if (ret != CMDS::TXCM)                 break;
		}
		//WHERE取得
		if (ret == CMDS::TXWHERE) {
			cond.put(const_cast<char*>("AND"), CMDS::TXAND);           //初回は1にANDする
			for (;;) {
				if (chkToken(sql, token, ret, CMDS::TXARG, CMDS::TXPRM)) { err("SELECT NO ARG IN WHERE"); return -1; }//arg1
				cond.put(reinterpret_cast<char*>(token), ret);

				if (chkToken(sql, token, ret, CMDS::TXOP)) { err("SELECT NO OPE IN WHERE"); return -1; }//op1
				cond.put(reinterpret_cast<char*>(token), ret);

				if (chkToken(sql, token, ret, CMDS::TXARG, CMDS::TXPRM)) { err("SELECT NO ARG IN WHERE"); return -1; }//arg2
				cond.put(reinterpret_cast<char*>(token), ret);

				if (chkToken(sql, token, ret, CMDS::TXAND, CMDS::TXOR)) break;
				cond.put(reinterpret_cast<char*>(token), ret);
			}
		}
		//ORDER BY 取得
		if (ret == CMDS::TXORDER) {
			if (chkToken(sql, token, ret, CMDS::TXBY)) { err("SYNTAX ERROR IN ORDER BY"); return -1; }//arg1
			if (chkToken(sql, token, ret, CMDS::TXARG)) { err("ORDER BY NO ARG IN WHERE"); return -1; }//arg1
			order.push_back(reinterpret_cast<char*>(token));
			if (!chkToken(sql, token, ret, CMDS::TXASC, CMDS::TXDESC)) {
				orderTyp = (ret == CMDS::TXASC) ? orderType::ASC : orderType::DESC;
				ret = getToken(sql, token);
			}
		}
		//LIMIT 取得
		if (ret == CMDS::TXLIMIT) {
			if (chkToken(sql, token, ret, CMDS::TXPRM)) { err("LIMIT NO ARG IN LIMIT"); return -1; }//arg
			limit.push_back(atoi(reinterpret_cast<char*>(token)));

			ret = getToken(sql, token);
			if (ret == CMDS::TXCM) {
				if (chkToken(sql, token, ret, CMDS::TXPRM)) { err("LIMIT NO ARG IN LIMIT"); return -1; }//arg
				limit.push_back(atoi(reinterpret_cast<char*>(token)));
				ret = getToken(sql, token);
			}
		}
		//末尾の処理
		if (ret != CMDS::TXED && ret != CMDS::TXNONE) {
			err("SYNTAX ERROR IN SELECT");
			return -1;
		}
		//テーブル存在チェック
		for (unsigned int i = 0; i < tables.size(); i++) {
			if (tblList.count(tables[i]) == 0) {
				err("Table not found");
				return -1;
			}
		}
		//テーブル取得
		vw = new view(cond);
		for (unsigned int i = 0; i < tables.size(); i++) {
			if (i == 0) {
				vw->Copy(tblList[tables[i]]);
			}
			else {
				vw->Add(tblList[tables[i]]);
			}
		}
		//WHERE実行
		if (vw->cond.cond.size()) {
			err("Is there more condition?");
			delete vw;
			return -1;
			//vw->Where(cond);
		}
		//関数系は別途やる。adhocで支持いれておく
		if (colnams[0] == "count(*)") {
			vw->flag = 1;
			retStr = vw->toJSON();
			delete vw;
		}
		else {
			//orderby 実行
			if (order.size()) {
				vw->OrderBy(order[0], orderTyp);
			}
			//select実行
			if (colnams.size() && colnams[0] != "*") {
				vw->Select(colnams);
			}
			//limit実行
			if (limit.size()) {
				vw->Limit(limit);
			}
			retStr = vw->toJSON();
			delete vw;
		}
		return 1;
		//CREATE TABLE tableName (field number/string, field number/string);
	case  CMDS::TXCREATE://CREATE
		ret = getToken(sql, token);
		// CREATE TABLE
		if (ret == CMDS::TXTBL) {
			if (chkToken(sql, token, ret, CMDS::TXARG)) { err("CREATE TABLE NO TABLE ERROR"); return -1; }//tablename
			tables.push_back(reinterpret_cast<char*>(token));

			ret = getToken(sql, token);
			// CREATE TABLE(
			if (ret == CMDS::TXPS) {
				for (;;) {
					if (chkToken(sql, token, ret, CMDS::TXARG)) { err("CREATE TABLE INVALID ARGS");   return -1; }//field name
					if (chkToken(sql, token2, ret, CMDS::TXARG) || (stricmp(reinterpret_cast<char*>(token2), "number") != 0 && stricmp(reinterpret_cast<char*>(token2), "string") != 0))
					{
						err("CREATE TABLE INVALID ARGS");   return -1;
					}//number/string
					dataType typ = (stricmp(reinterpret_cast<char*>(token2), "number") == 0) ? dataType::NUMBER : dataType::STRING;
					Column* clm = new Column(tables[0], reinterpret_cast<char*>(token), typ);
					columns.push_back(clm);
					ret = getToken(sql, token);
					if (ret == CMDS::TXPE)          break;
					else if (ret != CMDS::TXCM) { err("CREATE TABLE SYNTAX ERROR");   return -1; }//number/string
				}
				//テーブル作成
				//チェック。既に同名のテーブルがないか？
				if (tblList.count(tables[0])) {
					for (unsigned int i = 0; i < columns.size(); i++) {
						delete columns[i];
					}
					err("Table already exists");
					return -1;
				}
				Table* ntbl = new Table(tables[0], columns);//永続化
				ntbl->changed = true;
				tblList[tables[0]] = ntbl;
				for (unsigned int i = 0; i < columns.size(); i++) {
					delete columns[i];
				}
			}
			// CREATE TABLE FROM
			else if (ret == CMDS::TXFROM) {
				if (chkToken(sql, token, ret, CMDS::TXARG, CMDS::TXPRM)) { err("No file assigned");            return -1; }
				FILE* rs = fopen(reinterpret_cast<char*>(token), "r");
				if (rs) {
					char work[4096];
					fgets(work, 4096, rs);
					while (*work && work[strlen(work) - 1] < ' ') work[strlen(work) - 1] = 0;
					Table* ntbl = new Table(tables[0].c_str(), work);
					//int num = 0;
					while (!feof(rs)) {
						memset(work, 0, sizeof(work));
						fgets(work, 4096, rs);
						while (*work && work[strlen(work) - 1] < ' ') work[strlen(work) - 1] = 0;
						if (strlen(work) == 0) break;
						ntbl->Insert(reinterpret_cast<char*>(work));
						//if ((num++ % 1000) == 0) {
						//	printf("%d\n", num);
						//}
					}
					fclose(rs);
					tblList[tables[0]] = ntbl;
				}
				else
				{
					err("No file assigned");
					return -1;
				}
			}
			break;
		}
		else if (ret == CMDS::TXDATABASE) {
			if (chkToken(sql, token, ret, CMDS::TXARG)) { err("CREATE DATABASE NO TABLE ERROR"); return -1; }//tablename
			if (catalog->dblist.count(reinterpret_cast<char*>(token))) { err("DATABASE ALREADY EXISTS");        return -1; }//dbname
			//token名でデータベース作成
			catalog->DBCreate(wString(reinterpret_cast<char*>(token)));
			break;
		}
		else {
			err("What to do?");
			return 0;
		}
		//break;
	case  CMDS::TXINSERT://INSERT INTO TABLENAME(fieldname1,fieldname2,..) VALUES(value1,value2,...);
		if (chkToken(sql, token, ret, CMDS::TXINTO)) { err("INSERT SYNTAX ERROR"); return -1; }//INTO
		if (chkToken(sql, token, ret, CMDS::TXARG)) { err("INSERT NO TABLE NAME"); return -1; }//TABLENAME
		tables.push_back(reinterpret_cast<char*>(token));
		if (chkToken(sql, token, ret, CMDS::TXPS)) { err("INSERT SYNTAX ERROR"); return -1; }//(
		for (;;) {
			ret = getToken(sql, token);
			if (ret == CMDS::TXARG) {
				colnams.push_back(reinterpret_cast<char*>(token));
			}
			else if (ret == CMDS::TXCM) {
				continue;
			}
			else if (ret == CMDS::TXPE) {
				break;
			}
			else {
				err("INSERT INVALID FIELD NAME");
				return -1;//Field Name
			}
		}
		if (chkToken(sql, token, ret, CMDS::TXVALUES)) { err("INSERT SYNTAX ERROR"); return -1; }//VALUES
		if (chkToken(sql, token, ret, CMDS::TXPS)) { err("INSERT SYNTAX ERROR"); return -1; }//(
		for (;;) {
			ret = getToken(sql, token);
			if (ret == CMDS::TXARG || ret == CMDS::TXPRM) {
				values.push_back(reinterpret_cast<char*>(token));
			}
			else if (ret == CMDS::TXCM) {
				continue;
			}
			else if (ret == CMDS::TXPE) {
				break;
			}
			else {
				err("INSERT INVALID FIELD NAME");
				return -1;//Field Name
			}
		}
		//INSERT実行
		//テーブル選択
		if (tblList.count(tables[0]) == 0) {
			err("TABLE NOT FOUND");
			return -1;
		}
		if (tblList[tables[0]]->Insert(colnams, values)) {
			return -1;
		}
		break;
	case  CMDS::TXUPDATE://UPDATE TABLENAME SET field = value, field = value.. WHERE cond1,op1,cond2;
		//テーブル名取得
		if (chkToken(sql, token, ret, CMDS::TXARG)) { err("UPDATE NO TABLE ERROR"); return -1; }//table
		tables.push_back(reinterpret_cast<char*>(token));

		if (chkToken(sql, token, ret, CMDS::TXSET)) { err("UPDATE SYNTAX ERROR");   return -1; }//SET
		//カラム名取得
		for (;;) {
			if (chkToken(sql, token, ret, CMDS::TXARG)) { err("UPDATE NO ARG ERROR");   return -1; }//ARG
			colnams.push_back(reinterpret_cast<char*>(token));
			if (chkToken(sql, token, ret, CMDS::TXOP)) { err("UPDATE SYNTAX ERROR");   return -1; }//=
			if (strcmp(reinterpret_cast<char*>(token), "=") != 0) { err("UPDATE SYNTAX ERROR");   return -1; }//=

			if (chkToken(sql, token, ret, CMDS::TXARG, CMDS::TXPRM)) { err("UPDATE NO VALUE ERROR"); return -1; }//ARG
			values.push_back(reinterpret_cast<char*>(token));
			ret = getToken(sql, token);
			if (ret != CMDS::TXCM)                       break;
		}
		//WHERE取得
		if (ret == CMDS::TXWHERE) {
			cond.put(const_cast<char*>("AND"), CMDS::TXAND);               //初回は1にANDする
			for (;;) {
				if (chkToken(sql, token, ret, CMDS::TXARG, CMDS::TXPRM)) { err("UPDATE NO ARG IN WHERE"); return -1; }//arg1
				cond.put(reinterpret_cast<char*>(token), ret);

				if (chkToken(sql, token, ret, CMDS::TXOP)) { err("UPDATE NO OPE IN WHERE"); return -1; }//op1
				cond.put(reinterpret_cast<char*>(token), ret);

				if (chkToken(sql, token, ret, CMDS::TXARG, CMDS::TXPRM)) { err("UPDATE NO ARG IN WHERE"); return -1; }//arg2
				cond.put(reinterpret_cast<char*>(token), ret);

				if (chkToken(sql, token, ret, CMDS::TXAND, CMDS::TXOR)) break;
				cond.put(reinterpret_cast<char*>(token), ret);
			}
		}
		//末尾の処理
		if (ret != CMDS::TXED && ret != CMDS::TXNONE) {
			err("UPDATE NO ARG IN WHERE");
			return -1;
		}

		//テーブル取得
		tbl = tblList[tables[0]];
		//where実行
		tbl->Update(colnams, values, cond);
		break;
	case  CMDS::TXDELETE://DELETE FROM TABLENAME WHERE cond1,op1,cond2
		//テーブル名取得
		if (chkToken(sql, token, ret, CMDS::TXFROM)) { err("DELETE SYNTAX ERROR");   return -1; }//table name
		if (chkToken(sql, token, ret, CMDS::TXARG)) { err("DELETE NO TABLE ERROR"); return -1; }//table name
		tables.push_back(reinterpret_cast<char*>(token));

		ret = getToken(sql, token);
		//WHERE取得
		if (ret == CMDS::TXWHERE) {
			cond.put(const_cast<char*>("AND"), CMDS::TXAND);                  //初回は1にANDする
			for (;;) {
				if (chkToken(sql, token, ret, CMDS::TXARG, CMDS::TXPRM)) { err("DELETE NO ARG IN WHERE"); return -1; }//arg1
				cond.put(reinterpret_cast<char*>(token), ret);

				if (chkToken(sql, token, ret, CMDS::TXOP)) { err("DELETE NO OPE IN WHERE"); return -1; }//op1
				cond.put(reinterpret_cast<char*>(token), ret);

				if (chkToken(sql, token, ret, CMDS::TXARG, CMDS::TXPRM)) { err("DELETE NO ARG IN WHERE"); return -1; }//arg2
				cond.put(reinterpret_cast<char*>(token), ret);

				if (chkToken(sql, token, ret, CMDS::TXAND, CMDS::TXOR))  break;
				cond.put(reinterpret_cast<char*>(token), ret);
			}
		}
		//末尾の処理
		if (ret != CMDS::TXED && ret != CMDS::TXNONE) {
			err("UPDATE NO ARG IN WHERE");
			return -1;
		}

		//テーブル取得
		tbl = tblList[tables[0]];
		//where実行
		tbl->Delete(cond);
		break;
		//show databases
		//show tables
		//show tables form database
	case CMDS::TXSHOW:
		ret = getToken(sql, token);
		if (ret == CMDS::TXDATABASES) {
			catalog->ShowCatalog(retStr);
			return 1;
		}
		else if (ret == CMDS::TXTABLES) {
			ret = getToken(sql, token);
			if (ret == CMDS::TXFROM) {
				if (chkToken(sql, token, ret, CMDS::TXARG)) {
					err("NO DATABASE SET");
					return -1;
				}
			}
			else if (ret == CMDS::TXED || ret == CMDS::TXNONE) {
				map<wString, Table*>::iterator it;
				retStr.clear();
#ifdef CMDLINE
				for (it = tbllist.begin(); it != tbllist.end(); it++) {
					retStr.cat_sprintf("%s\n", it->first.c_str());
				}
#else
				retStr = "[";
				int first = 1;
				for (it = tblList.begin(); it != tblList.end(); it++) {
					retStr.cat_sprintf("%s\"%s\"", ((first) ? "" : ","), it->first.c_str());
					first = 0;
				}
				retStr += "]";
#endif
			}
			return 1;
		}
		else {
			err("what to do?");
			return 0;
		}
	case CMDS::TXDROP:
		ret = getToken(sql, token);
		if (ret == CMDS::TXTBL) {
			if (chkToken(sql, token, ret, CMDS::TXARG)) { err("DROPTABLE NO TABLE ERROR"); return -1; }//tablename
			//チェック。同名のテーブルがあるか？
			if (!tblList.count(reinterpret_cast<char*>(token))) {
				err("Table not exist");
				return -1;
			}
			Table* ntbl = tblList[reinterpret_cast<char*>(token)];
			//ntbl->changed = true;
			delete ntbl;
			tblList.erase(reinterpret_cast<char*>(token));
			//this->Save();
			//ＤＢに変更がありました。
			this->changed = true;
			break;
		}
		else if (ret == CMDS::TXDATABASE) {
			if (chkToken(sql, token, ret, CMDS::TXARG)) { err("CREATE DATABASE NO TABLE ERROR"); return -1; }//tablename
			//存在チェック
			if (!catalog->dblist.count(reinterpret_cast<char*>(token))) {
				err("Database not exist");
				return -1;
			}
			Database* db = catalog->dblist[reinterpret_cast<char*>(token)];
			//使用してない時だけ消せる
			if (db) {
				if (db->ref > 0) {
					err("Database in use");
					return -1;
				}
				delete db;
			}

			//カタログから消去
			catalog->dblist.erase(reinterpret_cast<char*>(token));
			break;
		}
		else {
			err("What to do?");
			return 0;
		}
		//break;
	//ALTER TABLE TABLENAME ADD (COLUMN1 definition1,COLUMN2 definition2..)
	//ALTER TABLE TABLENAME MODIFY (COLUMN1 definition1,COLUMN2 definition2..)
	//ALTER TABLE TABLENAME RENAME TO NEWTABLENAME
	//ALTER TABLE TABLENAME RENAME COLUMN OLDCOLUMN TO NEWCOLUMN
	//ALTER TABLE TABLENAME DROP (COLUMN1, COLUMN2..);

	case CMDS::TXALTER:
		if (chkToken(sql, token, ret, CMDS::TXTBL)) { err("ALTER TABLE SYNTAX ERROR");   return -1; }//tablename
		if (chkToken(sql, token, ret, CMDS::TXARG)) { err("ALTER TABLE NO TABLE ERROR"); return -1; }//tablename
		tables.push_back(reinterpret_cast<char*>(token));
		ret = getToken(sql, token);
		//追加/変更
		if (ret == CMDS::TXADD || ret == CMDS::TXMODIFY || ret == CMDS::TXDROP) {
			CMDS nctyp = ret;
			if (chkToken(sql, token, ret, CMDS::TXPS)) { err("ALTER TABLE SYNTAX ERROR");   return -1; }//field name
			for (;;) {
				if (chkToken(sql, token, ret, CMDS::TXARG)) { err("ALTER TABLE INVALID ARGS"); return -1; }//field name
				if (chkToken(sql, token2, ret, CMDS::TXARG) || (stricmp(reinterpret_cast<char*>(token2), "number") != 0 && stricmp(reinterpret_cast<char*>(token2), "string") != 0))
				{
					err("CREATE TABLE INVALID ARGS");   return -1;
				}//number/string
				dataType typ = (stricmp(reinterpret_cast<char*>(token2), "number") == 0) ? dataType::NUMBER : dataType::STRING;
				Column* clm = new Column(tables[0], reinterpret_cast<char*>(token), typ);
				columns.push_back(clm);
				//delete clm;
				ret = getToken(sql, token);
				if (ret == CMDS::TXPE)          break;
				else if (ret != CMDS::TXCM) { err("ALTER TABLE SYNTAX ERROR");   return -1; }
			}
			//ADD,MODIFY,DROPチェック処理
			//テーブル取得
			tbl = tblList[tables[0]];
			vector<int> colpos;
			for (unsigned int i = 0; i < columns.size(); i++) {
				//ADD 同じカラムない
				//MODIFY　違うカラムない
				int flag = 0;
				for (unsigned int j = 0; j < tbl->column.size(); j++) {
					if (tbl->column[j]->Compare(columns[i]->name, cond.clmalias, cond.tblalias)) {
						if (nctyp == CMDS::TXADD) { err("ALTER TABLE DUPLEX COLUMN NAME");   return -1; }//field name
						flag = 1;
						colpos.push_back(j);
					}
				}
				if (flag == 0) {
					if (nctyp == CMDS::TXMODIFY) { err("ALTER TABLE COLUMN NOT FOUND");   return -1; }//field name
				}
			}
			//ADD,MODIFY実行処理
			if (nctyp == CMDS::TXADD) {
				for (unsigned int i = 0; i < columns.size(); i++) {
					tbl->column.push_back(columns[i]);
					Node* nd = new Node(columns[i]->type);
					//個数を増やす
					for (unsigned int j = 0; j < tbl->node[0]->size(); j++) {
						nd->put("");
					}
					tbl->node.push_back(nd);
				}
			}
			else if (nctyp == CMDS::TXMODIFY) {
				for (unsigned int i = 0; i < columns.size(); i++) {
					tbl->column[colpos[i]] = columns[i];
					//ノードの変更
					Node* nd = new Node(columns[i]->type);
					//nd->resize(tbl->node[colpos[i]].size());
					for (unsigned int j = 0; j < tbl->node[colpos[i]]->size(); j++) {
						nd->put(tbl->node[colpos[i]]->getNode(j, 1));
					}
					//tbl->node[colpos[i]].changetype(columns[i]->type);
					//消す
					tbl->node.erase(tbl->node.begin() + colpos[i]);
					//挿入
					tbl->node.insert(tbl->node.begin() + colpos[i], nd);
				}
			}
			//変更したことにする
			tbl->node[0]->changed = true;
		}
		else if (ret == CMDS::TXDROP) {
			CMDS nctyp = ret;
			if (chkToken(sql, token, ret, CMDS::TXPS)) { err("ALTER TABLE SYNTAX ERROR");   return -1; }//field name
			for (;;) {
				if (chkToken(sql, token, ret, CMDS::TXARG)) { err("ALTER TABLE INVALID ARGS"); return -1; }//field name
				colnams.push_back(reinterpret_cast<char*>(token));
				ret = getToken(sql, token);
				if (ret == CMDS::TXPE)          break;
				else if (ret != CMDS::TXCM) { err("ALTER TABLE SYNTAX ERROR");   return -1; }
			}
			//ADD,MODIFY,DROPチェック処理
			//テーブル取得
			tbl = tblList[tables[0]];
			vector<int> colpos;
			for (unsigned int i = 0; i < colnams.size(); i++) {
				//DROP 違うカラムない
				for (unsigned int j = 0; j < tbl->column.size(); j++) {
					if (tbl->column[j]->Compare(colnams[i], cond.clmalias, cond.tblalias)) {
						colpos.push_back(j);
					}
					else {
						err("ALTER TABLE COLUMN NOT FOUND");
						return -1;
					}
				}
			}
			//DROP実行処理
			if (nctyp == CMDS::TXDROP) {   //TXDROP
				for (int i = static_cast<int>(columns.size()) - 1; i >= 0; i--) {
					tbl->column.erase(tbl->column.begin() + colpos[i]);
					tbl->node.erase(tbl->node.begin() + colpos[i]);
				}
			}
			//カラム一時データ削除
			for (unsigned int i = 0; i < columns.size(); i++) {
				delete columns[i];
			}
			//変更したことにする
			tbl->node[0]->changed = true;
			//変更処理
		}
		else if (ret == CMDS::TXRENAME) {
			ret = getToken(sql, token);
			//テーブル名変更
			if (ret == CMDS::TXTO) {
				if (chkToken(sql, token, ret, CMDS::TXARG)) { err("ALTER TABLE NO TABLE ERROR"); return -1; }//tablename
				tables.push_back(reinterpret_cast<char*>(token));
				//テーブル名変更処理
				tbl = tblList[tables[0]];
				tbl->name = tables[1];
				//変更したことにする
				tbl->node[0]->changed = true;
			}
			else if (ret == CMDS::TXCOLUMN) {
				if (chkToken(sql, token, ret, CMDS::TXARG)) { err("ALTER TABLE NO TABLE ERROR"); return -1; }//tablename
				colnams.push_back(reinterpret_cast<char*>(token));
				if (chkToken(sql, token, ret, CMDS::TXTO)) { err("ALTER TABLE NO TABLE ERROR"); return -1; }//tablename
				if (chkToken(sql, token, ret, CMDS::TXARG)) { err("ALTER TABLE NO TABLE ERROR"); return -1; }//tablename
				colnams.push_back(reinterpret_cast<char*>(token));
				//カラム名変更処理
				tbl = tblList[tables[0]];
				//カラム番号求める
				for (unsigned int i = 0; i < columns.size(); i++) {
					if (tbl->column[i]->Compare(colnams[0], cond.clmalias, cond.tblalias)) {
						tbl->column[i]->name = colnams[1];
						return 0;
					}
				}
				err("ALTER TABLE NO SUCH COLUMN");
				return -1;
			}
			else {
				err("ALTER TABLE SYNTAX ERRRO");
				return -1;
			}
		}
		break;
		//DESC table_name;
	case CMDS::TXDESC:
		if (chkToken(sql, token, ret, CMDS::TXARG)) { err("DESC NO TABLE ERROR"); return -1; }//tablename
		//テーブル取得
		if (tblList.count(reinterpret_cast<char*>(token)) == 0) {
			err("DESC TABLE NAME NOT EXISTS ERROR");
			return -1;
		}
		tbl = tblList[reinterpret_cast<char*>(token)];
		retStr.clear();
#ifdef CMDLINE
		retStr = "name\ttype\n";
		for (unsigned int i = 0; i < tbl->column.size(); i++) {
			retStr.cat_sprintf("%s\t%s\n", tbl->column[i]->name.c_str(), (tbl->column[i]->type == STRING) ? "STRING" : "NUMBER");
		}
#else
		retStr = "[{";
		for (unsigned int i = 0; i < tbl->column.size(); i++) {
			retStr.cat_sprintf("%s\"%s\":\"%s\"", ((i) ? "," : ""), tbl->column[i]->name.c_str(), (tbl->column[i]->type == dataType::STRING) ? "STRING" : "NUMBER");
		}
		retStr += "}]";
#endif
		return 1;
	case CMDS::TXHELP:
#ifdef CMDLINE
		printf("SHOW DATABASES;\n"
			"SHOW TABLES;\n"
			"DESC table_name;\n"
			"CREATE DATABASE database_name;\n"
			"CREATE TABLE table_name(column_name1 number/string,...);\n"
			"USE database_nameE;\n"
			"DROP database_name;\n"
			"DROP table_name;\n"
			"ALTER TABLE TABLENAME ADD (COLUMN1 definition1,COLUMN2 definition2..);\n"
			"ALTER TABLE TABLENAME MODIFY (COLUMN1 definition1,COLUMN2 definition2..);\n"
			"ALTER TABLE TABLENAME DROP (COLUMN1, COLUMN2..);;\n"
			"ALTER TABLE TABLENAME RENAME TO NEWTABLENAME;\n"
			"ALTER TABLE TABLENAME RENAME COLUMN OLDCOLUMN TO NEWCOLUMN;\n"
			"select/insert/update/delete..\n");
#else
		retStr =
			"["
			"[\"name\"]"
			"[\"SHOW DATABASES;\"]"
			"[\"SHOW TABLES;\"]"
			"[\"DESC table_name;\"]"
			"[\"CREATE DATABASE database_name;\"]"
			"[\"CREATE TABLE table_name(column_name1 number/string,...);\"]"
			"[\"USE database_nameE;\"]"
			"[\"DROP database_name;\"]"
			"[\"DROP table_name;\"]"
			"[\"ALTER TABLE TABLENAME ADD (COLUMN1 definition1,COLUMN2 definition2..);\"]"
			"[\"ALTER TABLE TABLENAME MODIFY (COLUMN1 definition1,COLUMN2 definition2..);\"]"
			"[\"ALTER TABLE TABLENAME DROP (COLUMN1, COLUMN2..);\"]"
			"[\"ALTER TABLE TABLENAME RENAME TO NEWTABLENAME;\"]"
			"[\"ALTER TABLE TABLENAME RENAME COLUMN OLDCOLUMN TO NEWCOLUMN;\"]"
			"[\"select/insert/update/delete..\"]"
			"]";
#endif
		break;
	default:
		err("What to do?");
		break;
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////
int Database::LoadFromFile(wString file) {
	//tableを全部読み込む
	unsigned int max;
	bufrd* br = new bufrd();
	tblList.clear();
	if (wString::FileExists(file)) {
		if (br->ropen(file)) {
			return -1;
		}
		//ファイルがない場合終了
	}
	else {
		return -1;
	}
	//テーブル個数取得
	if (br->Read(&max, sizeof(unsigned int))) return -1;
	//テーブル取得
	for (unsigned int i = 0; i < max; i++) {
		Table* tbl = new Table();
		if (tbl->LoadFromFile(br) < 0) {
			err("table load error\n");
			return -1;
		}
		tblList[tbl->name] = tbl;
	}
	br->rclose();
	delete br;
	return 0;
}
////////////////////////////////////////////////////////////////////////////
int Database::SaveToFile(wString file) {
	//テーブル保存
	//int flag=0;
	//map<wString,Table*>::iterator it;
	//for( it = tbllist.begin() ; it != tbllist.end() ; it++ ){
	//    flag += it->second->changed();
	//}
	//変更ありなら保存

	//if( flag ){
	auto br = new bufrd();
	if (br->wopen(file)) {
		return -1;
	}
	//名前はcatalogクラスで保存
	//テーブル個数保存
	auto max = (unsigned int)tblList.size();
	br->Write(&max, sizeof(unsigned int));
	//テーブル保存
	map<wString, Table*>::iterator nit;
	for (nit = tblList.begin(); nit != tblList.end(); nit++) {
		nit->second->SaveToFile(br);
	}
	br->wclose();
	delete br;
	return 0;
	//}
	//保存しなかった
	//return -1;
}

/////////////////////////////////////////////////////////////////////////////
//カタログクラス
////////////////////////////////////////////////////////////////////////////
//シングルトン取得
//DBCatalog& DBCatalog::getInstance(void){
//    static DBCatalog instance;
//    return instance;
//}
////////////////////////////////////////////////////////////////////////////
//データベースに接続する
Database* DBCatalog::DBConnect(const wString& DBName) {
	Database* db = NULL;
	//ここにクリティカルセクションを設定
#ifdef linux
	pthread_mutex_lock(&mutex);
#else
	EnterCriticalSection(&cs);
#endif
	if (dblist.count(DBName)) {
		//未登録
		if (dblist[DBName] == NULL) {
			dblist[DBName] = new Database(DBName);
		}
		dblist[DBName]->ref++;

		db = dblist[DBName];
	}
#ifdef linux
	pthread_mutex_unlock(&mutex);
#else
	LeaveCriticalSection(&cs);
#endif
	return db;
}
////////////////////////////////////////////////////////////////////////////
//データベースを閉じる
int DBCatalog::DBClose(Database* db) {
	if (!db) {
		return -1;
	}
	////強制保存する
	//if (forceSave)
	//{
	//	forceSave = false;
 //       db->Save();
	//}
	map<wString, Database*>::iterator it;
	for (it = dblist.begin(); it != dblist.end(); it++) {
		if ((*it).second == db) {
			//参照カウンタを減らす
			if (db->ref > 0) {
				db->ref--;
				if (db->ref <= 0) {
					delete db;
					dblist[(*it).first] = NULL;
				}
			}
			return 0;
		}
	}
	return -1;
}
////////////////////////////////////////////////////////////////////////////
//新規にDB作成
Database* DBCatalog::DBCreate(wString DBName) {
	if (dblist.count(DBName)) {
		err("database already exists");
		return NULL;
	}
	dblist[DBName] = new Database(DBName);
	return dblist[DBName];
}
////////////////////////////////////////////////////////////////////////////
unsigned int DBCatalog::ShowCatalog(wString& retStr)
{
	map<wString, Database*>::iterator it;
	retStr.clear();
#ifdef CMDLINE
	for (it = dblist.begin(); it != dblist.end(); it++) {
		retStr.cat_sprintf("%s\n", (*it).first.c_str());
	}
#else
	retStr = "[";
	int lines = 0;
	for (it = dblist.begin(); it != dblist.end(); it++) {
		retStr.cat_sprintf("%s\"%s\"", ((lines++) ? "," : ""), (*it).first.c_str());
	}
	retStr += "]";
#endif
	return (unsigned int)dblist.size();
}
////////////////////////////////////////////////////////////////////////////
int DBCatalog::LoadFromFile(void)
{
	unsigned int len;
	unsigned int max;
	int    fd;
	char   name[1024] = { 0 };
	dblist.clear();
	//オープン
	if (!wString::FileExists(CATFILE)) return 0;
	fd = open(CATFILE, O_RDONLY | O_BINARY);
	//dbname read
	if (read(fd, &max, sizeof(unsigned int)) != sizeof(unsigned int)) { err("can't read"); close(fd); return -1; }
	//read all database
	for (unsigned int i = 0; i < max; i++) {
		if (read(fd, &len, sizeof(unsigned int)) != sizeof(unsigned int)) { err("can't read len"); close(fd); return -1; }
		if (read(fd, name, len) != (int)len) { err("can't read dbname"); close(fd); return -1; }
		name[len] = 0;
		//ロード時DBはNULL。Createで作成する
		dblist[name] = NULL;
	}
	close(fd);
	return 0;
}
//create database
//use databaseを受け付ける
////////////////////////////////////////////////////////////////////////////
int DBCatalog::SaveToFile(void)
{
	//DBCatalog保存
	wString path = current_dir + DELIMITER + CATFILE;
	int fd = myopen(path.c_str(), O_CREAT | O_TRUNC | O_WRONLY | O_BINARY, S_IREAD | S_IWRITE);
	if (fd < 0) {
		return -1;
	}
	//dbname保存
	map<wString, Database*>::iterator it;
	auto max = (unsigned int)dblist.size();
	if (write(fd, &max, sizeof(unsigned int)) != sizeof(unsigned int)) { close(fd); return -1; }
	for (it = dblist.begin(); it != dblist.end(); it++) {
		auto len = it->first.length();
		if (write(fd, &len, sizeof(unsigned int)) != sizeof(unsigned int)) { close(fd); return -1; }
		if (write(fd, it->first.c_str(), len) != (int)len) { close(fd); return -1; }
	}
	// ファイルクローズ
	close(fd);
	for (it = dblist.begin(); it != dblist.end(); it++) {
		if (it->second != NULL) {
			delete it->second;
		}
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////
DBCatalog::DBCatalog(void)
{
	//MUTEX初期化
	refs = 0;
#ifdef linux
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init(&mutex, &attr);
#else
	InitializeCriticalSection(&cs);
#endif
	//動的に作る
	connects = new map<wString, Database*>;
	dblist.clear();
	//強制保存しない
	//forceSave = false;
	if (LoadFromFile() < 0) {
		err("DataBase Read Error");
	}
}

wString _DBConnect(wString& database)
{
	const static char material[] = "abcdefghijklmnopqrstuvwxyz0123456789";
	Database* db;
	db = catalog->DBConnect(database);
	if (db == NULL) {
		return "";
	}
	//乱数生成
	srand((unsigned)time(NULL));
	unsigned char work[27] = { 0 };
	wString temp;
	while (1) {
		for (int i = 0; i < 26; i++) {
			work[i] = material[rand() % (sizeof(material) - 1)];
		}
		temp = reinterpret_cast<char*>(work);
		//同じモノはダメ
		if (connects->count(reinterpret_cast<char*>(work)) == 0) break;
	}
	(*connects)[reinterpret_cast<char*>(work)] = db;
	return temp;
}
int _DBDisConnect(wString& key)
{
	if (connects->count(key) > 0) {
		Database* db = (*connects)[key];
		catalog->DBClose(db);
		connects->erase(key);
		return 0;
	}
	return -1;
}
wString _DBSQL(const wString& key, wString& sql)
{
	if (connects->count(key) > 0) {
		wString retStr;
		Database* db = (*connects)[key];
		int ret = db->SQL(sql, retStr);
		switch (ret) {
		case -1:
			//return "ERROR:" + sql;
			return "ERROR";
		case 0:
			return "OK";
		default:
			return retStr;
		}
	}
	else
	{
		return "ERROR KEY NOT FOUND";
	}
}
////////////////////////////////////////////////////////////////////////////
DBCatalog::~DBCatalog(void)
{
	refs--;
	//connectはまあいいかな。
	map<wString, Database*>::iterator it;
	for (it = connects->begin(); it != connects->end(); it++) {
		wString nam = it->first;
		_DBDisConnect(nam);
	}
	delete connects;
	SaveToFile();
}
#if 0
////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	IGNORE_PARAMETER(argc);
	IGNORE_PARAMETER(argv);
#if 1
	unsigned char cmd[1024];
	unsigned char cmd2[1024];
	unsigned char token[1024];
	//wString::wStringInit();
	CTYP ret;
	//必要ならDBCreateでDB作成
	//通常はDBConnectでDBに接続
	//終了時または適宜textに書き込んで終了
	debug_log_initialize("debug.log");
	printf("Welcome to cybele msql. type help to show help. quit to terminate system.\n");
	//catalog = new DBCatalog();
	Database* db;
	if (catalog->dblist.size() == 0) {
		db = catalog->DBCreate((char*)"_SYSTEM");
	}
	else {
		db = catalog->DBConnect((char*)"_SYSTEM");
	}
	//FILE* rs = fopen( "sample_sql.txt","rt");


	//HANDLE handle1;
	//HANDLE handle2;
	//DWORD  id1;
	//DWORD  id2;

	//handle1 = CreateThread(0, 0, (LPTHREAD_START_ROUTINE) loop , (void*)NULL, 0, &id1);
	//handle2 = CreateThread(0, 0, (LPTHREAD_START_ROUTINE) loop , (void*)NULL, 0, &id2);


	for (;;) {
		gets((char*)cmd);
		//fgets(cmd,1024,rs);
		//while( cmd[strlen(cmd)-1]< ' ' ){
		//    cmd[strlen(cmd)-1] = 0;
		//}
		//puts( cmd );
		strcpy((char*)cmd2, (char*)cmd);
		ret = getToken(cmd, token);
		if (ret == TXQUIT) break;
		else if (ret == TXUSE) {
			if (chkToken(cmd, token, ret, TXARG)) {
				err("SYNTAX ERROR");
				continue;
			}
			Database* db1 = catalog->DBConnect((char*)token);
			if (db1) {
				catalog->DBClose(db);
				db = db1;
				printf("OK\n");
			}
			else {
				printf("ERROR\n");
			}
		}
		else {
			wString retStr;
			int ret = db->SQL((char*)cmd2, retStr);
			switch (ret) {
			case -1:
				printf("ERROR\n");
				break;
			case 0:
				printf("OK\n");
				break;
			case 1:
				printf("%s", retStr.c_str());
				printf("OK\n");
				break;
			}
			continue;
		}

	}
	//delete catalog;
	//loop_flag = 0;
	//fclose( rs );
	//WaitForMultipleObjects(1, &handle1, TRUE, INFINITE);
	//CloseHandle(handle1);
	//WaitForMultipleObjects(1, &handle2, TRUE, INFINITE);
	//CloseHandle(handle2);
#else
	//    wString::wStringInit();
	debug_log_initialize("debug.log");
	char* ret = Connect((unsigned char*)"test");
	if (ret) {
		wString Database = ret;
		wString res = (char*)SQL(Database, (unsigned char*)"select * from address limit 0,10;");
		printf("%s\n", res.c_str());
		DisConnect(Database);
	}
	int ch = getchar();
#endif
	return 0;
}
#endif
//---------------------------------------------------------------------------





