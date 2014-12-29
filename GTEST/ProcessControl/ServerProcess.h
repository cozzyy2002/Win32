#pragma once

#include "ProcessControl.h"

class CServerProcess : public CProcessControl
{
public:
	class IListener {
	public:
		virtual void onConnect() = 0;
		virtual void onResponse(LPCTSTR res) = 0;
		virtual void onNotify(LPCTSTR notiry) = 0;
	};

	CServerProcess(IListener* listener);
	virtual ~CServerProcess();

	bool connect(LPCTSTR fileName);
	bool sendCommand(LPCTSTR cmd);

protected:
	virtual void onReceiveMessage(LPVOID msg, size_t size);
};
