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

/**
 * Fuji network sockets.
 * Fuji default network sockets.
 */
enum MFNetwork_ServicePorts
{
	MFNetPort_InputServer = 12321,	/**< Default port for the Fuji remote input device server */
	MFNetPort_LogServer = 12322,	/**< Default port for the Fuji logging output server */
	MFNetPort_FileServer = 12323,	/**< Default port for the Fuji file server */
	MFNetPort_DebugServer = 12324,	/**< Default port for the Fuji script debuging server */
	MFNetPort_RPCServer = 12325		/**< Default port for the Fuji script remote procedure call server */
};

/**
 * Starts the fuji remote input server.
 * Starts the fuji remote input server and begins listening for remote input connections.
 * @return None.
 */
void MFNetwork_BeginInputServer();

/**
 * Check if the remote input server is running.
 * Checks if the remote input server is running.
 * @return Returns true if the remote input server is running.
 */
bool MFNetwork_IsRemoteInputServerRunning();

/**
 * Get maximum number of remote input devices.
 * Gets maximum number of remote input devices.
 * @return The maximum number of remote input devices.
 */
int MFNetwork_MaxRemoteDevices();

/**
 * Get the state of a remote input device.
 * Gets the state of a remote input device.
 * @param remoteDeviceID The remote device ID.
 * @return Returns the remote device state.
 */
int MFNetwork_GetRemoteDeviceStatus(int remoteDeviceID);

/**
 * Connect a local input device to a remote input server.
 * Connect a local input device to a remote input server.
 * @param remoteAddress Network address of the remote input server.
 * @param device Local device to connect to remote input server.
 * @param deviceID Local device ID of device.
 * @return Returns 0 on success.
 */
int MFNetwork_ConnectInputDeviceToRemoteHost(MFSocketAddress &remoteAddress, int device, int deviceID);

void MFNetwork_BeginLoggingServer();
int MFNetwork_ConnectToLogServer(MFSocketAddress &remoteAddress);

void MFNetwork_BeginFileServer();
void MFNetwork_BeginScriptDebugServer();
void MFNetwork_BeginScriptRPCServer();

#endif // _MFNETWORK_H

/** @} */
