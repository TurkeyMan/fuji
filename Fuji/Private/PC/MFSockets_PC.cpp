#pragma warning(disable: 4706)
#pragma warning(disable: 4127)

#include <winsock2.h>
#include <ws2tcpip.h>

#include "Fuji.h"
#include "MFSockets.h"

static WSADATA wsData;
static bool wsActive = false;

void MFSockets_InitModule()
{
	int error;

	error = WSAStartup(MAKEWORD(2, 0), &wsData);

	DBGASSERT(error == 0, "Winsock failed to start..");

	// check for correct version
	if(LOBYTE(wsData.wVersion) != 2 || HIBYTE(wsData.wVersion) != 0)
	{
		// incorrect WinSock version
		WSACleanup();
	}

	wsActive = true;
}

void MFSockets_DeinitModule()
{
	if(wsActive)
		WSACleanup();
}

MFSocket MFSockets_CreateSocket(MFAddressFamily addressFamily, MFSocketType type, int protocol)
{
	return (MFSocket)socket((int)addressFamily, (int)type, protocol);
}

int MFSockets_CloseSocket(MFSocket socket)
{
	return closesocket((SOCKET)socket);
}

sockaddr* MFSocketsPC_GetSockaddr(const MFSocketAddress *pAddress, int *pAddrLen)
{
	*pAddrLen = 0;

	if(!pAddress)
		return NULL;

	sockaddr *pSockAddr = NULL;

	if(pAddress->family == MFAF_Inet)
	{
		DBGASSERT(pAddress->cbSize == sizeof(MFSocketAddressInet), "address size does not match MFSocketAddressInet.");

		MFSocketAddressInet *pInet = (MFSocketAddressInet*)pAddress;

		static sockaddr_in ain;
		pSockAddr = (sockaddr*)&ain;
		*pAddrLen = sizeof(sockaddr_in);

		memset(&ain, 0, sizeof(sockaddr_in));
		ain.sin_family = (short)pInet->family;
		ain.sin_addr.S_un.S_un_b.s_b1 = pInet->address.b1;
		ain.sin_addr.S_un.S_un_b.s_b2 = pInet->address.b2;
		ain.sin_addr.S_un.S_un_b.s_b3 = pInet->address.b3;
		ain.sin_addr.S_un.S_un_b.s_b4 = pInet->address.b4;
		ain.sin_port = (uint16)pInet->port;
		MFEndian_HostToBig(&ain.sin_port);
	}
	else
	{
		DBGASSERT(false, "Unsupported socket family.");
	}

	return pSockAddr;
}

MFSocketAddress* MFSocketsPC_GetSocketAddress(const sockaddr *pSockAddress)
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

		inet.address.b1 = ain->sin_addr.S_un.S_un_b.s_b1;
		inet.address.b2 = ain->sin_addr.S_un.S_un_b.s_b2;
		inet.address.b3 = ain->sin_addr.S_un.S_un_b.s_b3;
		inet.address.b4 = ain->sin_addr.S_un.S_un_b.s_b4;
	}
	else
	{
		DBGASSERT(false, "Unsupported socket family.");
	}

	return pSockAddr;
}

int MFSockets_Bind(MFSocket socket, const MFSocketAddress &address)
{
	int addrLen = 0;
	sockaddr *pSockAddr = MFSocketsPC_GetSockaddr(&address, &addrLen);
	DBGASSERT(pSockAddr, "Invalid socket address...");

	return bind((SOCKET)socket, pSockAddr, addrLen);
}

int MFSockets_Connect(MFSocket socket, const MFSocketAddress &address)
{
	int addrLen = 0;
	sockaddr *pSockAddr = MFSocketsPC_GetSockaddr(&address, &addrLen);
	DBGASSERT(pSockAddr, "Invalid socket address...");

	return connect((SOCKET)socket, pSockAddr, addrLen);
}

int MFSockets_Listen(MFSocket socket, int backlog)
{
	return listen((SOCKET)socket, backlog);
}

