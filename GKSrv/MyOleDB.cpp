#include "StdAfx.h"
#include "MyOleDB.h"

// TEST를 위한 임시 Include 이다. 테스트 종료 후 삭제 하여야 함

// #include "MessageThread.h"

MyOleDB::MyOleDB(void)  : m_dbinit(DBPROPSET_DBINIT)
{
	m_opened = false;
}

MyOleDB::~MyOleDB(void)
{
	::CoUninitialize();
}

BOOL MyOleDB::OpenDatabase(char * ip, char * db, char * id, char * pass, DWORD port)
{
	char cntstr[256];
	memset(cntstr, 0, sizeof(cntstr));
	sprintf(cntstr, "PROVIDER=SQLOLEDB;SERVER=%s,%d;UID=%s;PWD=%s;DATABASE=%s;Auto Translate=True;Packet Size=4096;Use Encryption for Data=False;Tag with column collation when possible=False", ip, port, id, pass, db);

    m_dbinit.AddProperty(DBPROP_INIT_PROVIDERSTRING, _T(cntstr));

	::CoInitialize(NULL);

	HRESULT hr = m_datasource.Open(_T("SQLOLEDB"), &m_dbinit);

	if (FAILED(hr))
		return FALSE;

	hr = m_session.Open(m_datasource);

	if (FAILED(hr))
		return FALSE;
	else
		return TRUE;
}

/*
BOOL MyOleDB::OpenDatabase(char * ip, char * db, char * id, char * pass)
{
	char cntstr[256];
	memset(cntstr, 0, sizeof(cntstr));
	sprintf(cntstr, "PROVIDER=SQLOLEDB;SERVER=%s;UID=%s;PWD=%s;DATABASE=%s;Auto Translate=True;Packet Size=4096;Use Encryption for Data=False;Tag with column collation when possible=False", ip, id, pass, db);
	//Auto Translate=True;Packet Size=4096;Workstation ID=211.189.89.26;Use Encryption for Data=False;Tag with column collation when possible=False

	//m_dbinit.AddProperty(DBPROP_INIT_PROMPT, (short)4);
    m_dbinit.AddProperty(DBPROP_INIT_PROVIDERSTRING, _T(cntstr));
	//m_dbinit.AddProperty(DBPROP_INIT_LCID,(long)1043);

	::CoInitialize(NULL);

	HRESULT hr = m_datasource.Open(_T("SQLOLEDB"), &m_dbinit);

	if (FAILED(hr))
		return FALSE;

	hr = m_session.Open(m_datasource);

	//TEST
	//__RMT_DEBUG("success open database");


	if (FAILED(hr))
		return FALSE;
	else
		return TRUE;
}
*/

void MyOleDB::CloseDatabase()
{
	m_session.Close();
	m_datasource.Close();
}


BOOL MyOleDB::ExecSQL(char * sql, BOOL WithResult)
{
	DBROWCOUNT ef_count = 0;		
	HRESULT hr;
	BOOL r;

	//hr = m_command_sql.Execute(

	if (WithResult)
	{
		//hr = m_command.Open(m_session, _T(sql), 0, &ef_count, DBGUID_DBSQL, false, 0);
		//hr = m_command.Execute(
	
		//TEST
		//__RMT_DEBUG("open sql option is ef_count %s", sql);

		hr = m_command.Open(m_session, _T(sql), 0, &ef_count, DBGUID_DEFAULT, false, 0);
		if (SUCCEEDED(hr) && (ef_count > 0))
			r = TRUE;
		else 
			r = FALSE;

		//TEST
		//__RMT_DEBUG("sql is open %d", r);
	}
	else
	{
		//hr = m_command.Open(m_session, _T(sql));

		//TEST
		//__RMT_DEBUG("open sql %s", sql);


		hr = m_command.Open(m_session, _T(sql));
		if (FAILED(hr))
			r = FALSE;
		else
			r = TRUE;

		//TEST
		//__RMT_DEBUG("open sql %d", r);
	}
	//hr = m_reg.Open(m_reg, sql, 0, &ef_count, DBGUID_DBSQL, false, 0);

	// 2004. 11. 22. 주석 처리함.

	//__RMT_DEBUG("ExecSQL() closing");
	m_command.ReleaseCommand();
	m_command.Close();
	//__RMT_DEBUG("ExecSQL() closed");

	if (r)
		return TRUE;
	else
		return FALSE;
}


BOOL MyOleDB::OpenSQL(char * sql)
{
	//__RMT_DEBUG("OpenSQL(char * sql), sql : %s", sql);

	HRESULT hr = m_command.Open(m_session, _T(sql));

	if (SUCCEEDED(hr))
	{
		//m_command.MoveFirst();
		//__RMT_DEBUG("OpenSQL(char * sql) true");
		m_opened = TRUE;
		return TRUE;
	}
	else
	{
		//__RMT_DEBUG("OpenSQL(char * sql) false");
		m_opened = FALSE;
		return FALSE;
	}
}


void MyOleDB::CloseSQL()
{
	m_opened = FALSE;

	m_command.ReleaseRows();
	m_command.ReleaseCommand();

	//TEST
	//__RMT_DEBUG("open closing from CloseSQL()");
	m_command.Close();
	//__RMT_DEBUG("open closed from CloseSQL()");
}

int MyOleDB::GetInt(char * field)
{
	//__RMT_DEBUG("GetInt() field:%s", field);

	void * r = m_command.GetValue((const CHAR *)field);
	
	int * temp = (int *)r;
	int rr;

	if (r)
		rr = *temp;
	else 
		rr = -1;

	//__RMT_DEBUG("GetInt() field:%s, r:%d", field, rr);

	return (int)rr;
}


BOOL MyOleDB::GetChar(char * field, char * result)
{
	void * temp = m_command.GetValue((const CHAR *)field);

	if (temp)
	{
		strcpy(result, (const char *)temp);
		
		// char * 형태의 마지막 " " 를 없애기 위한 코드
		size_t s = strlen(result);
		int c = 0;
		while (TRUE)
		{
			if (result[s - c - 1] == 32)
				result[s - c - 1] = 0;
			else
				break;
			c++;
		}
		return TRUE;
	}
	else
		return FALSE;
}


char * MyOleDB::GetChar(char * field)
{
	void * temp = m_command.GetValue((const CHAR *)field);
	char * r = (char *)temp;

	if (temp)
	{
		// char * 형태의 마지막 " " 를 없애기 위한 코드
		size_t s = strlen(r);
		int c = 0;
		while (TRUE)
		{
			if (r[s - c - 1] == 32)
				r[s - c - 1] = 0;
			else
				break;
			c++;
		}
		return r;
	}
	else
		return NULL;
}


BOOL MyOleDB::GetDate(char * field, DBTIMESTAMP & dbtime)
{
	void * temp = m_command.GetValue((const CHAR *)field);

	if (temp)
	{
		memcpy(&dbtime, (const void *)temp, sizeof(DBTIMESTAMP));
		return TRUE;
	}
	else
		return FALSE;
}


HRESULT MyOleDB::MoveNext()
{
	//__RMT_DEBUG("move next");

	HRESULT hr = m_command.MoveNext();
	return hr;
}


HRESULT MyOleDB::MovePrev()
{
	//__RMT_DEBUG("move prev");

	HRESULT hr = m_command.MovePrev();
	return hr;
}


HRESULT MyOleDB::MoveFirst()
{
	//__RMT_DEBUG("move first");

	HRESULT hr = m_command.MoveFirst();
	return hr;
}
