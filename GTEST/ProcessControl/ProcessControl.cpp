#include "stdafx.h"
#include "ProcessControl.h"
#include "Dispatcher.h"
#include <memory>

const CProcessControl::EventHandler CProcessControl::eventStateTable[StateMax][EventMax] = {
	{},
};

CProcessControl::CProcessControl()
{
	m_handleEventThread = NULL;
}

CProcessControl::~CProcessControl()
{
	if (m_handleEventThread) {
		EventData* pEventData = new EventData();
		pEventData->event = TerminateEvent;
		postEvent(pEventData);
	}
}

bool CProcessControl::start()
{
	// Already started?
	if (m_handleEventThread) return false;

	m_handleEventThread = dispatch(this, &CProcessControl::handleEventThread, &m_handleEventThreadId);

	return (m_receiveMessageThread != NULL);
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
		EventHandler eventHandler = eventStateTable[m_state][eventData->event];
		if(eventHandler == NULL) continue;

		m_state = (this->*eventHandler)(*eventData);
	}
}
