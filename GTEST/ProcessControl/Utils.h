#pragma once

class AutoHandle {
public:
	AutoHandle(HANDLE h = NULL, HANDLE hInvalid = NULL)
		: m_handle(h), m_hInvalid(hInvalid) {};
	~AutoHandle() {
		if(m_handle != m_hInvalid) ::CloseHandle(m_handle);
	};
	HANDLE operator=(HANDLE h) { m_handle = h; return m_handle; };
	operator HANDLE() const { return m_handle; };

protected:
	HANDLE m_handle;
	HANDLE m_hInvalid;
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
