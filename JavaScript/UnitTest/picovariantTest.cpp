#include "stdafx.h"
#include <gtest/gtest.h>

#define _X86_
#include "npPlugin/picovariant.h"

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

static log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("Test.pcovariantTest"));

class NPFunctions {
public:
	void init() {
		identifier = (NPIdentifier)1;
		VOID_TO_NPVARIANT(variant);
		hasProperty = true;
		getProperty = true;
		released = false;
	};

	NPIdentifier NPN_GetStringIdentifier(const NPUTF8 *name) { return identifier; };
	bool NPN_HasProperty(NPP npp, NPObject *npobj, NPIdentifier propertyName) { return hasProperty; };
	bool NPN_GetProperty(NPP npp, NPObject *npobj, NPIdentifier propertyName, NPVariant *result) {
		*result = variant;
		return getProperty;
	};
	void NPN_ReleaseVariantValue(NPVariant *variant) { released = true; };

	NPIdentifier identifier;
	NPVariant variant;
	bool hasProperty;
	bool getProperty;
	bool released;
};

static NPFunctions npFunctions;
NPIdentifier NPN_GetStringIdentifier(const NPUTF8 *name) { return npFunctions.NPN_GetStringIdentifier(name); };
bool NPN_HasProperty(NPP npp, NPObject *npobj, NPIdentifier propertyName) { return npFunctions.NPN_HasProperty(npp, npobj, propertyName); };
bool NPN_GetProperty(NPP npp, NPObject *npobj, NPIdentifier propertyName,
					 NPVariant *result) { return npFunctions.NPN_GetProperty(npp, npobj, propertyName, result); };
void NPN_ReleaseVariantValue(NPVariant *variant) { npFunctions.NPN_ReleaseVariantValue(variant); };

class picovariantTest : public testing::Test {
public:
	void SetUp() {
		npFunctions.init();
	};
};

TEST_F(picovariantTest, Void) {
	picovariant::value v;

	EXPECT_STREQ("Void", v.getTypeName()) << "value::getTypeName()";
	EXPECT_TRUE(v.is<void>()) << "value::is<void>()";
	EXPECT_FALSE(v.is<picovariant::null>()) << "value::is<null>()";
	EXPECT_FALSE(v.is<bool>()) << "value::is<bool>()";
	EXPECT_FALSE(v.is<int>()) << "value::is<int>()";
	EXPECT_FALSE(v.is<std::string>()) << "value::is<string>()";
	EXPECT_FALSE(v.is<NPObject*>()) << "value::is<NPObject*>()";

	EXPECT_FALSE(v.isNumber()) << "value::isNumber()";
	EXPECT_FALSE(v.isNullString()) << "value::isNullString()";
	EXPECT_FALSE(v.isEmptyString()) << "value::isEmptyString()";
	EXPECT_FALSE(v.isNullObject()) << "value::isNullObject()";
}

TEST_F(picovariantTest, Null) {
	NPVariant var;
	NULL_TO_NPVARIANT(var);
	picovariant::value v(var);

	EXPECT_STREQ("Null", v.getTypeName()) << "value::getTypeName()";
	EXPECT_FALSE(v.is<void>()) << "value::is<void>()";
	EXPECT_TRUE(v.is<picovariant::null>()) << "value::is<null>()";
	EXPECT_FALSE(v.is<bool>()) << "value::is<bool>()";
	EXPECT_FALSE(v.is<int>()) << "value::is<int>()";
	EXPECT_FALSE(v.is<std::string>()) << "value::is<string>()";
	EXPECT_FALSE(v.is<NPObject*>()) << "value::is<NPObject*>()";

	EXPECT_FALSE(v.isNumber()) << "value::isNumber()";
	EXPECT_FALSE(v.isNullString()) << "value::isNullString()";
	EXPECT_FALSE(v.isEmptyString()) << "value::isEmptyString()";
	EXPECT_FALSE(v.isNullObject()) << "value::isNullObject()";
}

