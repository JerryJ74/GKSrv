#pragma once



// CCallSignalServerSocket ��� ����Դϴ�.

class CCallSignalServerSocket : public CAsyncSocket
{
public:
	CCallSignalServerSocket();
	virtual ~CCallSignalServerSocket();
	virtual void OnAccept(int nErrorCode);
};


