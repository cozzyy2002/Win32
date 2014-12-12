#pragma once
class CProcessControl
{
protected:
	enum State {
		InitialState,
		ConnectingState,
		ConnectedState,
		SendingMessageState,
		StateMax
	};
	enum Event {
		ConnectedEvent,
		SendMessageEvent,
		RecivedMessageEvent,
		CancelEvent,
		EventMax
	};

	typedef struct _EventData {
		Event event;
	} EventData;
	CProcessControl();
	virtual ~CProcessControl();

	typedef State (CProcessControl::*handleEvent_t)(const EventData& eventData);
	static const handleEvent_t eventStateTable[StateMax][EventMax];

	bool start();
	void sendMessage(LPVOID msg, size_t size);
	virtual void onReceiveMessage(LPVOID msg, size_t size) = 0;
	void handleEventThread();
	void receiveMessageThread();

	State m_state;
	HANDLE m_handleEventThread;		// will be returnd and closed by CDispatcher.
	HANDLE m_receiveMessageThread;	// will be returnd and closed by CDispatcher.
	HANDLE m_terminateEvent;
};
