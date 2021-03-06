#pragma once

class Utils {
protected:
	static log4cplus::Logger logger;
};

class AutoHandle : public Utils {
public:
	// explicit prevents constructor from being called by implicit type conversion
	explicit AutoHandle(HANDLE hHandle = NULL, HANDLE hInvalid = NULL)
		: m_handle(hHandle), m_hInvalid(hInvalid) {
		LOG4CPLUS_TRACE(logger, __FUNCTION__ ": handle=" << m_handle << ",invalid=" << m_hInvalid);
	};

	virtual ~AutoHandle()
	{
		LOG4CPLUS_TRACE(logger, __FUNCTION__ ": handle=" << m_handle << ",invalid=" << m_hInvalid);
		if(isValid()) ::CloseHandle(m_handle);
	};

	inline AutoHandle& set(const HANDLE h) { m_handle = h; return *this; };
	inline operator HANDLE() const { return m_handle; };
	inline bool isValid() const { return m_handle != m_hInvalid; };

protected:
	HANDLE m_handle;
	HANDLE m_hInvalid;

private:
	AutoHandle(const AutoHandle&);
};

class AutoFileHandle : public AutoHandle {
public:
	explicit AutoFileHandle(HANDLE hHandle = INVALID_HANDLE_VALUE)
		: AutoHandle(hHandle, INVALID_HANDLE_VALUE) {};

private:
	AutoFileHandle(const AutoFileHandle&);
};

template<typename T>
class AutoArray : public Utils {
public:
	AutoArray(size_t size) {
		p = new T[size];
		LOG4CPLUS_TRACE(logger, __FUNCTION__ ": " << p << ",size=" << size);
	};
	virtual ~AutoArray() {
		LOG4CPLUS_TRACE(logger, __FUNCTION__ ": " << p);
		delete[] p;
	}
	inline T* get() const { return p; };
	inline operator T*() const { return p; };
protected:
	T* p;
};
