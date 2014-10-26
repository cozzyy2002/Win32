// Callbacks.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <iostream>
#include <map>

#include "CallbackList.h"

using namespace std;

/**
 * callback interface.
 * calledback class or event listner class should implement this interface.
 */
class ICallbacks {
public:
	virtual void name() =0;
	virtual void age(int n) = 0;
	virtual string location(const char* state, const char* city) = 0;
};
class X : public ICallbacks {
public:
	void x() {};
	virtual void name() { cout << "class X" << endl; };
	virtual void age(int n) { cout << "age=" << to_string(n) << endl; };
	virtual string location(const char* state, const char* city) { cout << state << "," << city << endl; return state;};
};
class Y : public X {
public:
	void y() {};
	virtual void name() { cout << "class Y" << endl; };
	virtual void age(int n) { cout << "age=" << to_string(n + 10) << endl; };
	virtual string location(const char* state, const char* city) { cout << "unknown" << endl; return "UNKNOWN"; };
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
	P1CallbackList<ICallbacks, int> callbacks;
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

	try {
		cout << "---- Callback<I, F> ----" << endl;
		P1Callback<ICallbacks, int> callback;
		callback.bind(&x, &ICallbacks::age, "void ICallback::age(int)");
		callback(20);
		callback.unbind();
		callback(120);
	} catch( std::exception& e) {
		cout << e << endl;
	}

	try {
		P2Callback<ICallbacks, const char*, const char*, string> locationCallback;
		locationCallback.bind(&x, &ICallbacks::location, "LocationCallback(state, city)");
		cout << "locationCallback returns: " << locationCallback("tokyo", "shibuya") << endl;
		locationCallback.unbind();
		locationCallback("", NULL);
	} catch( std::exception& e) {
		cout << e << endl;
	}

	//void (ICallbacks::*pName)() = &ICallbacks::name;
	//(x.*pName)();
	//(y.*pName)();

	//ICallbacks* c = &x;

	//c->name();
	//c = &y;
	//c->name();

	cout << "---- MyException::operator<< ----" << endl;
	try {
		throw MyException("xxx");
	} catch(MyException& e) {
		cout << "test: " << e << endl;
	}
	return 0;
}
