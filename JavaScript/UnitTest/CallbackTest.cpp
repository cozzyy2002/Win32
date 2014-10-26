#include "stdafx.h"
#include <gtest/gtest.h>

#include <helpers/Callback.h>

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

	P0Callback<Callee> callback;

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

	P1Callback<Callee, int, int> callback;

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
