#include "stdafx.h"
#include "ProcessControl.h"
#include "Dispatcher.h"
#include <memory>

const CProcessControl::handleEvent_t CProcessControl::eventStateTable[StateMax][EventMax] = {
	{},
};

CProcessControl::CProcessControl()
{
	m_terminateEvent = NULL;
}

CProcessControl::~CProcessControl()
{
	if (m_terminateEvent) {
		::SetEvent(m_terminateEvent);
		::CloseHandle(m_terminateEvent);
	}
}

bool CProcessControl::start()
{
	// Already started?
	if (m_terminateEvent) return false;

	m_handleEventThread = dispatch(this, &CProcessControl::handleEventThread);

	m_terminateEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);

	return (m_terminateEvent && m_receiveMessageThread);
}

void CProcessControl::handleEventThread()
{
	MSG msg;
	while(::GetMessage(&msg, NULL, 0, 0)) {
		// TODO: exclusive control
		std::shared_ptr<EventData> eventData((EventData*)msg.lParam);
		if((m_state < 0) || (StateMax <= m_state) ||
			(eventData->event < 0) || (EventMax < eventData->event)) {
			// state or event is out of range
			break;
		}
		handleEvent_t handleEvent = eventStateTable[m_state][eventData->event];
		if(handleEvent == NULL) continue;

		m_state = (this->*handleEvent)(*eventData);
	}
}
