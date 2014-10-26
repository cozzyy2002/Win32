#pragma once

#include <string>
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
	Callback() : listener(NULL), method(NULL) {};

	void bind(I* listener, F method, const char* name = NULL) {
		this->listener = listener;
		this->method = method;
		this->_name = name ? name : typeid(*listener).name();
	};
	void unbind() {
		listener = NULL;
		method = NULL;
	};

	const char* name() const { return _name.c_str(); };

	I* listener;
	F method;
	std::string _name;

	bool canApply() const {
		bool ret = (listener && method);
		std::cout << (ret ? "calling " : "can't call ") << typeid(method).name() << " of " << _name << std::endl;
		return ret;
	};

	bool has(I* listener, F method = NULL) const {
		return (listener == this->listener) && (!method || (method == this->method));
	};
	bool operator==(const Callback& other) const { return has(other.listener, other.method); };
};

template<class I, typename P1, typename R = void>
class P1Callback : public Callback<I, R (I::*)(P1)> {
public:
	R operator()(P1 p1) const {
		if(!canApply()) throw std::exception("can't apply");
		return (listener->*method)(p1);
	};
};

template<class I, typename P1, typename P2, typename R = void>
class P2Callback : public Callback<I, R (I::*)(P1, P2)> {
public:
	R operator()(P1 p1, P2 p2) const {
		if(!canApply()) throw std::exception("can't apply");
		return (listener->*method)(p1, p2);
	};
};
