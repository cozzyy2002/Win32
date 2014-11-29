#pragma once

#include <windows.h>
#include <mmsystem.h>

#include "helpers/Callback.h"
#include "helpers/SynchronizedObject.h"

class Dispatcher {
protected:
	// allocated by SynchronizedObject
	typedef struct _Timer {
		Dispatcher* pThis;
		UINT timer;			// identifier returned by timeSetEvent()
		HANDLE done;		// event handle used to stop() and join()

		_Timer() : pThis(NULL), timer(0), done(NULL) {};
	} Timer;

public:
	typedef Timer* TimerId;
	typedef SynchronizedObject<Timer> Sync;

	TimerId start(UINT delay, bool interval);
	static bool stop(TimerId timerId);
	static bool join(TimerId timerId, DWORD limit);

protected:
	Dispatcher();
	virtual ~Dispatcher();

	static void CALLBACK onTimeout(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);
	virtual void call() = 0;

	TimerId timerId;
	bool interval;
};

typedef Dispatcher::TimerId TimerId;

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

template<class I>
TimerId setTimeout(I* listener, void (I::*method)(), DWORD delay = 0)
{
	P0Dispatcher<I>* disp = new P0Dispatcher<I>(listener, method);
	return disp->start(delay, false);
}

template<class I>
TimerId setInterval(I* listener, void (I::*method)(), DWORD delay = 0)
{
	P0Dispatcher<I>* disp = new P0Dispatcher<I>(listener, method);
	return disp->start(delay, true);
}

template<class I, typename P1>
TimerId setTimeout(I* listener, void (I::*method)(const P1&), P1 p1, DWORD delay = 0)
{
	P1Dispatcher<I, P1>* disp = new P1Dispatcher<I, P1>(listener, method, p1);
	return disp->start(delay, false);
}

inline bool clearTimeout(TimerId id) { return Dispatcher::stop(id); }
inline bool clearInterval(TimerId id) { return Dispatcher::stop(id); }
inline bool joinTimeout(TimerId id, DWORD limit = INFINITE) {return Dispatcher::join(id, limit); }
inline bool joinInterval(TimerId id, DWORD limit = INFINITE) {return Dispatcher::join(id, limit); }
