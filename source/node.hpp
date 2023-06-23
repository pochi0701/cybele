#ifndef nodehpp
#define nodehpp
#include <string>
#include <vector>
#include <iostream>
#ifdef linux
#include <unistd.h>
#endif
#include "cbl_String.h"
#include "define.h"
//#define O_BINARY 0
/////////////////////////////////////////////////////////////////////////////
enum class dataType { STRING = 0, NUMBER };
enum class orderType { ASC = 0, DESC };
/////////////////////////////////////////////////////////////////////////////
using namespace std;
#define HSIZ (1024*8)
#define SIZ HSIZ*2
typedef unsigned int UINT;
/////////////////////////////////////////////////////////////////////////////

/// <summary>
/// バッファリングreadクラス
/// </summary>
class bufrd
{
private:
	unsigned char buf[SIZ] = { 0 };
	int           bufptr;
	int           fd;
	unsigned int  rest;
public:
	bufrd(void) {}

	/// <summary>
	/// 読み込みオープン
	/// </summary>
	/// <param name="file">ファイル名</param>
	/// <returns>成功:0 失敗:-1</returns>
	int ropen(const wString& file) {
		bufptr = 0;
		fd = open(file.c_str(), O_RDONLY | O_BINARY);
		if (fd < 0) return -1;
		rest = read(fd, static_cast<void*>(buf), sizeof(buf));
		return 0;
	}

	/// <summary>
	/// 書き込みオープン
	/// </summary>
	/// <param name="file">ファイル名</param>
	/// <returns>成功:0 失敗:-1</returns>
	int wopen(const wString& file) {
		bufptr = 0;
		fd = open(file.c_str(), O_CREAT | O_TRUNC | O_WRONLY | O_BINARY, S_IREAD | S_IWRITE);
		if (fd < 0) return -1;
		return 0;
	}

	/// <summary>
	/// 読み込みクローズ
	/// </summary>
	/// <param name=""></param>
	void rclose(void) {
		close(fd);
		fd = -1;
	}
	
	/// <summary>
	/// 書き込みクローズ
	/// </summary>
	/// <param name=""></param>
	void wclose(void) {
		write(fd, static_cast<void*>(buf), bufptr);
		close(fd);
		fd = -1;
	}
	//読み込み：戻り値 成功:0 失敗:-1
	//失敗時closeしなくて良い
	int Read(void* data, unsigned int siz)
	{
		if (rest >= siz) {
			memcpy(data, buf + bufptr, siz);
			bufptr += siz;
			rest -= siz;
			if (bufptr >= HSIZ) {
				memcpy(buf, buf + HSIZ, HSIZ);
				rest += read(fd, static_cast<void*>(buf + HSIZ), HSIZ);
				bufptr -= HSIZ;
			}
			return 0;
		}
		else {
			close(fd);
			return -1;
		}
	}
	//読み込み：戻り値 成功:0 失敗:-1
	//失敗時closeしなくて良い
	int Write(const void* data, unsigned int siz)
	{
		memcpy(buf + bufptr, data, siz);
		bufptr += siz;
		if (bufptr >= HSIZ) {
			int num = write(fd, static_cast<void*>(buf), HSIZ);
			if (num != HSIZ) {
				return -1;
			}
			memcpy(buf, buf + HSIZ, HSIZ);
			bufptr -= HSIZ;
		}
		return 0;
	}
};
/////////////////////////////////////////////////////////////////////////////

/// <summary>
/// ノードクラス
/// </summary>
class Node {
private:
	//各文字列への先頭ポインタ
	vector<wString>  data;
	//データ・タイプ
	dataType         type;
public:
	bool             changed;

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="mytype"></param>
	Node(dataType mytype)
	{
		type = mytype;
		changed = false;
		data.clear();
	}

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="nd"></param>
	Node(const Node* nd) {
		data.clear();
		type = nd->type;
		data.resize(nd->data.size());
		//コピーする
		copy(nd->data.begin(), nd->data.end(), data.begin());
		changed = false;
	}

	/// <summary>
	/// デストラクタ
	/// </summary>
	/// <param name=""></param>
	~Node(void) {
		data.clear();
	}
	//ノードの文字列取得
	int Delete(unsigned int ptr)
	{
		if (ptr > data.size()) {
			return -1;
		}
		else {
			data.erase(data.begin() + ptr);//1行削除
			return 0;
		}
	}
	//指定ノードの文字列取得
	wString getNode(const unsigned int ptr, int native = 0)
	{
		wString temp;
		if (type == dataType::STRING) {
			if (native) {
				temp = data[ptr];
			}
			else {
				temp.sprintf("\"%s\"", data[ptr].c_str());
			}
		}
		else {
			temp.sprintf("%d", atoi(static_cast<char*>(data[ptr].c_str())));
		}
		return temp;
	}
	//指定ノードの文字列取得
	const wString& getNodeNative(const  unsigned int ptr)
	{
		return data[ptr];
	}
	//

