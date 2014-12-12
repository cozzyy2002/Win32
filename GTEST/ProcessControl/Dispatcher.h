#pragma once

template<class T>
class CDispatcher {
public:
	typedef void (T::*func_t)();

	CDispatcher(T* target, typename func_t func)
		: m_target(target), m_func(func), m_thread(NULL) {};
	~CDispatcher() {
		if(m_thread) ::CloseHandle(m_thread);
	};

	/**
		Dispatch worker thread function.

		Returns thread handle. the handle will be closed when thread function exits.
	*/
	HANDLE dispatch() {
		HANDLE thread = ::CreateThread(NULL, 0, ThreadProc, this, 0, NULL);
		if(thread) {
			m_thread = thread;
		} else {
			delete this;
		}
		return thread;
	};

	static DWORD WINAPI ThreadProc(LPVOID lpParameter) {
		CDispatcher* pThis = (CDispatcher*)lpParameter;
		((pThis->m_target)->*(pThis->m_func))();
		delete pThis;
		return 0;
	}

protected:
	T* m_target;
	func_t m_func;
	HANDLE m_thread;
};

template<class T>
HANDLE dispatch(T* target, typename CDispatcher<T>::func_t func)
{
	CDispatcher<T>* disp = new CDispatcher<T>(target, func);
	return disp->dispatch();
}
