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
typedef enum {STRING,NUMBER} dataType;
typedef enum {ASC,DESC} orderType;
/////////////////////////////////////////////////////////////////////////////
using namespace std;
#define SIZ 1024*2
#define HSIZ 1024
/////////////////////////////////////////////////////////////////////////////
//バッファリングreadクラス
class bufrd
{
private:
    unsigned char buf[SIZ];
    int           bufptr;
    int           fd;
    size_t        rest;
public:
    bufrd(void){}
    //読み込みオープン：戻り値 成功:0 失敗:-1
    int ropen( const wString& file){
       bufptr = 0;
       fd = open(file.c_str(), O_RDONLY | O_BINARY );
       if( fd < 0 ) return -1;
       rest = read(fd,(void*)buf,sizeof( buf ));
       return 0;
    }
    //書き込みオープン：戻り値 成功:0 失敗:-1
    int wopen( const wString& file){
       bufptr = 0;
       fd = open(file.c_str(), O_CREAT | O_TRUNC | O_WRONLY | O_BINARY , S_IREAD | S_IWRITE );
       if( fd < 0 ) return -1;
       return 0;
    }
    //クローズ
    void rclose(void){
       close(fd);
       fd = -1;
    }
    //クローズ
    void wclose(void){
       write(fd,(void*)buf,bufptr);
       close(fd);
       fd = -1;
    }
    //読み込み：戻り値 成功:0 失敗:-1
    //失敗時closeしなくて良い
    int Read(void* data,size_t siz)
    {
        if( rest >= siz){
            memcpy( data,buf+bufptr,siz);
            bufptr += siz;
            rest   -= siz;
            if( bufptr >= HSIZ ){
                memcpy(buf,buf+HSIZ,HSIZ);
                rest += read(fd,(void*)(buf+HSIZ),HSIZ);
                bufptr -= HSIZ;
            }
            return 0;
        }else{
            close(fd);
            return -1;
        }
    }
    //読み込み：戻り値 成功:0 失敗:-1
    //失敗時closeしなくて良い
    int Write(const void* data,size_t siz)
    {
        memcpy( buf+bufptr,data,siz);
        bufptr += siz;
        if( bufptr >= HSIZ ){
            int num = write(fd,(void*)buf,HSIZ);
            if( num != HSIZ ){
                return -1;
            }
            memcpy(buf,buf+HSIZ,HSIZ);
            bufptr -= HSIZ;
        }
        return 0;
    }
};
/////////////////////////////////////////////////////////////////////////////
//ノードクラス
class Node{
private:
    //各文字列への先頭ポインタ
    vector<wString>  data;
    //データ・タイプ
    dataType         type;
    //コンストラクタ
public:
    int              changed;
    Node(dataType mytype)
    {
        type = mytype;
        changed = 0;
        data.clear();
    }
    Node(const Node* nd){
        data.clear();
        type = nd->type;
        data.resize(nd->data.size());
        //コピーする
        copy(nd->data.begin(), nd->data.end(), data.begin());
        changed = 0;
    }
    ~Node(void){
        data.clear();
    }
    //ノードの文字列取得
    int Delete(size_t ptr)
    {
        if( ptr > data.size() ){
            return -1;
        }else{
            data.erase(data.begin()+ptr);//1行削除
            return 0;
        }
    }
    //指定ノードの文字列取得
    wString getNode(const size_t ptr,int native=0)
    {
        wString temp;
        if( type == STRING ){
            if( native){
                temp = data[ptr];
            }else{
                temp.sprintf( "\"%s\"",data[ptr].c_str());
            }
        }else{
            temp.sprintf( "%d", atoi((char*)data[ptr].c_str()));
        }
        return temp;
    }
    //指定ノードの文字列取得
    const wString& getNodeNative(const size_t ptr)
    {
        return data[ptr];
    }
    //設定した個数
    size_t size()
    {
        return data.size();
    }
    //ソート結果をvector<int>に保存
    //indexにはsortすべき項目が入っている
    void sort(vector<int> &indexdt, vector<int> &indexno, orderType mytype){
        size_t siz = indexdt.size();
        //数値
        if( type == NUMBER){
            if( mytype == ASC ){
                //昇順
                for( size_t i = 0 ; i < siz-1 ; i++ ){
                    for( size_t j = i+1 ; j < siz ; j++ ){
                        int ii = indexdt[i];
                        int jj = indexdt[j];
                        if( data[ii][0] == '-' || data[jj][0] == '-' ){
                            if( data[ii] < data[jj] ){
                                swap(indexdt[i],indexdt[j]);
                                swap(indexno[i],indexno[j]);
                            }
                        }else{
                            if( data[ii] > data[jj] ){
                                swap(indexdt[i],indexdt[j]);
                                swap(indexno[i],indexno[j]);
                            }
                        }
                    }
                }
            }else{
                //降順
                for( size_t i = 0 ; i < siz-1 ; i++ ){
                    for( size_t j = i+1 ; j < siz ; j++ ){
                        int ii = indexdt[i];
                        int jj = indexdt[j];
                        if( data[ii][0] == '-' || data[jj][0] == '-' ){
                            if( data[ii] > data[jj] ){
                                swap(indexdt[i],indexdt[j]);
                                swap(indexno[i],indexno[j]);
                            }
                        }else{
                            if( data[ii] < data[jj] ){
                                swap(indexdt[i],indexdt[j]);
                                swap(indexno[i],indexno[j]);
                            }
                        }
                    }
                }
            }
        }else{
            if( mytype == ASC ){
                //昇順
                for( size_t i = 0 ; i < siz-1 ; i++ ){
                    for( size_t j = i+1 ; j < siz ; j++ ){
                        if( data[indexdt[i]] > data[indexdt[j]] ){
                            swap(indexdt[i],indexdt[j]);
                            swap(indexno[i],indexno[j]);
                        }
                    }
                }
            }else{
                //降順
                for( size_t i = 0 ; i < siz-1 ; i++ ){
                    for( size_t j = i+1 ; j < siz ; j++ ){
                        if( data[indexdt[i]] < data[indexdt[j]] ){
                            swap(indexdt[i],indexdt[j]);
                            swap(indexno[i],indexno[j]);
                        }
                    }
                }
            }
        }
    }
    //文字が格納されているか？
    int contains(char* letters)
    {
        for( size_t i = 0 ; i < data.size();i++){
            if( data[i] == letters ){
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
    void put(const wString& letters,int ptr=-1)
    {
        //string tmp = letters;
        if( type == NUMBER ){
            char work[128];
            sprintf( work, "%+010d",atoi(letters.c_str()));
            if( ptr >= 0 ){
                data[ptr] = work;
            }else{
                data.push_back(work);
            }
        }else{
            if( ptr >= 0 ){
                data[ptr]  = letters;
            }else{
                data.push_back(letters);
            }
        }
        changed = 1;
        return;
    }
    //復帰
    int LoadFromFile(bufrd& br)
    {
        unsigned short len;
        size_t         max;
        unsigned char  uctp;
        data.clear();//必要？
        char work[1024];
        //データ・タイプ取得
        if( br.Read(&uctp,sizeof(uctp)))              return -1;
        type = (dataType)uctp;
        //総数取得
        if( br.Read(&max,sizeof(size_t))   )           return -1;
        //データ取得
        data.resize(max);
        for( size_t i = 0 ; i < max ; i++ ){
            if( br.Read(&len,sizeof(unsigned short)))  return -1;
            if( br.Read(work,len)                   )  return -1;
            work[len] = 0;
            data[i] = work;
        }
        changed = 0;
        return 0;
    }
    //保存
    void SaveToFile(bufrd& br)
    {
        unsigned short len;
        size_t         max;
        unsigned char  uctp;
        max = data.size();
        //データ・タイプ保存
        uctp = (unsigned char)type;
        br.Write(&uctp,sizeof(uctp));
        //総数を保存
        br.Write(&max,sizeof(size_t)   );
        //データを保存
        for( size_t i = 0 ; i < max ; i++ ){
            len = (unsigned short)data[i].size();
            br.Write(&len,sizeof(unsigned short));
            br.Write(data[i].c_str(),len        );
        }
        changed = 0;
    }
};
#endif
