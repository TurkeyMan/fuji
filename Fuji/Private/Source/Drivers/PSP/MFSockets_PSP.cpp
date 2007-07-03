#include "Fuji.h"

#if MF_SOCKETS == PSP

#include "MFSockets.h"

#include <pspkernel.h>
#include <pspsdk.h>
#include <pspnet.h>
#include <pspnet_inet.h>
#include <pspnet_apctl.h>
#include <pspnet_resolver.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>

static char gIPAddress[32] = "";

static bool netActive = false;

int MFSocketsPSP_ConnectToAccessPoint()
{
	int error;
	int stateLast = -1;

	// Connect using the first profile
	error = sceNetApctlConnect(1);
	if(error)
	{
		MFDebug_Warn(1, MFStr("sceNetApctlConnect returns %08X", error));
		return 0;
	}

	MFDebug_Warn(4, "Connecting...");

	while(1)
	{
		int state;
		error = sceNetApctlGetState(&state);
		if(error)
		{
			MFDebug_Warn(1, MFStr("sceNetApctlGetState returns $%x", error));
			return error;
		}
		if(state > stateLast)
		{
			MFDebug_Warn(4, MFStr("Connection state %d of 4", state));
			stateLast = state;
		}
		if(state == 4)
			break;  // connected with static IP

		// wait a little before polling again
		sceKernelDelayThread(50*1000); // 50ms
	}

	MFDebug_Warn(3, "Connected to access point!");

	return error;
}

int MFSockets_InitModulePlatformSpecific()
{
	// lets not try and initialise the inet...
	return 0;

	int error;

	MFDebug_Log(4, "Attempting to load inet modules...");
	error = pspSdkLoadInetModules();

	if(error >= 0)
	{
		MFDebug_Warn(1, "Could not load inet modules");
		return 0;
	}

	MFDebug_Log(4, "Attempting to initialise the system inet module...");
	error = pspSdkInetInit();

	if(error)
	{
		MFDebug_Warn(1, MFStr("Could not initialise the network %08X", error));
		return 0;
	}

	if(MFSocketsPSP_ConnectToAccessPoint())
	{
		MFDebug_Warn(1, "Couldnt connect to access point");
		return 0;
	}

	if(sceNetApctlGetInfo(8, gIPAddress))
	{
		MFDebug_Warn(1, "Couldnt get IP address");
		MFString_Copy(gIPAddress, "Unknown IP address");
		return 0;
	}

	netActive = true;
	return 1;
}

void MFSockets_DeinitModulePlatformSpecific()
{
	// Terminate the pspnet_apctl module
//	pspNetApctlTerm();
	// Terminate the pspnet_inet module
//	pspNetInetTerm();
	// Terminate the pspnet module
//	pspNetTerm();
}

MFSocket MFSockets_CreateSocket(MFAddressFamily addressFamily, MFSocketType type, int protocol)
{
	return (MFSocket)sceNetInetSocket((int)addressFamily, (int)type, protocol);
}

int MFSockets_CloseSocket(MFSocket socket)
{
	return sceNetInetClose((int)socket);
}

sockaddr* MFSocketsPSP_GetSockaddr(const MFSocketAddress *pAddress, int *pAddrLen)
{
	*pAddrLen = 0;

	if(!pAddress)
		return NULL;

	sockaddr *pSockAddr = NULL;

	if(pAddress->family == MFAF_Inet)
	{
		MFDebug_Assert(pAddress->cbSize == sizeof(MFSocketAddressInet), "address size does not match MFSocketAddressInet.");

		MFSocketAddressInet *pInet = (MFSocketAddressInet*)pAddress;

		static sockaddr_in ain;
		pSockAddr = (sockaddr*)&ain;
		*pAddrLen = sizeof(sockaddr_in);

		MFZeroMemory(&ain, sizeof(sockaddr_in));
		ain.sin_family = (short)pInet->family;
		ain.sin_addr.s_addr = pInet->address.address;
		ain.sin_port = (uint16)pInet->port;
		MFEndian_HostToBig(&ain.sin_port);
	}
	else
	{
		MFDebug_Assert(false, "Unsupported socket family.");
	}

	return pSockAddr;
}

MFSocketAddress* MFSocketsPSP_GetSocketAddress(const sockaddr *pSockAddress)
{
	MFSocketAddress *pSockAddr = NULL;

	if(pSockAddress->sa_family == (int)MFAF_Inet)
	{
		static MFSocketAddressInet inet;
		pSockAddr = &inet;

		sockaddr_in *ain = (sockaddr_in*)pSockAddress;

		inet.cbSize = sizeof(MFSocketAddressInet);

		inet.family = MFAF_Inet;

		uint16 t = ain->sin_port;
		MFEndian_BigToHost(&t);
		inet.port = (int)t;
		inet.address.address = ain->sin_addr.s_addr;
	}
	else
	{
		MFDebug_Assert(false, "Unsupported socket family.");
	}

	return pSockAddr;
}

int MFSockets_Bind(MFSocket socket, const MFSocketAddress &address)
{
	int addrLen = 0;
	sockaddr *pSockAddr = MFSocketsPSP_GetSockaddr(&address, &addrLen);
	MFDebug_Assert(pSockAddr, "Invalid socket address...");

	return sceNetInetBind((int)socket, pSockAddr, addrLen);
}

int MFSockets_Connect(MFSocket socket, const MFSocketAddress &address)
{
	int addrLen = 0;
	sockaddr *pSockAddr = MFSocketsPSP_GetSockaddr(&address, &addrLen);
	MFDebug_Assert(pSockAddr, "Invalid socket address...");

	return sceNetInetConnect((int)socket, pSockAddr, addrLen);
}

