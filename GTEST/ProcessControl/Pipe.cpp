#include "stdafx.h"
#include "Pipe.h"

#include <exception>

static log4cplus::Logger logger = log4cplus::Logger::getInstance(_T("ProcessControl.Pipe"));
static const TCHAR pipeName[] = _T("\\\\.\\pipe\\pipename");

CPipe::CPipe() : m_hPipe(NULL)
{
	m_hReadyToWriteEvent = ::CreateEvent(NULL, TRUE, TRUE, NULL);	// will be reset before pipe operation
	m_hReadCompletedEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
}

CPipe::~CPipe()
{
	if(m_hPipe) ::CloseHandle(m_hPipe);
	::CloseHandle(m_hReadyToWriteEvent);
	::CloseHandle(m_hReadCompletedEvent);
}

void CPipe::create()
{
	m_hPipe = ::CreateNamedPipe(pipeName,
								PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
								PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_NOWAIT,
								1, 1024, 1024, NMPWAIT_USE_DEFAULT_WAIT, NULL);
	if(m_hPipe == INVALID_HANDLE_VALUE) {
		LOG4CPLUS_ERROR(logger, "CreateNamedPipe() failed. error=" << ::GetLastError());
		throw std::exception(__FUNCTION__ ": CreateNamedPipe() failed.");
	}
}

void CPipe::connect()
{
	if(m_hPipe) {
		// wait to connect for client process.
		Overlapped ov(NULL);
		BOOL result = ::ConnectNamedPipe(m_hPipe, &ov);
		if(!result) {
			DWORD error = ::GetLastError();
			switch(error) {
			case ERROR_IO_PENDING:
			case ERROR_PIPE_CONNECTED:
			case ERROR_PIPE_LISTENING:
				break;
			default:
				LOG4CPLUS_ERROR(logger, "ConnectNamedPipe() failed. error=" << error);
				throw std::exception(__FUNCTION__ ": ConnectNamedPipe() failed.");
			}
		}
	} else {
		// connect to the pipe created by server process
		m_hPipe = ::CreateFile(pipeName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
		if(m_hPipe == INVALID_HANDLE_VALUE) {
			m_hPipe = NULL;
			LOG4CPLUS_ERROR(logger, "CreateFile() failed. error=" << ::GetLastError());
			throw std::exception(__FUNCTION__ ": CreateFile() failed.");
		}
	}
}

void CPipe::send(const std::string& str, DWORD timeout /*= 0*/)
{
	LOG4CPLUS_DEBUG(logger, "sending message '" << str.c_str() << "'");
	size_t size = str.size();
	writePipe(&size, sizeof(size), timeout);
	writePipe(str.c_str(), size, timeout);
}

DWORD CPipe::writePipe(LPCVOID data, size_t size, DWORD timeout /*= 0*/)
{
	LOG4CPLUS_TRACE(logger, "writing " << size << "byte");
	DWORD numberOfBytesWritten;
	waitEvent(m_hReadyToWriteEvent, timeout);
	Overlapped ov(m_hReadyToWriteEvent);
	BOOL result = ::WriteFile(m_hPipe, data, size, &numberOfBytesWritten, &ov);
	if(!result) {
		LOG4CPLUS_ERROR(logger, "WiteFile() failed. error=" << ::GetLastError() << ",size=" << size);
		throw std::exception(__FUNCTION__ ": WiteFile() failed.");
	}
	if(0 < timeout) {
		numberOfBytesWritten = getOverlappedResult(&ov, timeout);
	}
	LOG4CPLUS_TRACE(logger, "wrote " << numberOfBytesWritten << "byte");
	return numberOfBytesWritten;
}

std::string CPipe::receive(DWORD timeout /*= INFINITE*/)
{
	std::string ret;
	size_t size;
	readPipe(&size, sizeof(size), timeout);
	AutoArray<CHAR> str(size);
	readPipe(str, size, timeout);
	ret.assign(str, size);
	LOG4CPLUS_DEBUG(logger, "received message '" << ret.c_str() << "'");
	return ret;
}

DWORD CPipe::readPipe(LPVOID data, size_t size, DWORD timeout)
{
	LOG4CPLUS_TRACE(logger, "reading " << size << "byte");
	DWORD numberOfBytesRead;
	Overlapped ov(m_hReadCompletedEvent);
	BOOL result = ::ReadFile(m_hPipe, data, size, &numberOfBytesRead, &ov);
	if(!result) {
		LOG4CPLUS_ERROR(logger, "ReadFile() failed. error=" << ::GetLastError());
		throw std::exception(__FUNCTION__ ": ReadFile() failed.");
	}
	if(0 < timeout) {
		numberOfBytesRead = getOverlappedResult(&ov, timeout);
	}
	LOG4CPLUS_TRACE(logger, "read " << numberOfBytesRead << "byte");
	return numberOfBytesRead;
}

DWORD CPipe::getOverlappedResult(OVERLAPPED* pov, DWORD timeout /*= INFINITE*/)
{
	LOG4CPLUS_TRACE(logger, "waiting overlapped " << pov->hEvent << "," << timeout << "mSec");
	waitEvent(pov->hEvent, timeout);
	DWORD numberOfBytesTransferred;
	BOOL result = ::GetOverlappedResult(m_hPipe, pov, &numberOfBytesTransferred, FALSE);
	if(!result) {
		LOG4CPLUS_ERROR(logger, "waiting event " << pov->hEvent << " failed. error=" << ::GetLastError());
		throw std::exception(__FUNCTION__ ": waiting event failed.");
	}
	return numberOfBytesTransferred;
}

void CPipe::waitEvent(HANDLE hEvent, DWORD timeout /*= INFINITE*/)
{
	LOG4CPLUS_TRACE(logger, "waiting event " << hEvent << "," << timeout << "mSec");
	DWORD wait = ::WaitForSingleObject(hEvent, timeout);
	if(wait != WAIT_OBJECT_0) {
		LOG4CPLUS_ERROR(logger, "waiting event " << hEvent << " failed. error=" << ::GetLastError());
		throw std::exception(__FUNCTION__ ": waiting event failed.");
	}
}
