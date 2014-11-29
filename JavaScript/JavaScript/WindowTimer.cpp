#include "stdafx.h"
#include "WindowTimer.h"

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

static log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("Dispatcher"));

Dispatcher::Dispatcher() : timerId(NULL)
{
}

Dispatcher::~Dispatcher()
{
	Sync sync(timerId);
	if(sync.get()) {
		if(!::SetEvent(timerId->done)) {
			LOG4CPLUS_ERROR(logger, "SetEvent() failed. error=" << ::GetLastError());
		}
		sync.destroy();
	}
}

TimerId Dispatcher::start(UINT delay, bool interval)
{
	this->interval = interval;

	Sync sync;
	timerId = sync.get();

	timerId->pThis = this;
	timerId->timer = timeSetEvent(delay, 100, onTimeout, (DWORD_PTR)timerId, interval ? TIME_PERIODIC : TIME_ONESHOT);
	if(timerId->timer) {
		timerId->done = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	} else {
		LOG4CPLUS_ERROR(logger, "timeSetEvent() failed. error=" << ::GetLastError());
	}
	return timerId;
}

bool Dispatcher::stop(TimerId timerId)
{
	Sync sync(timerId);
	timerId = sync.get();
	bool ret = timerId != NULL;
	if(ret) {
		if(timeKillEvent(timerId->timer) != TIMERR_NOERROR) {
			LOG4CPLUS_WARN(logger, "timeKillEvent() failed. error=" << ::GetLastError());
		}
		delete timerId->pThis;
	}
	return ret;
}

bool Dispatcher::join(TimerId timerId, DWORD limit)
{
	HANDLE done = NULL;
	{
		// lock while retrieving done event handle
		Sync sync(timerId);
		timerId = sync.get();
		if(timerId) done = timerId->done;
	}
	bool ret = done != NULL;
	if(ret) {
		ret = WAIT_OBJECT_0 == ::WaitForSingleObject(done, limit);
		if(!ret) {
			LOG4CPLUS_WARN(logger, "joined by timeout or error. error=" << ::GetLastError());
		}
	}
	return ret;
}

void Dispatcher::onTimeout(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{

	Sync sync((void*)dwUser);
	TimerId timerId = sync.get();
	if(timerId) {
		Dispatcher* pThis = timerId->pThis;
		try {
			pThis->call();
		} catch(...) {
			LOG4CPLUS_ERROR(logger, "worker thread failed.");
		}
		// NOTE: for interval timer, stop() should be called.
		if(!pThis->interval) delete pThis;
	}
}
