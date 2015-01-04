#pragma once

class Utils {
protected:
	static log4cplus::Logger logger;
};

class AutoHandle : public Utils {
public:
	AutoHandle(HANDLE h = NULL, HANDLE hInvalid = NULL)
		: m_handle(h), m_hInvalid(hInvalid) {
		LOG4CPLUS_TRACE(logger, __FUNCTION__ ": handle=" << m_handle << ",invalid=" << m_hInvalid);
	};
	~AutoHandle() {
		LOG4CPLUS_TRACE(logger, __FUNCTION__ ": handle=" << m_handle << ",invalid=" << m_hInvalid);
		if(isValid()) ::CloseHandle(m_handle);
	};

	HANDLE operator=(HANDLE h) { m_handle = h; return m_handle; };
	operator HANDLE() const { return m_handle; };
	bool isValid() const { return m_handle != m_hInvalid; };

protected:
	HANDLE m_handle;
	HANDLE m_hInvalid;
};

template<typename T>
class AutoArray : public Utils {
public:
	AutoArray(size_t size) {
		p = new T[size];
		LOG4CPLUS_TRACE(logger, __FUNCTION__ ": " << p << ",size=" << size);
	};
	~AutoArray() {
		LOG4CPLUS_TRACE(logger, __FUNCTION__ ": " << p);
		delete[] p;
	}
	T* get() const { return p; };
	operator T*() const { return p; };
protected:
	T* p;
};
