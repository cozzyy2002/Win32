#include "stdafx.h"
#include "WindowTimer.h"

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

static log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("Dispatcher"));

TimerId Dispatcher::start(UINT delay, bool interval)
{
	TimerId timerId = timeSetEvent(delay, 100, onTimeout, (DWORD_PTR)this, interval ? TIME_PERIODIC : TIME_ONESHOT);
	if(timerId) {
		joinEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	} else {
		LOG4CPLUS_ERROR(logger, "timeSetEvent() failed. error=" << ::GetLastError());
	}
	return timerId;
}

bool Dispatcher::stop(TimerId id)
{
	bool ret = timeKillEvent(id) == TIMERR_NOERROR;
	if(!ret) LOG4CPLUS_ERROR(logger, "timeKillEvent() failed. error=" << ::GetLastError());
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
	if(!::SetEvent(pThis->joinEvent)) {
		LOG4CPLUS_ERROR(logger, "SetEvent() failed. error=" << ::GetLastError());
	}
	delete pThis;
}
