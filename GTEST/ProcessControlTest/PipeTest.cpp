#include "stdafx.h"

#include "Pipe.h"
#include <process.h>

static log4cplus::Logger logger = log4cplus::Logger::getInstance(_T("ProcessControl.PipeTest"));

using namespace testing;

class Testee : public CPipe {
public:
	using CPipe::writePipe;
	using CPipe::readPipe;
};

class PipeTest : public testing::Test {
public:
	Testee server;
	Testee client;
};

class PipeNormalTest : public PipeTest {
public:
	void SetUp()
	{
		ASSERT_NO_THROW(server.create());
		ASSERT_NO_THROW(server.connect());
		ASSERT_NO_THROW(client.connect());
	};
	void TearDown() {};
};

TEST_F(PipeNormalTest, serverSends)
{
	const char* msg = "message from server";
	ASSERT_NO_THROW(server.send(msg));
	EXPECT_STRCASEEQ(msg, client.receive().c_str());
}

TEST_F(PipeNormalTest, serverSends2)
{
	char* msg = "1st message from server";
	ASSERT_NO_THROW(server.send(msg));
	EXPECT_STRCASEEQ(msg, client.receive().c_str());

	msg = "2nd message from server";
	ASSERT_NO_THROW(server.send(msg));
	EXPECT_STRCASEEQ(msg, client.receive().c_str());
}

TEST_F(PipeNormalTest, clientSends)
{
	const char* msg = "message from client";
	ASSERT_NO_THROW(client.send(msg));
	EXPECT_STRCASEEQ(msg, server.receive().c_str());
}

TEST_F(PipeNormalTest, BothSends)
{
	char* msg = "message from server";
	ASSERT_NO_THROW(server.send(msg));
	EXPECT_STRCASEEQ(msg, client.receive().c_str());

	msg = "message from client";
	ASSERT_NO_THROW(client.send(msg));
	EXPECT_STRCASEEQ(msg, server.receive().c_str());
}

TEST_F(PipeTest, connect_PipeNotCreated)
{
	ASSERT_THROW(client.connect(), std::exception);
}

class PipeReadWriteTest
	: public PipeNormalTest
	, public WithParamInterface<size_t>	// size of data to write/read
{
public:
	void SetUp()
	{
		PipeNormalTest::SetUp();
	};
	void TearDown() {
		PipeNormalTest::TearDown();
	};

	struct _Data {
		BYTE* pBuff;
		size_t size;
		Testee& server;
		Testee&client;
	};
	static unsigned int __stdcall writeThread(void* p)
	{
		_Data* pData = (_Data*)p;
		try {
			pData->server.writePipe(pData->pBuff, pData->size, 200);
		} catch(std::exception& e) {
			LOG4CPLUS_ERROR(logger, __FUNCTION__ " exception: " << e.what());
		}
		return 0;
	};
	static unsigned int __stdcall readThread(void* p)
	{
		_Data* pData = (_Data*)p;
		try {
			pData->client.readPipe(pData->pBuff, pData->size, 2000);
		} catch(std::exception& e) {
			LOG4CPLUS_ERROR(logger, __FUNCTION__ " exception: " << e.what());
		}
		return 0;
	}
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
	_Data data = {readBuff, size, server, client};
	//_beginthreadex(NULL, 0, readThread, &data, 0, NULL);
	//_Data data = {writeBuff, size, server, client};
	//_beginthreadex(NULL, 0, writeThread, &data, 0, NULL);
	::Sleep(1000);
	HANDLE hThread = ::CreateThread(NULL, 0,
		[](LPVOID p)->DWORD
			{
				_Data* pData = (_Data*)p;
				try {
					pData->client.readPipe(pData->pBuff, pData->size, 2000);
				} catch(std::exception& e) {
					LOG4CPLUS_ERROR(logger, __FUNCTION__ " exception: " << e.what());
					return 1;
				}
				return 0;
			},
		&data, 0, NULL);
	ASSERT_NO_THROW(server.writePipe(writeBuff, size, 1500));
	::WaitForSingleObject(hThread, INFINITE);
	DWORD exitCode;
	EXPECT_EQ(TRUE, ::GetExitCodeThread(hThread, &exitCode)) << "GetExitCode() failed. error=" << ::GetLastError();
	EXPECT_EQ(0, exitCode);
	::CloseHandle(hThread);
	//ASSERT_NO_THROW(client.readPipe(readBuff, size, 1000));
	for(size_t i = 0; i < size; i++) {
		ASSERT_EQ(writeBuff[i], readBuff[i]) << "data position=" << i;
	}
}

INSTANTIATE_TEST_CASE_P(Internal, PipeReadWriteTest, Values(1, 10, 100, 1024, 2000));
