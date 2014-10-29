#include "stdafx.h"
#include <gtest/gtest.h>

#include <helpers/Callback.h>

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

static log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("Test.CallbackTest"));

class Callee {
public:
	Callee() : calledMethod(NULL) {};

	void methodP0() {
		calledMethod = "methodP0";
	};
	int methodP1(int i) {
		calledMethod = "methodP1";
		return i;
	};

	char* calledMethod;
};

class CallbackTest : public ::testing::Test {
public:
	Callee testCallback;
};

TEST_F(CallbackTest, method_with_no_param) {

	Callback<Callee, void (Callee::*)()>::P0<> callback;

	ASSERT_FALSE(callback) << "before bind";
	callback.bind(&testCallback, &Callee::methodP0);
	ASSERT_TRUE(callback) << "after bind";

	ASSERT_NO_THROW(callback()) << "calling the method";
	EXPECT_STREQ("methodP0", testCallback.calledMethod) << "called method";

	callback.unbind();
	ASSERT_FALSE(callback) << "after unbind";
	ASSERT_THROW(callback(), std::exception) << "unbound";
}

TEST_F(CallbackTest, method_with_1_param) {

	Callback<Callee, int (Callee::*)(int)>::P1<int, int> callback;

	ASSERT_FALSE(callback) << "before bind";
	callback.bind(&testCallback, &Callee::methodP1);
	ASSERT_TRUE(callback) << "after bind";

	int param = 10, ret;
	ASSERT_NO_THROW(ret = callback(param)) << "calling the method";
	EXPECT_STREQ("methodP1", testCallback.calledMethod) << "called method";
	EXPECT_EQ(param, ret) << "return value";

	callback.unbind();
	ASSERT_FALSE(callback) << "after unbind";
	ASSERT_THROW(callback(param), std::exception) << "unbound";
}

#include <helpers/Dispatcher.h>
