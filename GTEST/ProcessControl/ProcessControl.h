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
		TerminateEvent,
		EventMax
	};

	typedef struct _EventData {
		Event event;
	} EventData;
	CProcessControl();
	virtual ~CProcessControl();

	typedef State (CProcessControl::*EventHandler)(const EventData& eventData);
	static const EventHandler eventStateTable[StateMax][EventMax];

	bool start();
	void sendMessage(LPVOID msg, size_t size);
	virtual void onReceiveMessage(LPVOID msg, size_t size) = 0;
	void handleEventThread();
	void receiveMessageThread();

	void postEvent(EventData* pEventData);
	State m_state;
	DWORD m_handleEventThreadId;
	HANDLE m_handleEventThread;		// will be returnd and closed by CDispatcher.
	HANDLE m_receiveMessageThread;	// will be returnd and closed by CDispatcher.
};