MFSocket MFSockets_Accept(MFSocket socket, MFSocketAddress *pConnectingSocketAddress)
{
	char address[sizeof(sockaddr_in)*10];
	int size = sizeof(sockaddr_in)*10;

	MFSocket result = (MFSocket)accept((SOCKET)socket, (sockaddr*)&address, &size);

	if(pConnectingSocketAddress)
	{
		MFSocketAddress *pAddress = MFSocketsPC_GetSocketAddress((sockaddr*)&address);

		if(pConnectingSocketAddress->cbSize >= pAddress->cbSize)
		{
			memcpy(pConnectingSocketAddress, pAddress, pAddress->cbSize);
		}
	}

	return result;
}

int MFSockets_Send(MFSocket socket, const char *pBuffer, int bufferLength, uint32 flags)
{
	return send((SOCKET)socket, pBuffer, bufferLength, flags);
}

int MFSockets_SendTo(MFSocket socket, const char *pBuffer, int bufferLength, uint32 flags, const MFSocketAddress *pAddress)
{
	int addrLen = 0;
	sockaddr *pSockAddr = NULL;

	if(pAddress)
	{
		pSockAddr = MFSocketsPC_GetSockaddr(pAddress, &addrLen);
		DBGASSERT(pSockAddr, "Invalid socket address...");
	}

	return sendto((SOCKET)socket, pBuffer, bufferLength, flags, pSockAddr, addrLen);
}

int MFSockets_Recv(MFSocket socket, char *pBuffer, int bufferSize, uint32 flags)
{
	return recv((SOCKET)socket, pBuffer, bufferSize, flags);
}

int MFSockets_RecvFrom(MFSocket socket, char *pBuffer, int bufferSize, uint32 flags, MFSocketAddress *pSenderAddress)
{
	char address[sizeof(sockaddr_in)*10];
	int size = sizeof(sockaddr_in)*10;

	int result = recvfrom((SOCKET)socket, pBuffer, bufferSize, flags, (sockaddr*)&address, &size);

	if(pSenderAddress)
	{
		MFSocketAddress *pAddress = MFSocketsPC_GetSocketAddress((sockaddr*)&address);

		if(pSenderAddress->cbSize >= pAddress->cbSize)
		{
			memcpy(pSenderAddress, pAddress, pAddress->cbSize);
		}
	}

	return result;
}

int MFSockets_GetAddressInfo(const char *pAddress, const char *pServiceName, const MFAddressInfo *pHint, MFAddressInfo **ppAddressInfo)
{
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
		hint.ai_addr = MFSocketsPC_GetSockaddr(pHint->pAddress, (int*)&hint.ai_addrlen);
		hint.ai_next = NULL;
	}

	int result = getaddrinfo(pAddress, pServiceName, pHint ? &hint : NULL, &pSockAddr);

	DBGASSERT(!result, "getaddrinfo failed.");

	if(!result)
	{
		addrinfo *pAI = pSockAddr;

		while(pAI && numAddresses < maxNumAddresses)
		{
			addressInfo[numAddresses].family = (MFAddressFamily)pAI->ai_family;
			addressInfo[numAddresses].flags = pAI->ai_flags;
			addressInfo[numAddresses].type = (MFSocketType)pAI->ai_socktype;
			addressInfo[numAddresses].protocol = (MFSocketProtocol)pAI->ai_protocol;
			addressInfo[numAddresses].pCanonName = pAI->ai_canonname;
			addressInfo[numAddresses].pAddress = MFSocketsPC_GetSocketAddress(pAI->ai_addr);
			addressInfo[numAddresses].pNext = pAI->ai_next ? &addressInfo[numAddresses+1] : 0;

			++numAddresses;
			pAI = pAI->ai_next;
		}

		DBGASSERT(!pAI, STR("Too many address results returned. (>%d)", maxNumAddresses));

		*ppAddressInfo = addressInfo;
	}

	return result;
}

int MFSockets_SetSocketOptions(MFSocket socket, MFSocketOptions option, const void* optval, int optlen)
{

	if(option == MFSO_NonBlocking)
	{
		DBGASSERT(optlen == sizeof(uint32), "optval must be an unsigned int defining the blocking mode.");

		return ioctlsocket((SOCKET)socket, FIONBIO, (u_long*)optval);
	}
	else
	{
		return setsockopt((SOCKET)socket, SOL_SOCKET, option, (const char *)optval, optlen);
	}
}
