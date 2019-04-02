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
//��r
extern int compare(const wString arg1, const wString op , const wString arg2, const dataType type );
class view;
/////////////////////////////////////////////////////////////////////////////
//�J�����N���X
class Column
{
private:
public:
    wString  table;//�擾���e�[�u����
    wString  name; //�J������
    dataType type; //�f�[�^�^
    //�J�����R���X�g���N�^
    Column(void);
    Column(const wString& tbl, const wString& nam, const dataType typ=STRING);
    Column(const Column* clm);
    //�e�[�u�����L�����l���������v�I�� select = TableName.ColumnName
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
//�����N���X
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
//�e�[�u���N���X
class Table
{
private:
    int             ref;          //read thread��
#ifdef linux
    pthread_mutex_t mutex;        //�N���e�B�J���Z�b�V����mutex
#else
    CRITICAL_SECTION cs;          //�N���e�B�J���Z�N�V����
#endif
public:
    wString         name;         //�e�[�u����
    vector<Column*> column;       //�J����
    vector<Node*>   node;         //�e�m�[�h
    vector<int>     index;        //orderby���̃t�B���^
    //�Z�b�V�����Ǘ�
    void readStart();
    void readEnd();
    void writeStart();
    void writeEnd();
    //�e���v���[�g
    Table(void);
    //CSV����e�[�u���쐬
    Table(const char* myname, const char* mycolumn);
    //String�z�񂩂�e�[�u���쐬
    Table(const wString myname, const vector<wString>& mycolumn);
    //�J������񂩂�e�[�u���쐬
    Table(const wString myname, const vector<Column*> mycolumn);
    void copy(Table* tbl);
    //�f�X�g���N�^
    ~Table();
    //�e�[�u���f�[�^�擾
    vector<Node*> getTable(void);
    //�e�[�u���C���T�[�g(CSV)
    //�J�������������ĂȂ����`�F�b�N���ĂȂ��̂Ŏc���NULL��ǉ�
    int Insert(const char* data);
    //�J�����ƃf�[�^���w��
    int Insert(const vector<wString>& clm,const vector<wString>& data);
    //�e�[�u���C���T�[�g
    int Insert(const vector<wString>& data);
    //�����z�񐶐�
    int condition_mat(condition& cond, vector<char>& mat);
    //�e�[�u���X�V
    int Update(const vector<wString>& colnams,const vector<wString>& values,condition& cond);
    //�e�[�u���폜
    int Delete(condition& cond);
    //���A
    int  LoadFromFile(bufrd& br);
    //�ۑ�
    void SaveToFile(bufrd& br);
    //�ύX�����
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
    pthread_mutex_t mutex;        //�N���e�B�J���Z�b�V����mutex
#else
    CRITICAL_SECTION cs;          //�N���e�B�J���Z�N�V����
#endif
public:
    map<wString,Database*> dblist;
    ////////////////////////////////////////////////////////////////////////////
    //�V���O���g���擾
    //static DBCatalog& getInstance(void);
    ////////////////////////////////////////////////////////////////////////////
    //�f�[�^�x�[�X�ɐڑ�����
    Database* DBConnect(const wString& DBName);
    ////////////////////////////////////////////////////////////////////////////
    //�f�[�^�x�[�X�����
    int DBClose(Database* db);
    ////////////////////////////////////////////////////////////////////////////
    //�V�K��DB�쐬
    Database* DBCreate(const wString DBName);
    ////////////////////////////////////////////////////////////////////////////
    //DB�N���[�Y
    void DBClose(const wString& DBName);
    ////////////////////////////////////////////////////////////////////////////
    size_t ShowCatalog(wString& retStr);
    ////////////////////////////////////////////////////////////////////////////
    int LoadFromFile(void);
    ////////////////////////////////////////////////////////////////////////////
    int SaveToFile(void);
    ////////////////////////////////////////////////////////////////////////////
    //�R���X�g���N�^
    DBCatalog(void);
    ////////////////////////////////////////////////////////////////////////////
    //�f�X�g���N�^
    ~DBCatalog(void);
};
extern DBCatalog* catalog;
#endif