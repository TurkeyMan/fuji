#if !defined(_STATUSWINDOW_H)
#define _STATUSWINDOW_H

#include "Global.h"

class StatusCommand {
public:	
	typedef enum {COMMAND_INIT, COMMAND_LOGMESSAGE, COMMAND_CLEARMESSAGES, COMMAND_CLOSE, COMMAND_WAITFORCLOSE} CommandType;

	virtual ~StatusCommand() {};

	CommandType type;
};

class CommandInit : public StatusCommand {
public:
	CommandInit() { type = COMMAND_INIT; }
	~CommandInit() {}
};

class CommandLogMessage : public StatusCommand {
public:	
	CommandLogMessage() { type = COMMAND_LOGMESSAGE; }
	CommandLogMessage(TCHAR *_message) { type = COMMAND_LOGMESSAGE; message = _message; }
	~CommandLogMessage() {}

	TCHAR *message;
};

class CommandClearMessages : public StatusCommand {
public:	
	CommandClearMessages() { type = COMMAND_CLEARMESSAGES; }
	~CommandClearMessages() {}
};

class CommandClose : public StatusCommand {
public:	
	CommandClose() { type = COMMAND_CLOSE; }
	~CommandClose() {}
};

class CommandWaitForClose : public StatusCommand {
public:
	CommandWaitForClose() { type = COMMAND_WAITFORCLOSE; }
	~CommandWaitForClose() {}
};


class StatusWindow {
public:	
	StatusWindow(HWND _parentWindow);
	~StatusWindow();
	void Print(const TCHAR *format, ...);
	void Close();
	void WaitForClose();

private:
	HWND parentWindow;
	
	void SendCommand(StatusCommand *_command);

	static DWORD WINAPI ThreadProc(LPVOID lpParameter);
	static INT_PTR CALLBACK MainDialogProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void CleanupThread();
	bool MessagePump();

	CRITICAL_SECTION csReentrant, csCommand, csThreadAlive;
	HANDLE otherThread, eventCommandSent, eventCommandAcknowledged;

	bool threadAlive, hasFailed;
	StatusCommand *command;
	HWND dialog;
};

#endif