TEST_F(picovariantTest, Boolean) {
	picovariant::value v(true);

	EXPECT_STREQ("Bool", v.getTypeName()) << "value::getTypeName()";
	EXPECT_FALSE(v.is<void>()) << "value::is<void>()";
	EXPECT_FALSE(v.is<picovariant::null>()) << "value::is<null>()";
	EXPECT_TRUE(v.is<bool>()) << "value::is<bool>()";
	EXPECT_FALSE(v.is<int>()) << "value::is<int>()";
	EXPECT_FALSE(v.is<std::string>()) << "value::is<string>()";
	EXPECT_FALSE(v.is<NPObject*>()) << "value::is<NPObject*>()";

	EXPECT_FALSE(v.isNumber()) << "value::isNumber()";
	EXPECT_FALSE(v.isNullString()) << "value::isNullString()";
	EXPECT_FALSE(v.isEmptyString()) << "value::isEmptyString()";
	EXPECT_FALSE(v.isNullObject()) << "value::isNullObject()";

	EXPECT_EQ(true, v.get<bool>()) << "get()";
}

TEST_F(picovariantTest, String) {
	picovariant::value v("abc");

	EXPECT_STREQ("String", v.getTypeName()) << "value::getTypeName()";
	EXPECT_FALSE(v.is<void>()) << "value::is<void>()";
	EXPECT_FALSE(v.is<picovariant::null>()) << "value::is<null>()";
	EXPECT_FALSE(v.is<bool>()) << "value::is<bool>()";
	EXPECT_FALSE(v.is<int>()) << "value::is<int>()";
	EXPECT_TRUE(v.is<std::string>()) << "value::is<string>()";
	EXPECT_FALSE(v.is<NPObject*>()) << "value::is<NPObject*>()";

	EXPECT_FALSE(v.isNumber()) << "value::isNumber()";
	EXPECT_FALSE(v.isNullString()) << "value::isNullString()";
	EXPECT_FALSE(v.isEmptyString()) << "value::isEmptyString()";
	EXPECT_FALSE(v.isNullObject()) << "value::isNullObject()";

	EXPECT_EQ(std::string("abc"), v.get<std::string>()) << "get()";
}

TEST_F(picovariantTest, String_empty) {
	picovariant::value v("");

	EXPECT_STREQ("String", v.getTypeName()) << "value::getTypeName()";
	EXPECT_FALSE(v.is<void>()) << "value::is<void>()";
	EXPECT_FALSE(v.is<picovariant::null>()) << "value::is<null>()";
	EXPECT_FALSE(v.is<bool>()) << "value::is<bool>()";
	EXPECT_FALSE(v.is<int>()) << "value::is<int>()";
	EXPECT_TRUE(v.is<std::string>()) << "value::is<string>()";
	EXPECT_FALSE(v.is<NPObject*>()) << "value::is<NPObject*>()";

	EXPECT_FALSE(v.isNumber()) << "value::isNumber()";
	EXPECT_FALSE(v.isNullString()) << "value::isNullString()";
	EXPECT_TRUE(v.isEmptyString()) << "value::isEmptyString()";
	EXPECT_FALSE(v.isNullObject()) << "value::isNullObject()";

	EXPECT_EQ(std::string(""), v.get<std::string>()) << "get()";
}

TEST_F(picovariantTest, String_null) {
	picovariant::value v((LPCSTR)NULL, 10);

	EXPECT_STREQ("String", v.getTypeName()) << "value::getTypeName()";
	EXPECT_FALSE(v.is<void>()) << "value::is<void>()";
	EXPECT_FALSE(v.is<picovariant::null>()) << "value::is<null>()";
	EXPECT_FALSE(v.is<bool>()) << "value::is<bool>()";
	EXPECT_FALSE(v.is<int>()) << "value::is<int>()";
	EXPECT_TRUE(v.is<std::string>()) << "value::is<string>()";
	EXPECT_FALSE(v.is<NPObject*>()) << "value::is<NPObject*>()";

	EXPECT_FALSE(v.isNumber()) << "value::isNumber()";
	EXPECT_TRUE(v.isNullString()) << "value::isNullString()";
	EXPECT_FALSE(v.isEmptyString()) << "value::isEmptyString()";
	EXPECT_FALSE(v.isNullObject()) << "value::isNullObject()";
}
