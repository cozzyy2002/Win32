#pragma once

#include <string>

class CPipe {
public:
	CPipe();
	virtual ~CPipe();

	void create();
	void connect();
	void send(const std::string& str, DWORD timeout = 0);
	std::string receive(DWORD timeout = INFINITE);

protected:
	DWORD writePipe(LPCVOID data, size_t size, DWORD timeout);
	DWORD readPipe(LPVOID data, size_t size, DWORD timeout);
	DWORD getOverlappedResult(OVERLAPPED* pov, DWORD timeout = INFINITE);
	void waitEvent(HANDLE hEvent, DWORD timeout = INFINITE);

	HANDLE m_hPipe;
	HANDLE m_hReadyToWriteEvent;	/// Connected and ready to write to the pipe
	HANDLE m_hReadCompletedEvent;	/// Received any message in the pipe

	typedef struct _Overlapped : public OVERLAPPED {
		_Overlapped(HANDLE hEvent)
		{
			ZeroMemory(this, sizeof(_Overlapped));
			this->hEvent = hEvent;
		};
	} Overlapped;
};

template<typename T>
class AutoArray {
public:
	AutoArray(size_t size) { p = new T[size]; };
	~AutoArray() { delete[] p; }
	T* get() const { return p; };
	operator T*() const { return p; };
protected:
	T* p;
};
