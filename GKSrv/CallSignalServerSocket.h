#pragma once



// CCallSignalServerSocket 명령 대상입니다.

class CCallSignalServerSocket : public CAsyncSocket
{
public:
	CCallSignalServerSocket();
	virtual ~CCallSignalServerSocket();
	virtual void OnAccept(int nErrorCode);
};


