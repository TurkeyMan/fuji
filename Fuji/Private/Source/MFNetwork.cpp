#include "Fuji.h"
#include "MFSockets.h"
#include "MFThread.h"
#include "MFNetwork_Internal.h"


/**************************************************************************
** Remote gamepad support
**************************************************************************/
struct RemoteInputPacket
{
	uint16 auth;
	uint16 packetLen;
	uint32 id;
};

struct InitEvent : public RemoteInputPacket
{
	MFInputDevice deviceType;
};

struct NameEvent : public RemoteInputPacket
{
	int buttonID;
};

struct InputEvent : public RemoteInputPacket
{
	int buton;
	float state;
};

static MFThread inputServer;
static MFMutex inputServerMutex;

// shared local devices
struct SharedDevice
{
	MFSocket socket;

	MFInputDevice device;
	int deviceID;
};

static const int gMaxSharedDevices = 4;
static SharedDevice gSharedDevices[gMaxSharedDevices];

// remote gamepads
struct RemoteGamepad
{
	char name[128];
	char butonNames[GamepadType_Max][128];
	float buttonState[GamepadType_Max];

	MFInputDeviceStatus status;
};

static const int gMaxNetworkGamepads = 4;
static RemoteGamepad gNetworkGamepads[gMaxNetworkGamepads];

// network gamepad functions
RemoteGamepad* MFNetwork_GetNewNetworkGamepadPointer()
{
	RemoteGamepad *pGamepad = NULL;

	MFThread_LockMutex(inputServerMutex);

	for(int a=0; a<gMaxNetworkGamepads; a++)
	{
		if(gNetworkGamepads[a].status == IDS_Unavailable)
		{
			pGamepad = &gNetworkGamepads[a];
			memset(pGamepad->buttonState, 0, sizeof(pGamepad->buttonState));
			pGamepad->status = IDS_Waiting;
			break;
		}
	}

	MFThread_ReleaseMutex(inputServerMutex);

	return pGamepad;
}

uint32 MFNetwork_InputServerConnectionTread(void *pUserData)
{
	bool connectionTerminated = false;
	char recvBuffer[160];

	MFSocket connection = (MFSocket)pUserData;

	RemoteGamepad *pGamepad = MFNetwork_GetNewNetworkGamepadPointer();

	if(!pGamepad)
	{
		MFDebug_Warn(2, "Maximum number of remote gamepads reached. Couldnt allocate remote gamepad.");
		MFSockets_CloseSocket(connection);
		return 0;
	}

	while(!connectionTerminated)
	{
		int bytes = MFSockets_Recv(connection, recvBuffer, sizeof(recvBuffer), 0);

		if(bytes == 0)
		{
			MFDebug_Warn(1, "Remote gamepad disconnected.");
			connectionTerminated = true;
		}

		const char *pBuffer = recvBuffer;

		while(bytes)
		{
			RemoteInputPacket *pPacket = (RemoteInputPacket*)pBuffer;

			if(pPacket->auth != 0xFEF0)
			{
				MFDebug_Warn(2, "Malformed packet..");
				bytes = 0;
				continue;
			}

			if(bytes < sizeof(RemoteInputPacket) || bytes < pPacket->packetLen)
			{
				memcpy(recvBuffer, pBuffer, bytes);
				bytes += MFSockets_Recv(connection, recvBuffer+bytes, sizeof(recvBuffer)-bytes, 0);
				pBuffer = recvBuffer;
				pPacket = (RemoteInputPacket*)pBuffer;
			}

			if(pPacket->id == MFMAKEFOURCC('I','N','I','T'))
			{
				InitEvent *pInitEvent = (InitEvent*)pBuffer;
				MFDebug_Assert(pInitEvent->deviceType == IDD_Gamepad, "Only gamepads supported currently..");

				strcpy(pGamepad->name, (char*)&pInitEvent[1]);
			}
			else if(pPacket->id == MFMAKEFOURCC('N','A','M','E'))
			{
				NameEvent *pNameEvent = (NameEvent*)pBuffer;

				strcpy(pGamepad->butonNames[pNameEvent->buttonID], (char*)&pNameEvent[1]);

				// if we have received the last button name, make the gamepad active and clear the state..
				if(pNameEvent->buttonID == GamepadType_Max-1)
				{
					MFThread_LockMutex(inputServerMutex);

					memset(pGamepad->buttonState, 0, sizeof(pGamepad->buttonState));
					pGamepad->status = IDS_Ready;

					MFThread_ReleaseMutex(inputServerMutex);
				}
			}
			else if(pPacket->id == MFMAKEFOURCC('S','T','A','T'))
			{
				InputEvent *pInputEvent = (InputEvent*)pBuffer;

				MFThread_LockMutex(inputServerMutex);
				pGamepad->buttonState[pInputEvent->buton] = pInputEvent->state;
				MFThread_ReleaseMutex(inputServerMutex);
			}
			else
			{
				recvBuffer[255] = 0;
				MFDebug_Warn(2, MFStr("Unknown packet received: \"%s\"", recvBuffer));
			}

			pBuffer += pPacket->packetLen;
			bytes -= pPacket->packetLen;
		}
	}

	MFThread_LockMutex(inputServerMutex);
	pGamepad->status = (MFInputDeviceStatus)-1;
	MFThread_ReleaseMutex(inputServerMutex);

	MFSockets_CloseSocket(connection);

	return 0;
}

