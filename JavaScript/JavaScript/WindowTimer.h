#pragma once

#include <Windows.h>

#include "helpers/Callback.h"

class Dispatcher {
public:
	HANDLE dispatch(DWORD delay = 0, bool interval = false);
	static bool cancel(HANDLE cancelEvent);
	static bool join(HANDLE cancelEvent, DWORD limit = INFINITE);

protected:
	Dispatcher() : cancelEvent(NULL) {};
	virtual ~Dispatcher() {
		if(cancelEvent) ::CloseHandle(cancelEvent);
	};

	void threadFunc();
	static void threadFunc(void* pVoid);

	virtual void call() = 0;

	HANDLE cancelEvent;
	DWORD delay;
	bool interval;
};

template<class I>
class P0Dispatcher : public Dispatcher {
public:
	typedef void (I::*F)();
	P0Dispatcher(I* listener, F method) : listener(listener), method(method) {};

	virtual void call() {
		P0Callback<I> callback(listener, method);
		callback();
	};

protected:
	I* listener;
	F method;
};

template<class I, typename P1>
class P1Dispatcher : public Dispatcher {
public:
	typedef void (I::*F)(const P1&);
	P1Dispatcher(I* listener, F method, P1 p1) : listener(listener), method(method), p1(p1) {};

	virtual void call() {
		P1Callback<I, const P1&> callback(listener, method);
		callback(p1);
	};

protected:
	I* listener;
	F method;
	const P1 p1;
};

typedef HANDLE TimerId;

template<class I>
TimerId setTimeout(I* listener, void (I::*method)(), DWORD delay = 0)
{
	P0Dispatcher<I>* disp = new P0Dispatcher<I>(listener, method);
	return disp->dispatch(delay, false);
}

template<class I, typename P1>
TimerId setTimeout(I* listener, void (I::*method)(const P1&), P1 p1, DWORD delay = 0)
{
	P1Dispatcher<I, P1>* disp = new P1Dispatcher<I, P1>(listener, method, p1);
	return disp->dispatch(delay, false);
}

extern bool clearTimeout(TimerId id);
extern bool joinTimeout(TimerId id, DWORD limit = INFINITE);
