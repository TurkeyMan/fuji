#include "Global.h"
#include "StatusWindow.h"
#include <strsafe.h>

StatusWindow::StatusWindow(HWND _parentWindow)
{
	parentWindow = _parentWindow;

	InitializeCriticalSection(&csReentrant);
	InitializeCriticalSection(&csCommand);
	InitializeCriticalSection(&csThreadAlive);

	otherThread = NULL;
	threadAlive = false;
	hasFailed = false;
	eventCommandSent = CreateEvent(NULL, FALSE, FALSE, NULL);
	eventCommandAcknowledged = CreateEvent(NULL, FALSE, FALSE, NULL);
}

StatusWindow::~StatusWindow()
{
	EnterCriticalSection(&csThreadAlive);
	if(threadAlive) {
		CommandClose cmdClose;
		
		SendCommand(&cmdClose);
	} else {
		if(otherThread != NULL) {
			CloseHandle(otherThread);
			otherThread = NULL;
		}
	}
	LeaveCriticalSection(&csThreadAlive);

	CloseHandle(eventCommandAcknowledged);
	CloseHandle(eventCommandSent);

	DeleteCriticalSection(&csThreadAlive);
	DeleteCriticalSection(&csCommand);	
	DeleteCriticalSection(&csReentrant);
}

void StatusWindow::Print(const TCHAR *format, ...)
{
	va_list args;
	static char buffer[4096];
	static CommandLogMessage cmdLogMessage;

	va_start(args, format);
    HRESULT pfok = StringCbVPrintf(buffer, 4096, format, args);
	va_end(args);
	
	if(pfok == S_OK || pfok == STRSAFE_E_INSUFFICIENT_BUFFER) {
		cmdLogMessage.message = buffer;
		SendCommand(&cmdLogMessage);
	}
}

void StatusWindow::Close()
{
	EnterCriticalSection(&csReentrant);
	if(hasFailed || !threadAlive) {
		LeaveCriticalSection(&csReentrant);
		return;
	}
	LeaveCriticalSection(&csReentrant);	
	
	CommandClose cmdClose;
	SendCommand(&cmdClose);
}

void StatusWindow::WaitForClose()
{
	EnterCriticalSection(&csReentrant);
	if(hasFailed || !threadAlive) {
		LeaveCriticalSection(&csReentrant);
		return;
	}
	LeaveCriticalSection(&csReentrant);
	
	CommandWaitForClose cmdWaitForClose;
	SendCommand(&cmdWaitForClose);
}

void StatusWindow::SendCommand(StatusCommand *_command)
{
	if(hasFailed) {
		return;
	}

	EnterCriticalSection(&csReentrant);
	EnterCriticalSection(&csThreadAlive);

	if(!threadAlive) {
		if(otherThread != NULL) {
			CloseHandle(otherThread);
		}

		ResetEvent(eventCommandSent);
		ResetEvent(eventCommandAcknowledged);

		otherThread = CreateThread(NULL, 0, StatusWindow::ThreadProc, (LPVOID)this, 0, NULL);
		if(otherThread == NULL) {
			hasFailed = true;
			LeaveCriticalSection(&csReentrant);
			LeaveCriticalSection(&csThreadAlive);
			return;
		}

		CommandInit cmdInit;
		command = &cmdInit;
		SetEvent(eventCommandSent);

		DWORD rv;
		rv = WaitForSingleObject(eventCommandAcknowledged, 10000);
		if(rv != WAIT_OBJECT_0) {
			MessageBox(parentWindow, "Error while waiting for new thread.\r\n\r\nThere may be a stray thread lying around now,\r\nSuggest closing 3D Studio and checking the process list.", "Error", MB_OK | MB_ICONERROR);
			TerminateThread(otherThread, 0);
			CloseHandle(otherThread);
			hasFailed = true;
			LeaveCriticalSection(&csReentrant);
			LeaveCriticalSection(&csThreadAlive);
			return;
		}

		threadAlive = true;
	}
	
	command = _command;
	if(command->type == StatusCommand::COMMAND_CLOSE)
		threadAlive = false;

	SetEvent(eventCommandSent);
	DWORD rv;
	rv = WaitForSingleObject(eventCommandAcknowledged, 10000);
	if(rv != WAIT_OBJECT_0) {
			MessageBox(parentWindow, "Error while waiting for new thread.\r\n\r\nThere may be a stray thread lying around now,\r\nSuggest closing 3D Studio and checking the process list.", "Error", MB_OK | MB_ICONERROR);
			TerminateThread(otherThread, 0);
			CloseHandle(otherThread);
			hasFailed = true;
			LeaveCriticalSection(&csReentrant);
			LeaveCriticalSection(&csThreadAlive);
			return;		
	}
	
	LeaveCriticalSection(&csThreadAlive);
	LeaveCriticalSection(&csReentrant);
}