uint32 MFNetwork_InputServerListenTread(void *pUserData)
{
	MFSocket inputServer = MFSockets_CreateSocket(MFAF_Inet, MFSockType_Stream, MFProtocol_TCP);

	MFSocketAddressInet addr;
	addr.cbSize = sizeof(addr);
	addr.family = MFAF_Inet;
	addr.address.address = 0;
	addr.port = MFNetPort_InputServer;

	MFSockets_Bind(inputServer, addr);

	while(!MFSockets_Listen(inputServer, 0x7FFFFFFF))
	{
		MFSocketAddressInet remoteAddress;
		remoteAddress.cbSize = sizeof(MFSocketAddressInet);

		MFSocket connection = MFSockets_Accept(inputServer, &remoteAddress);

		if(connection)
		{
			MFDebug_Warn(1, MFStr("Remote gamepad connection accepted: %s", MFSockets_GetAddressString(remoteAddress)));
			MFThread_CreateThread("Fuji Input Connection", MFNetwork_InputServerConnectionTread, connection);
		}
		else
		{
			// failed somehow
		}
	}

	return 0;
}

void MFNetwork_BeginInputServer()
{
	if(!MFSockets_IsActive())
		return;

	inputServerMutex = MFThread_CreateMutex("Network Gamepad List");
	inputServer	= MFThread_CreateThread("Fuji Input Server", MFNetwork_InputServerListenTread, NULL);
}

int MFNetwork_ConnectInputDeviceToRemoteHost(MFSocketAddress &remoteAddress, int device, int deviceID)
{
	if(!MFInput_IsAvailable(device, deviceID))
	{
		MFDebug_Assert(false, "Device is not available.");
		return 1;
	}

	SharedDevice *pDevice = NULL;

	for(int i=0; i<gMaxSharedDevices; i++)
	{
		if(!gSharedDevices[i].socket)
		{
			pDevice = &gSharedDevices[i];
			break;
		}
	}

	if(!pDevice)
	{
		MFDebug_Warn(false, "Maximum shared devices reached, cannot share device.");
		return 2;
	}

	MFSocket socket = MFSockets_CreateSocket(MFAF_Inet, MFSockType_Stream, MFProtocol_TCP);	

	int r = MFSockets_Connect(socket, remoteAddress);

	if(r)
	{
		MFDebug_Warn(false, "Error connecting to remote host.");
		MFSockets_CloseSocket(socket);
		return 3;
	}

	char buffer[160];

	pDevice->socket = socket;
	pDevice->device = (MFInputDevice)device;
	pDevice->deviceID = deviceID;

	const char *pDeviceName = MFInput_GetDeviceName(device, deviceID);
	int len = strlen(pDeviceName);
	MFDebug_Assert(len < 128, "Device name too long...");

	InitEvent *pInit = (InitEvent*)buffer;
	pInit->auth = 0xFEF0;
	pInit->id = MFMAKEFOURCC('I','N','I','T');
	pInit->deviceType = (MFInputDevice)device;
	strcpy((char*)&pInit[1], pDeviceName);
	pInit->packetLen = (uint16)MFALIGN(sizeof(InitEvent) + len + 1, 4);

	MFSockets_Send(pDevice->socket, buffer, pInit->packetLen, 0);

	for(int a=0; a<GamepadType_Max; a++)
	{
		const char *pButtonName = MFInput_GetGamepadButtonName(a, deviceID);
		len = strlen(pButtonName);
		MFDebug_Assert(len < 128, "Button name too long...");

		NameEvent *pName = (NameEvent*)buffer;
		pName->auth = 0xFEF0;
		pName->id = MFMAKEFOURCC('N','A','M','E');
		pName->buttonID = a;
		strcpy((char*)&pName[1], pButtonName);
		pName->packetLen = (uint16)MFALIGN(sizeof(NameEvent) + len + 1, 4);

		MFSockets_Send(pDevice->socket, buffer, pName->packetLen, 0);
	}

	return 0;
}