int MFSockets_Listen(MFSocket socket, int backlog)
{
	return sceNetInetListen((int)socket, backlog);
}

MFSocket MFSockets_Accept(MFSocket socket, MFSocketAddress *pConnectingSocketAddress)
{
	char address[sizeof(sockaddr_in)*4];
	socklen_t size = sizeof(sockaddr_in)*4;

	MFSocket result = (MFSocket)sceNetInetAccept((int)socket, (sockaddr*)&address, &size);

	if(pConnectingSocketAddress)
	{
		MFSocketAddress *pAddress = MFSocketsPSP_GetSocketAddress((sockaddr*)&address);

		if(pConnectingSocketAddress->cbSize >= pAddress->cbSize)
		{
			MFCopyMemory(pConnectingSocketAddress, pAddress, pAddress->cbSize);
		}
	}

	return result;
}

int MFSockets_Send(MFSocket socket, const char *pBuffer, int bufferLength, uint32 flags)
{
	return sceNetInetSend((int)socket, pBuffer, bufferLength, flags);
}

int MFSockets_SendTo(MFSocket socket, const char *pBuffer, int bufferLength, uint32 flags, const MFSocketAddress *pAddress)
{
	int addrLen = 0;
	sockaddr *pSockAddr = NULL;

	if(pAddress)
	{
		pSockAddr = MFSocketsPSP_GetSockaddr(pAddress, &addrLen);
		MFDebug_Assert(pSockAddr, "Invalid socket address...");
	}

	return sceNetInetSendto((int)socket, pBuffer, bufferLength, flags, pSockAddr, addrLen);
}

int MFSockets_Recv(MFSocket socket, char *pBuffer, int bufferSize, uint32 flags)
{
	return sceNetInetRecv((int)socket, pBuffer, bufferSize, flags);
}

int MFSockets_RecvFrom(MFSocket socket, char *pBuffer, int bufferSize, uint32 flags, MFSocketAddress *pSenderAddress)
{
	char address[sizeof(sockaddr_in)*4];
	socklen_t size = sizeof(sockaddr_in)*4;

	int result = sceNetInetRecvfrom((int)socket, pBuffer, bufferSize, flags, (sockaddr*)&address, &size);

	if(pSenderAddress)
	{
		MFSocketAddress *pAddress = MFSocketsPSP_GetSocketAddress((sockaddr*)&address);

		if(pSenderAddress->cbSize >= pAddress->cbSize)
		{
			MFCopyMemory(pSenderAddress, pAddress, pAddress->cbSize);
		}
	}

	return result;
}

int MFSockets_GetAddressInfo(const char *pAddress, const char *pServiceName, const MFAddressInfo *pHint, MFAddressInfo **ppAddressInfo)
{
	// TODO: NFI how to do this on PSP...
/*
	const int maxNumAddresses = 20;
	static MFAddressInfo addressInfo[maxNumAddresses];
	int numAddresses = 0;

	static addrinfo *pSockAddr = NULL;
	addrinfo hint;

	if(pSockAddr)
	{
		// **** this is a totally LAME way to manage the addrinfo cleanup!!
		freeaddrinfo(pSockAddr);
	}

	*ppAddressInfo = NULL;

	if(pHint)
	{
		hint.ai_family = (int)pHint->family;
		hint.ai_flags = pHint->flags;
		hint.ai_socktype = (int)pHint->type;
		hint.ai_protocol = (int)pHint->protocol;
		hint.ai_canonname = (char*)pHint->pCanonName;
		hint.ai_addr = MFSocketsPSP_GetSockaddr(pHint->pAddress, (int*)&hint.ai_addrlen);
		hint.ai_next = NULL;
	}

	int result = getaddrinfo(pAddress, pServiceName, pHint ? &hint : NULL, &pSockAddr);

	if(result)
	{
		// some error

		switch(result)
		{
			case WSAHOST_NOT_FOUND:
				MFDebug_Warn(4, MFStr("Host not found: '%s'", pAddress));
				break;
			default:
				MFDebug_Assert(!result, "getaddrinfo failed.");
		}
	}
	else
	{
		addrinfo *pAI = pSockAddr;

		while(pAI && numAddresses < maxNumAddresses)
		{
			addressInfo[numAddresses].family = (MFAddressFamily)pAI->ai_family;
			addressInfo[numAddresses].flags = pAI->ai_flags;
			addressInfo[numAddresses].type = (MFSocketType)pAI->ai_socktype;
			addressInfo[numAddresses].protocol = (MFSocketProtocol)pAI->ai_protocol;
			addressInfo[numAddresses].pCanonName = pAI->ai_canonname;
			addressInfo[numAddresses].pAddress = MFSocketsPSP_GetSocketAddress(pAI->ai_addr);
			addressInfo[numAddresses].pNext = pAI->ai_next ? &addressInfo[numAddresses+1] : 0;

			++numAddresses;
			pAI = pAI->ai_next;
		}

		MFDebug_Assert(!pAI, MFStr("Too many address results returned. (>%d)", maxNumAddresses));

		*ppAddressInfo = addressInfo;
	}

	return result;
*/
	return 1;
}

int MFSockets_SetSocketOptions(MFSocket socket, MFSocketOptions option, const void* optval, int optlen)
{
	if(option == MFSO_NonBlocking)
	{
		MFDebug_Assert(optlen == sizeof(uint32), "optval must be an unsigned int defining the blocking mode.");

		// TODO: NFI how to do this on PSP...
//		return ioctlsocket((int)socket, FIONBIO, (u_long*)optval);
		return 0;
	}
	else
	{
		return sceNetInetSetsockopt((int)socket, SOL_SOCKET, option, (const char *)optval, optlen);
	}
}

#endif
