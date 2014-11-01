#include "stdafx.h"
#include "WindowTimer.h"

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
	bool ret = ::SetEvent(cancelEvent) ? true : false;
	if(!ret) LOG4CPLUS_ERROR(logger, "SetEvent() failed. error=" << ::GetLastError());
	return ret;
}

bool Dispatcher::join(HANDLE cancelEvent, DWORD limit)
{
	DWORD wait = ::WaitForSingleObject(cancelEvent, limit);
	bool ret = (wait == WAIT_OBJECT_0);
	if(!ret) LOG4CPLUS_TRACE(logger, "WaitForSingleObject() returned " << wait << ",error=" << ::GetLastError());
	return ret;
}

void Dispatcher::threadFunc(void* pVoid)
{
	Dispatcher* pThis = static_cast<Dispatcher*>(pVoid);
	try {
		pThis->threadFunc();
	} catch(...) {
		LOG4CPLUS_ERROR(logger, "worker thread failed.");
	}
	delete pThis;
}

void Dispatcher::threadFunc()
{
	do {
		DWORD wait = WAIT_TIMEOUT;
		if(this->delay) {
			LOG4CPLUS_TRACE(logger, "waiting " << this->delay << "mSec,timer ID=" << this->cancelEvent);
			wait = ::WaitForSingleObject(this->cancelEvent, this->delay);
		}
		switch(wait) {
		case WAIT_TIMEOUT:
			try {
				LOG4CPLUS_TRACE(logger, "calling timer ID=" << this->cancelEvent);
				this->call();
			} catch(std::exception& e) {
				// Callback throws exception if 
				LOG4CPLUS_ERROR(logger, "timout method threw: " << e.what());
			}
			break;
		case WAIT_OBJECT_0:
			LOG4CPLUS_TRACE(logger, "canceled timer ID=" << this->cancelEvent);
			break;
		default:
			LOG4CPLUS_ERROR(logger, "wait faild: wait=" << wait << ",error=" << ::GetLastError());
			break;
		}
	} while(this->interval);
	// resume join()
	::SetEvent(this->cancelEvent);
}

bool clearTimeout(TimerId id)
{
	return Dispatcher::cancel(id);
}

bool joinTimeout(TimerId id, DWORD limit)
{
	return Dispatcher::join(id, limit);
}