void MFNetwork_SendInputStateUpdate(SharedDevice *pDevice)
{
	for(int a=0; a<GamepadType_Max; a++)
	{
		if(MFInput_IsReady(pDevice->device, pDevice->deviceID))
		{
			float prev;
			float state = MFInput_Read(a, pDevice->device, pDevice->deviceID, &prev);

			if(state != prev)
			{
				InputEvent event;
				event.auth = 0xFEF0;
				event.id = MFMAKEFOURCC('S','T','A','T');
				event.packetLen = sizeof(InputEvent);
				event.buton = a;
				event.state = state;

				MFSockets_Send(pDevice->socket, (char*)&event, sizeof(event), 0);
			}
		}
	}
}

bool MFNetwork_IsRemoteInputServerRunning()
{
	return inputServer != NULL;
}

int MFNetwork_MaxRemoteDevices()
{
	return gMaxNetworkGamepads;
}

int MFNetwork_GetRemoteDeviceStatus(int remoteDeviceID)
{
	MFInputDeviceStatus status;

	MFThread_LockMutex(inputServerMutex);

	status = gNetworkGamepads[remoteDeviceID].status;

	MFThread_ReleaseMutex(inputServerMutex);

	return status;
}

void MFNetwork_GetRemoteGamepadState(int id, MFGamepadState *pGamepadState)
{
	for(int a=0; a<GamepadType_Max; a++)
	{
		pGamepadState->values[a] = gNetworkGamepads[id].buttonState[a];
	}
}

const char* MFNetwork_GetRemoteGamepadName(int id)
{
	if(MFNetwork_GetRemoteDeviceStatus(id) != IDS_Ready)
		return "Waiting...";

	return gNetworkGamepads[id].name;
}

const char* MFNetwork_GetRemoteGamepadButtonName(int id, int button)
{
	if(MFNetwork_GetRemoteDeviceStatus(id) != IDS_Ready)
		return "";

	return gNetworkGamepads[id].butonNames[button];
}

void MFNetwork_LockInputMutex()
{
	MFThread_LockMutex(inputServerMutex);
}

void MFNetwork_ReleaseInputMutex()
{
	MFThread_ReleaseMutex(inputServerMutex);
}

void MFNetwork_BeginLoggingServer()
{

}

void MFNetwork_BeginFilesystemServer()
{
	
}

void MFNetwork_BeginScriptDebugServer()
{

}




void MFNetwork_InitModule()
{
	memset(gNetworkGamepads, 0, sizeof(gNetworkGamepads));
	memset(gSharedDevices, 0, sizeof(gSharedDevices));
}

void MFNetwork_DeinitModule()
{

}

void MFNetwork_Update()
{
	if(!MFNetwork_IsRemoteInputServerRunning())
		return;

	for(int a=0; a<gMaxSharedDevices; a++)
	{
		if(gSharedDevices[a].socket)
			MFNetwork_SendInputStateUpdate(&gSharedDevices[a]);
	}

	MFThread_LockMutex(inputServerMutex);

	for(int a=0; a<gMaxNetworkGamepads; a++)
	{
		if(gNetworkGamepads[a].status == -1)
			gNetworkGamepads[a].status = IDS_Unavailable;
	}

	MFThread_ReleaseMutex(inputServerMutex);
}