DWORD WINAPI StatusWindow::ThreadProc(LPVOID lpParameter)
{
	HANDLE eventList[1];
	StatusWindow *me = (StatusWindow *)lpParameter;
	bool waitingForClose = false;

	eventList[0] = me->eventCommandSent;
	
	me->dialog = CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_STATUSWINDOW), NULL, StatusWindow::MainDialogProc, (LPARAM)me);
	
	MSG msg;
	while(1) {
		DWORD rv;
		rv = MsgWaitForMultipleObjects(1, eventList, FALSE, INFINITE, QS_ALLEVENTS);

		if((rv >= WAIT_OBJECT_0) && (rv < WAIT_OBJECT_0 + 1)) {
			if(rv == WAIT_OBJECT_0) {
				switch(me->command->type) {
					case StatusCommand::COMMAND_CLOSE:
						// csThreadAlive is going to be entered already by the other thread
						DestroyWindow(me->dialog);
						// Pump the remaining messages
						while(GetMessage(&msg, NULL, 0, 0) > 0);
						SetEvent(me->eventCommandAcknowledged);
						return(TRUE);


					case StatusCommand::COMMAND_LOGMESSAGE:
						int length;

						length = GetWindowTextLength(GetDlgItem(me->dialog, IDC_LOG));
						
						SendDlgItemMessage(me->dialog, IDC_LOG, EM_SETSEL, length, length);
						SendDlgItemMessage(me->dialog, IDC_LOG, EM_REPLACESEL, 0, (LPARAM)(TCHAR *)(static_cast<CommandLogMessage *>(me->command))->message);
						SendDlgItemMessage(me->dialog, IDC_LOG, WM_VSCROLL, SB_BOTTOM, (LPARAM)NULL);
						SetEvent(me->eventCommandAcknowledged);
						break;

					case StatusCommand::COMMAND_WAITFORCLOSE:
						waitingForClose = true;
						break;

					default:
						SetEvent(me->eventCommandAcknowledged);
						break;
				}
			}
		} else if(rv == WAIT_OBJECT_0 + 1) {
			while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0) {
				if(msg.message == WM_QUIT) {
					if(waitingForClose) {
						me->threadAlive = false;
						SetEvent(me->eventCommandAcknowledged);
					} else {					
						// Flushes the messages and sets threadAlive to false
						me->CleanupThread();
					}
					return(TRUE);
				}
			
				if(!IsDialogMessage(me->dialog, &msg)) {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		}
	}
}

