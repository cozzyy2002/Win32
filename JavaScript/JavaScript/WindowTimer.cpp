#include "stdafx.h"
#include "WindowTimer.h"

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

static log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("Dispatcher"));

TimerId Dispatcher::start(UINT delay, bool interval)
{
	this->interval = interval;

	hHeap = ::GetProcessHeap();
	timerId = (TimerId)::HeapAlloc(hHeap, 0, sizeof(Timer));

	timerId->timer = timeSetEvent(delay, 100, onTimeout, (DWORD_PTR)this, interval ? TIME_PERIODIC : TIME_ONESHOT);
	if(timerId->timer) {
		timerId->done = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	} else {
		LOG4CPLUS_ERROR(logger, "timeSetEvent() failed. error=" << ::GetLastError());
	}
	return timerId;
}

bool Dispatcher::validateTimer(TimerId timerId)
{
	bool ret = ::HeapValidate(::GetProcessHeap(), 0, timerId) ? true : false;
	if(!ret) {
		LOG4CPLUS_WARN(logger, "timer may have stopped.");
	}
	return ret;
}

bool Dispatcher::deleteTimer(TimerId timerId)
{
	bool ret = validateTimer(timerId);
	if(ret) {
		::SetEvent(timerId->done);
		if(!::HeapFree(::GetProcessHeap(), 0, timerId)) {
			LOG4CPLUS_WARN(logger, "HeapFree() failed. error=" << ::GetLastError());
		}
	}
	return ret;
}

bool Dispatcher::stop(TimerId timerId)
{
	bool ret = validateTimer(timerId);
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
	bool ret = validateTimer(timerId);
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
	Dispatcher* pThis = static_cast<Dispatcher*>((void*)dwUser);
	try {
		pThis->call();
	} catch(...) {
		LOG4CPLUS_ERROR(logger, "worker thread failed.");
	}
	if(validateTimer(pThis->timerId)) {
		if(!::SetEvent(pThis->timerId->done)) {
			LOG4CPLUS_ERROR(logger, "SetEvent() failed. error=" << ::GetLastError());
		}
	}
	// NOTE: for interval timer, stop() should be called.
	if(!pThis->interval) delete pThis;
}
