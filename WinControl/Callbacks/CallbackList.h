#pragma once

#include "Callback.h"

template<class I, typename F, class C>
class CallbackList : public std::vector<C> {
	typedef std::vector<C> callbacks_t;
public:
	bool addListener(I* listener, F method) {
		iterator i = find(listener, method);
		bool ok = (i == end());
		if(ok) {
			C callback;
			callback.bind(listener, method);
			push_back(callback);
		} else {
			std::cout << "ERROR: already listening: " << i->name() << std::endl;
		}
		return ok;
	};
	bool removeListener(I* listener) {
		iterator i = find(listener);
		bool ok = (i != end());
		if(ok) {
			erase(i);
		} else {
			std::cout << "ERROR: not listening: " << typeid(*listener).name() << std::endl;
		}
		return ok;
	};

	iterator find(I* listener, F method = NULL) {
		iterator i;
		for(i = begin(); i != end(); i++) {
			if(i->has(listener, method)) break;
		}
		return i;
	};
	iterator find(const Callback<I, F>& callback) { return find(callback.listener, callback.method); };
};

template<class I, typename P1>
class P1CallbackList : public CallbackList<I, void (I::*)(P1), P1Callback<I, P1>> {
public:
	void forEach(P1 n) const {
		for(const_iterator i = begin(); i < end(); i++) (*i)(n);
	};
};