	/// <summary>
	/// 設定した個数
	/// </summary>
	/// <returns>個数</returns>
	unsigned int size()
	{
		return (unsigned int)data.size();
	}

	//ソート結果をvector<int>に保存
	//indexにはsortすべき項目が入っている
	void sort(vector<int>& indexdt, vector<int>& indexno, orderType mytype) {
		auto siz = (unsigned int)indexdt.size();
		//数値
		if (type == dataType::NUMBER) {
			if (mytype == orderType::ASC) {
				//昇順
				for (auto i = 0U; i < siz - 1; i++) {
					for (auto j = i + 1U; j < siz; j++) {
						int ii = indexdt[i];
						int jj = indexdt[j];
						if (data[ii][0] == '-' || data[jj][0] == '-') {
							if (data[ii] < data[jj]) {
								swap(indexdt[i], indexdt[j]);
								swap(indexno[i], indexno[j]);
							}
						}
						else {
							if (data[ii] > data[jj]) {
								swap(indexdt[i], indexdt[j]);
								swap(indexno[i], indexno[j]);
							}
						}
					}
				}
			}
			else {
				//降順
				for (unsigned int i = 0; i < siz - 1; i++) {
					for (unsigned int j = i + 1; j < siz; j++) {
						int ii = indexdt[i];
						int jj = indexdt[j];
						if (data[ii][0] == '-' || data[jj][0] == '-') {
							if (data[ii] > data[jj]) {
								swap(indexdt[i], indexdt[j]);
								swap(indexno[i], indexno[j]);
							}
						}
						else {
							if (data[ii] < data[jj]) {
								swap(indexdt[i], indexdt[j]);
								swap(indexno[i], indexno[j]);
							}
						}
					}
				}
			}
		}
		else {
			if (mytype == orderType::ASC) {
				//昇順
				for (unsigned int i = 0; i < siz - 1; i++) {
					for (unsigned int j = i + 1; j < siz; j++) {
						if (data[indexdt[i]] > data[indexdt[j]]) {
							swap(indexdt[i], indexdt[j]);
							swap(indexno[i], indexno[j]);
						}
					}
				}
			}
			else {
				//降順
				for (unsigned int i = 0; i < siz - 1; i++) {
					for (unsigned int j = i + 1; j < siz; j++) {
						if (data[indexdt[i]] < data[indexdt[j]]) {
							swap(indexdt[i], indexdt[j]);
							swap(indexno[i], indexno[j]);
						}
					}
				}
			}
		}
	}
	//文字が格納されているか？
	int contains(char* letters)
	{
		for (unsigned int i = 0; i < data.size(); i++) {
			if (data[i] == letters) {
				return i;
			}
		}
		return -1;
	}
	//修飾なしで文字を登録
	void putNative(const wString& letters)
	{
		data.push_back(letters);
		return;
	}
	//文字を登録
	void put(const wString& letters, int ptr = -1)
	{
		//string tmp = letters;
		if (type == dataType::NUMBER) {
			char work[128];
			sprintf(work, "%+010d", atoi(letters.c_str()));
			if (ptr >= 0) {
				data[ptr] = work;
			}
			else {
				data.push_back(work);
			}
		}
		else {
			if (ptr >= 0) {
				data[ptr] = letters;
			}
			else {
				data.push_back(letters);
			}
		}
		changed = true;
		return;
	}

	/// <summary>
	/// ファイルからDBへ復帰
	/// </summary>
	/// <param name="br">読み込むバッファ</param>
	/// <returns>0</returns>
	int LoadFromFile(bufrd* br)
	{
		unsigned short len;
		unsigned int   max;
		unsigned char  uctp;
		data.clear();//必要？
		char work[1024];
		//データ・タイプ取得
		if (br->Read(&uctp, sizeof(uctp)))				return -1;
		type = (dataType)uctp;
		//総数取得
		if (br->Read(&max, sizeof(unsigned int)))		return -1;
		//データ取得
		data.resize(max);
		for (unsigned int i = 0; i < max; i++) {
			if (br->Read(&len, sizeof(unsigned short)))	return -1;
			if (br->Read(work, len))  return -1;
			work[len] = 0;
			data[i] = work;
		}
		changed = false;
		return 0;
	}

	/// <summary>
	/// データベース情報をファイルに保存
	/// </summary>
	/// <param name="br">書き込みバッファ</param>
	void SaveToFile(bufrd* br)
	{
		auto max = data.size();
		auto uctp = (unsigned char)type;
		//データ・タイプ保存
		br->Write(&uctp, sizeof(uctp));
		//総数を保存
		br->Write(&max, sizeof(unsigned int));
		//データを保存
		for (auto i = 0U; i < max; i++) {
			auto len = data[i].size();
			br->Write(&len, sizeof(unsigned short));
			br->Write(data[i].c_str(), len);
		}
		changed = false;
	}
};
#endif
