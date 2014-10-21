// Callbacks.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace std;

/**
 * callback interface.
 * calledback class or event listner class should implement this interface.
 */
class ICallbacks {
public:
	virtual void name() =0;
	virtual void age(int n) = 0;
	virtual void location(const char* state, const char* city) = 0;
};
class X : public ICallbacks {
public:
	void x() {};
	virtual void name() { cout << "class X" << endl; };
	virtual void age(int n) { cout << "age=" << to_string(n) << endl; };
	virtual void location(const char* state, const char* city) { cout << state << "," << city << endl; };
};
class Y : public X {
public:
	void y() {};
	virtual void name() { cout << "class Y" << endl; };
	virtual void age(int n) { cout << "age=" << to_string(n + 10) << endl; };
	virtual void location(const char* state, const char* city) { cout << "unknown" << endl; };
};

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
	F getMethod() const { return (listener && method) ? method : NULL; };
	const char* name() const { return _name.c_str(); };

	I* listener;
	F method;
	string _name;

	bool canApply() const {
		bool ret = (listener && method);
		cout << (ret ? "calling " : "can't call ") << typeid(method).name() << " of " << _name << endl;
		return ret;
	};
	void operator()() const { if(canApply()) (listener->*method)(); };
	void operator()(int n) const { if(canApply()) (listener->*method)(n); };
	void operator()(const char* a, const char* b) const { if(canApply()) (listener->*method)(a, b); };

	bool has(I* listener, F method = NULL) const {
		return (listener == this->listener) && (!method || (method == this->method));
	};
	bool operator==(const Callback& other) const { return has(other.listener, other.method); };
};

template<class I, typename F>
class CallbackList : public vector<Callback<I, F>> {
	typedef vector<Callback<I, F>> callbacks_t;
public:
	bool addListener(I* listener, F method) {
		iterator i = find(listener, method);
		bool ok = (i == end());
		if(ok) {
			Callback<I, F> callback;
			callback.bind(listener, method);
			push_back(callback);
		} else {
			cout << "ERROR: already listening: " << i->name() << endl;
		}
		return ok;
	};
	bool removeListener(I* listener) {
		iterator i = find(listener);
		bool ok = (i != end());
		if(ok) {
			erase(i);
		} else {
			cout << "ERROR: not listening: " << typeid(*listener).name() << endl;
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
class Arg1CallbackList : public CallbackList<I, void (I::*)(P1)> {
public:
	void forEach(P1 n) {
		for(const_iterator i = begin(); i < end(); i++) (*i)(n);
	};
};

template<int i>
string IntFunc(int value) { return to_string(value + i); };

template<const char c>
string CharFunc(int value) { char s[2] = { c, '\0' }; return s; };

template<bool b>
string BoolFunc(int value) { return b ? "true" : "false"; };

template<typename ValueType, typename RetType = ValueType>
class CalcFuncs {
public:
	typedef RetType (CalcFuncs::*Func)(ValueType) const;
	CalcFuncs(Func func, ValueType value) : func(func), value(value) {};

	RetType operator()(ValueType arg) const { return (this->*func)(arg); };

	RetType Fix(ValueType)     const { return (RetType)(value); };
	RetType Mul(ValueType arg) const { return (RetType)(value * arg); };
	RetType Add(ValueType arg) const { return (RetType)(value + arg); };

private:
	Func func;
	ValueType value;
};

#define CALC_FUNCS CalcFuncs<double>
#define FIX_VALUE(v) CALC_FUNCS(&CALC_FUNCS::Fix, v)
#define MUL_VALUE(v) CALC_FUNCS(&CALC_FUNCS::Mul, v)
#define ADD_VALUE(v) CALC_FUNCS(&CALC_FUNCS::Add, v)

class MyException : public exception {
public:
	MyException(const char* reason) : exception(reason) {};

	const char* msg() const { return "MyException()"; };
};

ostream& operator<<(ostream& os, const exception& e)
{
	os << "exception is: " << ((typeid(e) == typeid(MyException)) ? dynamic_cast<const MyException*>(&e)->msg() : e.what());
	return os;
}

int _tmain(int argc, _TCHAR* argv[])
{
	cout << "---- function table ----" << endl;
	typedef string (*Calc)(int value);
	static const Calc calcs[] = {
		to_string, BoolFunc<true>,
		CharFunc<'0'>, CharFunc<'4'>,
		IntFunc<10>, IntFunc<-20>,
	};

	for(int i = 0; i < sizeof(calcs)/sizeof(calcs[0]); i++) {
		cout << "calcs[" << i << "]=" << (*calcs[i])(100) << endl;
	}

	cout << "---- function table ----" << endl;
	static const CALC_FUNCS calcFuncs[] = {
		FIX_VALUE(1.23),
		MUL_VALUE(0.321), MUL_VALUE(0.765),
		ADD_VALUE(-10.555),
	};
	for(int i = 0; i < sizeof(calcFuncs)/sizeof(calcFuncs[0]); i++) {
		cout << "calcFuncs[" << i << "]=" << calcFuncs[i](100) << endl;
	}

	X x;
	Y y;

	cout << "---- vector<Callback<I, F>> addListener ----" << endl;
	Arg1CallbackList<ICallbacks, int> callbacks;
	callbacks.addListener(&x, &ICallbacks::age);
	callbacks.addListener(&x, &ICallbacks::age);
	callbacks.addListener(&y, &ICallbacks::age);
	// equivalent to for(size_t i = 0; i < callbacks.size(); i++) callbacks[i](-20);
	callbacks.forEach(-20);
	cout << "---- vector<Callback<I, F>> removeListener ----" << endl;
	callbacks.removeListener(&x);
	callbacks.removeListener(&x);
	// equivalent to for(size_t i = 0; i < callbacks.size(); i++) callbacks[i](123);
	callbacks.forEach(123);

	cout << "---- Callback<I, F> ----" << endl;
	Callback<ICallbacks, void (ICallbacks::*)(int)> callback;
	callback.bind(&x, &ICallbacks::age, "void ICallback::age(int)");
	callback(20);
	callback.unbind();
	callback(120);

	Callback<ICallbacks, void (ICallbacks::*)(const char*, const char*)> locationCallback;
	locationCallback.bind(&x, &ICallbacks::location, "LocationCallback(state, city)");
	locationCallback("tokyo", "shibuya");
	locationCallback.unbind();
	locationCallback("", NULL);

	void (ICallbacks::*pName)() = &ICallbacks::name;
	(x.*pName)();
	(y.*pName)();

	ICallbacks* c = &x;

	c->name();
	c = &y;
	c->name();

	cout << "---- MyException::operator<< ----" << endl;
	try {
		throw MyException("xxx");
	} catch(MyException& e) {
		cout << "test: " << e << endl;
	}
	return 0;
}
