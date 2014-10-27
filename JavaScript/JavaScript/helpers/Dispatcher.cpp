#include "Dispatcher.h"

#include <process.h>

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

static log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("Dispatcher"));

HANDLE Dispatcher::dispatch(DWORD delay, bool interval)
{
	this->delay = delay;
	this->interval = interval;
	this->cancelEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);

	_beginthread(threadFunc, 0, this);
	return this->cancelEvent;
}

bool Dispatcher::cancel(HANDLE cancelEvent)
{
	BOOL ret = ::SetEvent(cancelEvent);
	return ret == TRUE;
}

bool Dispatcher::join(HANDLE cancelEvent, DWORD limit)
{
	DWORD wait = ::WaitForSingleObject(cancelEvent, limit);
	return wait == WAIT_OBJECT_0;
}

void Dispatcher::threadFunc(void* pVoid)
{
	Dispatcher* pThis = static_cast<Dispatcher*>(pVoid);
	try {
		pThis->threadFunc();
	} catch(...) {
	}
	delete pThis;
}

void Dispatcher::threadFunc()
{
	do {
		DWORD wait = WAIT_TIMEOUT;
		if(this->delay) wait = ::WaitForSingleObject(this->cancelEvent, this->delay);
		if(wait == WAIT_TIMEOUT) {
			try {
				LOG4CPLUS_INFO(logger, "calling timer ID=" << this->cancelEvent);
				this->call();
			} catch(std::exception&) {
				// Callback throws exception if 
			}
			// resume join()
			::SetEvent(this->cancelEvent);
		} else {
			// canceled
			break;
		}
	} while(this->interval);
}
