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
//�J�����N���X
class Column
{
public:
    string   table;//�擾���e�[�u����
    string   name; //�J������
    string   alias;//�G�C���A�X
    dataType type; //�f�[�^�^
    //�J�����R���X�g���N�^
    Column(void);
    Column(const string tbl, const string nam, const dataType typ=STRING);
    Column(const Column* clm);
    //void dump(void);
    //�e�[�u�����L�����l���������v�I�� select = TableName.ColumnName
    int Compare( string select);
    int SaveToFile(int fd);
    int LoadFromFile(int fd);
};
/////////////////////////////////////////////////////////////////////////////
//�e�[�u���N���X
class Table
{
public:
    string          name;         //�e�[�u����
    string          alias;        //�e�[�u���G�C���A�X
    vector<Column*> column;       //�J����
    vector<Node*>   node;         //�e�m�[�h
    vector<int>     index;        //orderby���̃t�B���^
    int             ref;          //read thread��
#ifdef linux
    pthread_mutex_t mutex;        //�N���e�B�J���Z�b�V����mutex
#else
    CRITICAL_SECTION cs;          //�N���e�B�J���Z�N�V����
#endif
    //�R���X�g���N�^
    //myname:�e�[�u����
    //mycolumn:�J�������
    //void cd(void);
    //�Z�b�V�����Ǘ�
    void readStart();
    void readEnd();
    void writeStart();
    void writeEnd();
    //�e���v���[�g
    Table(void);
    //CSV����e�[�u���쐬
    Table(char* myname, char* mycolumn);
    //String�z�񂩂�e�[�u���쐬
    Table(string myname, vector<string> mycolumn);
    //�J������񂩂�e�[�u���쐬
    Table(string myname, vector<Column*> mycolumn);
    void copy(const Table* tbl);
    //�R�s�[�R���X�g���N�^
    //Table(const Table& tbl);
    //�f�X�g���N�^
    ~Table();
    //�e�[�u���f�[�^�擾
    vector<Node*> getTable(void);
    //�e�[�u���C���T�[�g(CSV)
    //�J�������������ĂȂ����`�F�b�N���ĂȂ��̂Ŏc���NULL��ǉ�
    int Insert(const char* data);
    //�J�����ƃf�[�^���w��
    int Table::Insert(const vector<string>& clm,const vector<string>& data);
    //�e�[�u���C���T�[�g
    int Insert(const vector<char*>& data);
    //�e�[�u���I��
    Table* Select(const char* selection);
    //�e�[�u���I��
    Table* Select(const vector<string>& colnums);
    //���בւ�
    Table* OrderBy(string targetColumn,orderType type=ASC);
    //�e�[�u���ɃJ�����ǉ�
    void AddColumn(vector<Column*> addColumn);
    //������Left join
    Table* LeftJoin(Table* joinTable, string arg1, char* op, string arg2);
    //intersection
    Table* Intersection(Table* Table2);
    //�e�[�u���i������
    Table* Where(const string arg1, const char* op, const string arg2);
    //��r
    int compare(const string arg1, const string op , const string arg2, const dataType type );
    //�e�[�u���X�V
    int Table::Update(const vector<string> colnams,const vector<string> values,const string arg1, const char* op, const string arg2);
    //�e�[�u���폜
    int Table::Delete(const string arg1, const char* op, const string arg2);
    //�V���A���C�Y
    string toString(void);
    //���A
    int LoadFromFile(int fd);
    //�ۑ�
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
    //�V���O���g���擾
    static DBCatalog& getInstance(void);
    ////////////////////////////////////////////////////////////////////////////
    //�f�[�^�x�[�X�ɐڑ�����
    Database* DBConnect(char* DBName);
    ////////////////////////////////////////////////////////////////////////////
    //�V�K��DB�쐬
    Database* DBCreate(char* DBName);
    ////////////////////////////////////////////////////////////////////////////
    //DB�N���[�Y
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