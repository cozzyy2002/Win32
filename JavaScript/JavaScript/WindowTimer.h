#pragma once

#include "stdafx.h"
#include "helpers/Dispatcher.h"

typedef HANDLE TimerId;

template<class I>
TimerId setTimeout(I* listener, void (I::*func)(), DWORD delay = 0)
{
	return dispatch<I>(listener, func, delay);
}
