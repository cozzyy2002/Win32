#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <exception>

/**
 * template class to call callback method or event listner method.
 *
 * @param I		interface class which define method to be called
 * @param F		type of the method
 */
template<class I, typename F>
class Callback {
public:
	/**
	 * constructor.
	 * @param[in,optional] name		instance name.
	 *								if omitted, method type name that compiler generated is used.
	 */
	Callback(I* listener = NULL, F method = NULL) : listener(listener), method(method) {};

	void bind(I* listener, F method, const char* name = NULL) {
		this->listener = listener;
		this->method = method;
		if(name) this->_name = name;
	};
	void unbind() {
		listener = NULL;
		method = NULL;
	};

	const char* name() const { return (_name.size() ? _name.c_str() : (method ? typeid(method).name() : typeid(I).name())); };

	I* listener;
	F method;
	std::string _name;

	bool canApply(bool throwError = false) const {
		bool ret = (listener && method);
		std::cout << (ret ? "calling " : "can't call ") << name() << std::endl;
		if(!ret && throwError) throw std::exception("unbound");
		return ret;
	};

	bool has(I* listener, F method = NULL) const {
		return (listener == this->listener) && (!method || (method == this->method));
	};
	bool operator==(const Callback& other) const { return has(other.listener, other.method); };
	operator bool() const { return (listener && method); };
};

template<class I, typename R = void>
class P0Callback : public Callback<I, R (I::*)()> {
public:
	typedef R (I::*F)();
	P0Callback(I* listener = NULL, F method = NULL) : Callback<I, F>(listener, method) {};
	R operator()() const {
		canApply(true) ;
		return (listener->*method)();
	};
};

template<class I, typename P1, typename R = void>
class P1Callback : public Callback<I, R (I::*)(P1)> {
public:
	typedef R (I::*F)(P1);
	P1Callback(I* listener = NULL, F method = NULL) : Callback<I, F>(listener, method) {};
	R operator()(P1 p1) const {
		canApply(true) ;
		return (listener->*method)(p1);
	};
};
