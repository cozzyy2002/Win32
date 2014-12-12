#pragma once

#include "ProcessControl.h"

class CMasterProcess : public CProcessControl
{
public:
	class IListener {
	public:
		virtual void onResponse(LPCTSTR res) = 0;
		virtual void onNotify(LPCTSTR notiry) = 0;
	};

	CMasterProcess(IListener* listener);
	virtual ~CMasterProcess();

	bool startSlaveProcess(LPCTSTR fileName);
	bool sendCommand(LPCTSTR cmd);

protected:
	virtual void onReceiveMessage(LPVOID msg, size_t size);
};
