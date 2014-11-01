#include "stdafx.h"
#include "WindowTimer.h"

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

static log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("Dispatcher"));

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

bool Dispatcher::deleteTimer(TimerId timerId)
{
	Sync sync(timerId);
	timerId = sync.get();
	if(timerId) {
		::SetEvent(timerId->done);
		delete timerId->pThis;
		sync.destroy();
	}
	return timerId != NULL;
}

bool Dispatcher::stop(TimerId timerId)
{
	Sync sync(timerId);
	timerId = sync.get();
	bool ret = timerId != NULL;
	if(ret) {
		if(timeKillEvent(timerId->timer) == TIMERR_NOERROR) {
			LOG4CPLUS_ERROR(logger, "timeKillEvent() failed. error=" << ::GetLastError());
		}
		ret = deleteTimer(timerId);
	}
	return ret;
}

bool Dispatcher::join(TimerId timerId, DWORD limit)
{
	Sync sync(timerId);
	timerId = sync.get();
	bool ret = timerId != NULL;
	if(ret) {
		ret = WAIT_OBJECT_0 == ::WaitForSingleObject(timerId->done, limit);
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
		if(!::SetEvent(pThis->timerId->done)) {
			LOG4CPLUS_ERROR(logger, "SetEvent() failed. error=" << ::GetLastError());
		}
		// NOTE: for interval timer, stop() should be called.
		if(!pThis->interval) deleteTimer(timerId);
	}
}
