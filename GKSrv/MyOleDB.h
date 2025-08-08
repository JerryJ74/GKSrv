#pragma once

#include <oledb.h>
//**************************************************************************//
// 2004. 11. 18. db 구조의 유연함을 가져 가지 위해서 oledb 클래스 설계 함
// 
// 사용시 주의 사항 : ExecSQL() 함수 호출은 Sql을 실행하고 Table을 Close 하며
//                    OpenSQL() 함수는 RecordSet을 가져 오기 위함이다. 
//                    OpenSQL() 호출 이후 CloseSQL()을 꼭 호출 하여야 한다.
//
//
//                                               무한넷 코리아 개발실 장지만
//**************************************************************************//


// debug를 위한 임시 코드 이다.

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
