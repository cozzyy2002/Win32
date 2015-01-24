#pragma once

#include "Utils.h"
#include <string>

class CPipe {
public:
	CPipe();
	virtual ~CPipe();

	void create(DWORD defaultTimeout = 0, DWORD inBufferSize = 128, DWORD outBufferSize = 128);
	void connect();
	void send(const std::string& str, DWORD timeout = 0);
	std::string receive(DWORD timeout = INFINITE);

protected:
	DWORD writePipe(LPCVOID data, size_t size, DWORD timeout);
	DWORD readPipe(LPVOID data, size_t size, DWORD timeout);
	DWORD getOverlappedResult(OVERLAPPED* pov, DWORD timeout = INFINITE);
	void waitEvent(HANDLE hEvent, DWORD timeout = INFINITE);

	AutoFileHandle m_hPipe;
	AutoHandle m_hReadyToWriteEvent;	/// Connected and ready to write to the pipe
	AutoHandle m_hReadCompletedEvent;	/// Received any message in the pipe
	DWORD m_OutBufferSize;
	DWORD m_InBufferSize;

	typedef struct _Overlapped : public OVERLAPPED {
		_Overlapped(HANDLE hEvent)
		{
			ZeroMemory(this, sizeof(_Overlapped));
			this->hEvent = hEvent;
		};
	} Overlapped;
};
