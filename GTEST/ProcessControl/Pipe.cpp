#include "stdafx.h"
#include "Pipe.h"

#include <exception>

static log4cplus::Logger logger = log4cplus::Logger::getInstance(_T("ProcessControl.Pipe"));
static const TCHAR pipeName[] = _T("\\\\.\\pipe\\pipename");

CPipe::CPipe()
{
	m_hReadyToWriteEvent = ::CreateEvent(NULL, TRUE, TRUE, NULL);	// will be reset before pipe operation
	m_hReadCompletedEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
}

CPipe::~CPipe()
{
}

void CPipe::create(DWORD defaultTimeout /*= 0*/, DWORD inBufferSize /*= 128*/, DWORD outBufferSize /*= 128*/)
{
	m_hPipe = ::CreateNamedPipe(pipeName,
								PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
								PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_NOWAIT,
								1, inBufferSize, outBufferSize, defaultTimeout, NULL);
	if(!m_hPipe.isValid()) {
		LOG4CPLUS_ERROR(logger, "CreateNamedPipe() failed. error=" << ::GetLastError());
		throw std::exception(__FUNCTION__ ": CreateNamedPipe() failed.");
	}
}

void CPipe::connect()
{
	if(m_hPipe.isValid()) {
		// wait for client process to connect.
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
		if(!m_hPipe.isValid()) {
			LOG4CPLUS_ERROR(logger, "CreateFile() failed. error=" << ::GetLastError());
			throw std::exception(__FUNCTION__ ": CreateFile() failed.");
		}
	}
	DWORD flags;
	if(!::GetNamedPipeInfo(m_hPipe, &flags, &m_OutBufferSize, &m_InBufferSize, NULL)) {
		LOG4CPLUS_ERROR(logger, "GetNamedPipeInfo() failed. error=" << ::GetLastError());
		throw std::exception(__FUNCTION__ ": GetNamedPipeInfo() failed.");
	}
	LOG4CPLUS_INFO(logger, ((flags & PIPE_SERVER_END) ? "server" : "client") << ": OutBufferSize=" << m_OutBufferSize << ",InBufferSize=" << m_InBufferSize);
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
	DWORD numberOfBytesWritten, total = 0;
	Overlapped ov(m_hReadyToWriteEvent);
	for(DWORD _size = size; 0 < _size; _size -= size) {
		size = min(m_OutBufferSize, _size);
		waitEvent(m_hReadyToWriteEvent, timeout);
		BOOL result = ::WriteFile(m_hPipe, data, size, &numberOfBytesWritten, &ov);
		if(!result) {
			LOG4CPLUS_ERROR(logger, "WiteFile() failed. error=" << ::GetLastError() << ",size=" << size);
			throw std::exception(__FUNCTION__ ": WiteFile() failed.");
		}

		// flash buffers if there are data to be written
		if(m_OutBufferSize < _size) {
			result = ::FlushFileBuffers(m_hPipe);
			if(!result) {
				LOG4CPLUS_ERROR(logger, "FlushFileBuffers() failed. error=" << ::GetLastError() << ",pipe=" << m_hPipe);
				throw std::exception(__FUNCTION__ ": WiteFile() failed.");
			}
		}
		data = (LPCBYTE)data + size;
		total += size;
	}
	if(0 < timeout) {
		numberOfBytesWritten = getOverlappedResult(&ov, timeout);
	}
	LOG4CPLUS_TRACE(logger, "wrote " << total << "byte");
	return total;
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
	DWORD numberOfBytesRead, total = 0;
	Overlapped ov(m_hReadCompletedEvent);
	for(DWORD _size = size; 0 < _size; _size -= size) {
		size = min(m_InBufferSize, _size);
		BOOL result = ::ReadFile(m_hPipe, data, size, &numberOfBytesRead, &ov);
		if(!result) {
			DWORD error = ::GetLastError();
			if(error != ERROR_IO_PENDING) {
				LOG4CPLUS_ERROR(logger, "ReadFile() failed. error=" << error);
				throw std::exception(__FUNCTION__ ": ReadFile() failed.");
			}
		}
		waitEvent(m_hReadCompletedEvent, timeout);
		data = (LPBYTE)data + size;
		total += size;
	}
	if(0 < timeout) {
		numberOfBytesRead = getOverlappedResult(&ov, timeout);
	}
	LOG4CPLUS_TRACE(logger, "read " << total << "byte");
	return total;
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
		LOG4CPLUS_ERROR(logger, "waiting event " << hEvent << " failed. wait=" << wait << ",error=" << ::GetLastError());
		throw std::exception(__FUNCTION__ ": waiting event failed.");
	}
}
