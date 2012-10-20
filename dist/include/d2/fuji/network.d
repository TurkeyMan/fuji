module fuji.network;

import fuji.socket;

/**
 * Fuji network sockets.
 * Fuji default network sockets.
 */
enum MFNetwork_ServicePorts
{
	InputServer = 12321,	/**< Default port for the Fuji remote input device server */
	LogServer = 12322,		/**< Default port for the Fuji logging output server */
	FileServer = 12323,		/**< Default port for the Fuji file server */
	DebugServer = 12324,	/**< Default port for the Fuji script debuging server */
	RPCServer = 12325		/**< Default port for the Fuji script remote procedure call server */
}

/**
 * Starts the fuji remote input server.
 * Starts the fuji remote input server and begins listening for remote input connections.
 * @return None.
 */
extern (C) void MFNetwork_BeginInputServer();

/**
 * Check if the remote input server is running.
 * Checks if the remote input server is running.
 * @return Returns true if the remote input server is running.
 */
extern (C) bool MFNetwork_IsRemoteInputServerRunning();

/**
 * Get maximum number of remote input devices.
 * Gets maximum number of remote input devices.
 * @return The maximum number of remote input devices.
 */
extern (C) int MFNetwork_MaxRemoteDevices();

/**
 * Get the state of a remote input device.
 * Gets the state of a remote input device.
 * @param remoteDeviceID The remote device ID.
 * @return Returns the remote device state.
 */
extern (C) int MFNetwork_GetRemoteDeviceStatus(int remoteDeviceID);

/**
 * Connect a local input device to a remote input server.
 * Connect a local input device to a remote input server.
 * @param remoteAddress Network address of the remote input server.
 * @param device Local device to connect to remote input server.
 * @param deviceID Local device ID of device.
 * @return Returns 0 on success.
 */
extern (C) int MFNetwork_ConnectInputDeviceToRemoteHost(ref const(MFSocketAddress) remoteAddress, int device, int deviceID);

extern (C) void MFNetwork_BeginLoggingServer();
extern (C) int MFNetwork_ConnectToLogServer(ref MFSocketAddress remoteAddress);

extern (C) void MFNetwork_BeginFileServer();
extern (C) void MFNetwork_BeginScriptDebugServer();
extern (C) void MFNetwork_BeginScriptRPCServer();

extern (C) int MFNetwork_SendEmail(const(char*) pEmailText, const(char*) pSender = "someones@computer.com", const(char*) pEmailHandlerURL = "www.dotblip.com", int port = 80);

