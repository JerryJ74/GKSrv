#pragma once

#include <oledb.h>
//**************************************************************************//
// 2004. 11. 18. db ������ �������� ���� ���� ���ؼ� oledb Ŭ���� ���� ��
// 
// ���� ���� ���� : ExecSQL() �Լ� ȣ���� Sql�� �����ϰ� Table�� Close �ϸ�
//                    OpenSQL() �Լ��� RecordSet�� ���� ���� �����̴�. 
//                    OpenSQL() ȣ�� ���� CloseSQL()�� �� ȣ�� �Ͽ��� �Ѵ�.
//
//
//                                               ���ѳ� �ڸ��� ���߽� ������
//**************************************************************************//


// debug�� ���� �ӽ� �ڵ� �̴�.

class MyOleDB
{
public:
	MyOleDB(void);
	~MyOleDB(void);
	
	//BOOL OpenDatabase(char * ip, char * db, char * id, char * pass);
	BOOL OpenDatabase(char * ip, char * db, char * id, char * pass, DWORD port);
	void CloseDatabase();

	BOOL ExecSQL(char * sql, BOOL WithResult = false);
	BOOL OpenSQL(char * sql);
	void CloseSQL();

	HRESULT MoveNext();
	HRESULT MovePrev();
	HRESULT MoveFirst();

	int GetInt(char * field);
	BOOL GetChar(char * field, char * result);
	char * GetChar(char * field);
	BOOL GetDate(char * field, DBTIMESTAMP & dbtime);

	BOOL IsOpened() { return m_opened; };
protected:
	CCommand<CDynamicAccessor, CRowset> m_command;
	CCommand<CNoAccessor> m_command_sql;
	CDataSource m_datasource;
	CSession m_session;
	CDBPropSet m_dbinit;

	BOOL m_opened;


	/////temp
	//void _DD(char * m, ...);
	//DWORD m_debugline;
	//CFile m_file;
};
