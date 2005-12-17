/**
 * @file MFNetwork.h
 * @brief Provides some engine level network features.
 * @author Manu Evans
 * @defgroup MFNetwork Network Features
 * @{
 */

#if !defined(_MFNETWORK_H)
#define _MFNETWORK_H

#include "MFSockets.h"

enum MFNetwork_ServicePorts
{
	MFNetPort_InputServer = 12321,	/**< Default port for the Fuji remote input device server */
	MFNetPort_LogServer = 12322,	/**< Default port for the Fuji logging output server */
	MFNetPort_FileServer = 12323,	/**< Default port for the Fuji file server */
	MFNetPort_DebugServer = 12324,	/**< Default port for the Fuji script debuging server */
	MFNetPort_RPCServer = 12325		/**< Default port for the Fuji script remote procedure call server */
};

void MFNetwork_BeginInputServer();
bool MFNetwork_IsRemoteInputServerRunning();
int MFNetwork_MaxRemoteDevices();
int MFNetwork_GetRemoteDeviceStatus(int remoteDeviceID);

int MFNetwork_ConnectInputDeviceToRemoteHost(MFSocketAddress &remoteAddress, int device, int deviceID);

void MFNetwork_BeginLoggingServer();
void MFNetwork_BeginFileServer();
void MFNetwork_BeginScriptDebugServer();
void MFNetwork_BeginScriptRPCServer();

#endif // _MFNETWORK_H

/** @} */