INT_PTR CALLBACK StatusWindow::MainDialogProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	StatusWindow *me;
	me = (StatusWindow *)(LONG_PTR)GetWindowLongPtr(hWndDlg, DWLP_USER);
	
	switch(uMsg) {
		case WM_INITDIALOG:
			SetWindowLongPtr(hWndDlg, DWLP_USER, (LONG_PTR)lParam);
			me = (StatusWindow *)lParam;
			return(TRUE);
	
		case WM_COMMAND:
			if((LOWORD(wParam) == IDC_LOG) && (HIWORD(wParam) == EN_CHANGE)) {
				int lineCount;

				lineCount = (int)SendDlgItemMessage(hWndDlg, IDC_LOG, EM_GETLINECOUNT, 0, 0);
				if(lineCount > STATUS_MAX_LINES) {
					int length, total;

					length = (int)SendDlgItemMessage(hWndDlg, IDC_LOG, EM_LINELENGTH, 0, 0);

					SendDlgItemMessage(hWndDlg, IDC_LOG, EM_SETSEL, 0, length + 2);
					SendDlgItemMessage(hWndDlg, IDC_LOG, EM_REPLACESEL, 0, (LPARAM)"");

					total = GetWindowTextLength(GetDlgItem(hWndDlg, IDC_LOG));
					SendDlgItemMessage(hWndDlg, IDC_LOG, EM_SETSEL, total, total);
				}
			
				return(TRUE);
			}

			if((LOWORD(wParam) == IDC_CLOSE) && (HIWORD(wParam) == BN_CLICKED)) {
				DestroyWindow(hWndDlg);
				return(TRUE);
			}

			if((LOWORD(wParam) == IDC_CLEAR) && (HIWORD(wParam) == BN_CLICKED)) {
				SendDlgItemMessage(hWndDlg, IDC_LOG, EM_SETSEL, 0, -1);
				SendDlgItemMessage(hWndDlg, IDC_LOG, EM_REPLACESEL, 0, (LPARAM)"");
				SendDlgItemMessage(hWndDlg, IDC_LOG, EM_SETSEL, 0, 0);

				return(TRUE);
			}

			return(FALSE);

		case WM_SIZE:
			RECT rc;
			int newLogWidth, newLogHeight, newClearX, newClearY, newCloseX, newCloseY;

			GetClientRect(hWndDlg, &rc);

			newLogWidth = rc.right - 14;
			newLogHeight = MAX(0, rc.bottom - 21 - 21);

			newClearX = MAX(0, rc.right - 76 - 62);
			newClearY = MAX(0, rc.bottom - 28);

			newCloseX = MAX(0, rc.right - 7 - 62);
			newCloseY = MAX(0, rc.bottom - 28);

			SetWindowPos(GetDlgItem(hWndDlg, IDC_LOG), NULL, 7, 7, newLogWidth, newLogHeight, SWP_NOZORDER | SWP_NOACTIVATE);
			SetWindowPos(GetDlgItem(hWndDlg, IDC_CLEAR), NULL, newClearX, newClearY, 62, 21, SWP_NOZORDER | SWP_NOACTIVATE);
			SetWindowPos(GetDlgItem(hWndDlg, IDC_CLOSE), NULL, newCloseX, newCloseY, 62, 21, SWP_NOZORDER | SWP_NOACTIVATE);

			return(TRUE);
		
		case WM_CLOSE:
			DestroyWindow(hWndDlg);
			return(TRUE);

		case WM_DESTROY:
			PostQuitMessage(0);
			return(TRUE);

		default:
			return(FALSE);
	}
}

void StatusWindow::CleanupThread() // To be called from otherThread, not the main one
{
	MSG msg;

	while(!TryEnterCriticalSection(&csThreadAlive)) {
		DWORD rv;
		HANDLE eventList[1];
		eventList[0] = eventCommandSent;
		
		rv = MsgWaitForMultipleObjects(1, eventList, FALSE, INFINITE, 0); // Not interested in any window events, just want to clear the command, and free the critical section	
		if(rv == WAIT_OBJECT_0) { // Once the result system is working, this will return RESULT_FAILURE
			switch(command->type) {
				case StatusCommand::COMMAND_CLOSE: // What a handy coincidence
					DestroyWindow(dialog);
					// Pump the remaining messages
					while(GetMessage(&msg, NULL, 0, 0) > 0);
					SetEvent(eventCommandAcknowledged);
					return;

				default:
					SetEvent(eventCommandAcknowledged);
					break;
			}
		
			delete command;
		}
	}
	
	threadAlive = false;
	LeaveCriticalSection(&csThreadAlive);
}