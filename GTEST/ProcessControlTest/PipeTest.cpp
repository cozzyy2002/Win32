#include "stdafx.h"

#include "Pipe.h"

static log4cplus::Logger logger = log4cplus::Logger::getInstance(_T("ProcessControl.PipeTest"));

using namespace testing;

class Testee : public CPipe {
public:
	using CPipe::writePipe;
	using CPipe::readPipe;
};

class PipeTest : public testing::Test {
public:
	Testee master;
	Testee slave;
};

class PipeNormalTest : public PipeTest {
public:
	void SetUp()
	{
		ASSERT_NO_THROW(master.create());
		ASSERT_NO_THROW(master.connect());
		ASSERT_NO_THROW(slave.connect());
	};
	void TearDown() {};
};

TEST_F(PipeNormalTest, MasterSends)
{
	const char* msg = "message from master";
	ASSERT_NO_THROW(master.send(msg));
	EXPECT_STRCASEEQ(msg, slave.receive().c_str());
}

TEST_F(PipeNormalTest, MasterSends2)
{
	char* msg = "1st message from master";
	ASSERT_NO_THROW(master.send(msg));
	EXPECT_STRCASEEQ(msg, slave.receive().c_str());

	msg = "2nd message from master";
	ASSERT_NO_THROW(master.send(msg));
	EXPECT_STRCASEEQ(msg, slave.receive().c_str());
}

TEST_F(PipeNormalTest, SlaveSends)
{
	const char* msg = "message from slave";
	ASSERT_NO_THROW(slave.send(msg));
	EXPECT_STRCASEEQ(msg, master.receive().c_str());
}

TEST_F(PipeNormalTest, BothSends)
{
	char* msg = "message from master";
	ASSERT_NO_THROW(master.send(msg));
	EXPECT_STRCASEEQ(msg, slave.receive().c_str());

	msg = "message from slave";
	ASSERT_NO_THROW(slave.send(msg));
	EXPECT_STRCASEEQ(msg, master.receive().c_str());
}

TEST_F(PipeTest, connect_PipeNotCreated)
{
	ASSERT_THROW(slave.connect(), std::exception);
}

class PipeReadWriteTest
	: public PipeNormalTest
	, public WithParamInterface<size_t>	// size of data to write/read
{
	void SetUp()
	{
		PipeNormalTest::SetUp();
	};
	void TearDown() {
		PipeNormalTest::TearDown();
	};
};

TEST_P(PipeReadWriteTest, Normal)
{
	size_t size = GetParam();

	AutoArray<BYTE> writeBuff(size);
	AutoArray<BYTE> readBuff(size);
	for(size_t i = 0; i < size; i++) {
		writeBuff[i] = 10 - i;
		readBuff[i] = 0;
	}

	ASSERT_NO_THROW(master.writePipe(writeBuff, size, 0));
	ASSERT_NO_THROW(slave.readPipe(readBuff, size, 1000));
	for(size_t i = 0; i < size; i++) {
		ASSERT_EQ(writeBuff[i], readBuff[i]) << "data position=" << i;
	}
}

INSTANTIATE_TEST_CASE_P(Internal, PipeReadWriteTest, Values(1, 10, 100, 1024, 1025));
