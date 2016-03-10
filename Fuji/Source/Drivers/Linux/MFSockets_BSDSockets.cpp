#include "Fuji_Internal.h"

#if MF_SOCKETS == MF_DRIVER_BSDSOCKETS

#include "MFSockets.h"
#include "Util.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int MFSockets_InitModulePlatformSpecific()
{
	return 1;
}

void MFSockets_DeinitModulePlatformSpecific()
{
}

MFSocket MFSockets_CreateSocket(MFAddressFamily addressFamily, MFSocketType type, int protocol)
{
	return (MFSocket)(intp)socket((int)addressFamily, (int)type, protocol);
}

int MFSockets_CloseSocket(MFSocket socket)
{
	return close((int)(intp)socket);
}

sockaddr* MFSocketsPC_GetSockaddr(const MFSocketAddress *pAddress, int *pAddrLen)
{
	*pAddrLen = 0;

	if(!pAddress)
		return NULL;

	sockaddr *pSockAddr = NULL;

	switch(pAddress->family)
	{
		case MFAF_Inet:
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
			break;
		}
		case MFAF_Inet6:
		{
			MFDebug_Assert(pAddress->cbSize == sizeof(MFSocketAddressInet6), "address size does not match MFSocketAddressInet6.");

			MFSocketAddressInet6 *pInet6 = (MFSocketAddressInet6*)pAddress;

			static sockaddr_in6 ain6;
			pSockAddr = (sockaddr*)&ain6;
			*pAddrLen = sizeof(sockaddr_in6);

			MFZeroMemory(&ain6, sizeof(sockaddr_in6));
			ain6.sin6_family = (short)pInet6->family;
			ain6.sin6_port = (uint16)pInet6->port;
			MFEndian_HostToBig(&ain6.sin6_port);
			ain6.sin6_flowinfo = pInet6->flowInfo;
			MFEndian_HostToBig(&ain6.sin6_flowinfo);
			ain6.sin6_scope_id = pInet6->scopeId;
			MFEndian_HostToBig(&ain6.sin6_scope_id);
			for(int a=0; a<8; ++a)
			{
				ain6.sin6_addr.s6_addr16[a] = pInet6->address.s[a];
				MFEndian_HostToBig(&ain6.sin6_addr.s6_addr16[a]);
			}
			break;
		}
		default:
		{
			MFDebug_Assert(false, "Unsupported socket family.");
			break;
		}
	}

	return pSockAddr;
}

MFSocketAddress* MFSocketsPC_GetSocketAddress(const sockaddr *pSockAddress)
{
	MFSocketAddress *pSockAddr = NULL;

	switch(pSockAddress->sa_family)
	{
		case MFAF_Inet:
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
			break;
		}
		case MFAF_Inet6:
		{
			static MFSocketAddressInet6 inet;
			pSockAddr = &inet;

			sockaddr_in6 *ain = (sockaddr_in6*)pSockAddress;

			inet.cbSize = sizeof(MFSocketAddressInet6);

			inet.family = MFAF_Inet6;

			uint16 port = ain->sin6_port;
			MFEndian_BigToHost(&port);
			inet.port = (int)port;

			inet.flowInfo = ain->sin6_flowinfo;
			MFEndian_BigToHost(&inet.flowInfo);

			inet.scopeId = ain->sin6_scope_id;
			MFEndian_BigToHost(&inet.scopeId);

			for(int a=0; a<8; ++a)
			{
				inet.address.s[a] = ain->sin6_addr.s6_addr16[a];
				MFEndian_BigToHost(&inet.address.s[a]);
			}
			break;
		}
		default:
		{
			MFDebug_Assert(false, "Unsupported socket family.");
			break;
		}
	}

	return pSockAddr;
}

int MFSockets_Bind(MFSocket socket, const MFSocketAddress &address)
{
	int addrLen = 0;
	sockaddr *pSockAddr = MFSocketsPC_GetSockaddr(&address, &addrLen);
	MFDebug_Assert(pSockAddr, "Invalid socket address...");

	return bind((int)(intp)socket, pSockAddr, addrLen);
}

int MFSockets_Connect(MFSocket socket, const MFSocketAddress &address)
{
	int addrLen = 0;
	sockaddr *pSockAddr = MFSocketsPC_GetSockaddr(&address, &addrLen);
	MFDebug_Assert(pSockAddr, "Invalid socket address...");

	return connect((int)(intp)socket, pSockAddr, addrLen);
}

int MFSockets_Listen(MFSocket socket, int backlog)
{
	return listen((int)(intp)socket, backlog);
}

MFSocket MFSockets_Accept(MFSocket socket, MFSocketAddress *pConnectingSocketAddress)
{
	char address[sizeof(sockaddr_in)*10];
	socklen_t size = sizeof(sockaddr_in)*10;

	MFSocket result = (MFSocket)(intp)accept((int)(intp)socket, (sockaddr*)&address, &size);

	if(pConnectingSocketAddress)
	{
		MFSocketAddress *pAddress = MFSocketsPC_GetSocketAddress((sockaddr*)&address);

		if(pConnectingSocketAddress->cbSize >= pAddress->cbSize)
		{
			MFCopyMemory(pConnectingSocketAddress, pAddress, pAddress->cbSize);
		}
	}

	return result;
}

int MFSockets_Send(MFSocket socket, const char *pBuffer, size_t bufferLength, uint32 flags)
{
	return send((int)(intp)socket, pBuffer, bufferLength, flags);
}

int MFSockets_SendTo(MFSocket socket, const char *pBuffer, size_t bufferLength, uint32 flags, const MFSocketAddress *pAddress)
{
	int addrLen = 0;
	sockaddr *pSockAddr = NULL;

	if(pAddress)
	{
		pSockAddr = MFSocketsPC_GetSockaddr(pAddress, &addrLen);
		MFDebug_Assert(pSockAddr, "Invalid socket address...");
	}

	return sendto((int)(intp)socket, pBuffer, bufferLength, flags, pSockAddr, addrLen);
}

int MFSockets_Recv(MFSocket socket, char *pBuffer, size_t bufferSize, uint32 flags)
{
	return recv((int)(intp)socket, pBuffer, bufferSize, flags);
}

int MFSockets_RecvFrom(MFSocket socket, char *pBuffer, size_t bufferSize, uint32 flags, MFSocketAddress *pSenderAddress)
{
	char address[sizeof(sockaddr_in)*10];
	socklen_t size = sizeof(sockaddr_in)*10;

	int result = recvfrom((int)(intp)socket, pBuffer, bufferSize, flags, (sockaddr*)&address, &size);

	if(pSenderAddress)
	{
		MFSocketAddress *pAddress = MFSocketsPC_GetSocketAddress((sockaddr*)&address);

		if(pSenderAddress->cbSize >= pAddress->cbSize)
		{
			MFCopyMemory(pSenderAddress, pAddress, pAddress->cbSize);
		}
	}

	return result;
}

int MFSockets_GetAddressInfo(const char *pAddress, const char *pServiceName, const MFAddressInfo *pHint, MFAddressInfo **ppAddressInfo)
{
	int result = 0;

	const int maxNumAddresses = 20;
	static MFAddressInfo addressInfo[maxNumAddresses];
	static MFSocketAddressInet6 addressInfoAddress[maxNumAddresses];
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

	result = getaddrinfo(pAddress, pServiceName, pHint ? &hint : NULL, &pSockAddr);

	if(result)
	{
		// some error
		switch(result)
		{
			default:
				MFDebug_Assert(!result, "getaddrinfo failed.");
		}
	}
	else
	{
		addrinfo *pAI = pSockAddr;

		while(pAI && numAddresses < maxNumAddresses)
		{
			if(pAI->ai_family == MFAF_Inet || pAI->ai_family == MFAF_Inet6)
			{
				if(numAddresses > 0)
					addressInfo[numAddresses-1].pNext = &addressInfo[numAddresses];

				addressInfo[numAddresses].family = (MFAddressFamily)pAI->ai_family;
				addressInfo[numAddresses].flags = pAI->ai_flags;
				addressInfo[numAddresses].type = (MFSocketType)pAI->ai_socktype;
				addressInfo[numAddresses].protocol = (MFSocketProtocol)pAI->ai_protocol;
				addressInfo[numAddresses].pCanonName = pAI->ai_canonname;
				MFCopyMemory(&addressInfoAddress[numAddresses], MFSocketsPC_GetSocketAddress(pAI->ai_addr), pAI->ai_family == MFAF_Inet ? sizeof(MFSocketAddressInet) : sizeof(MFSocketAddressInet6));
				addressInfo[numAddresses].pAddress = (MFSocketAddressInet*)&addressInfoAddress[numAddresses];
				addressInfo[numAddresses].pNext = NULL;
				++numAddresses;
			}

			pAI = pAI->ai_next;
		}

		MFDebug_Assert(!pAI, MFStr("Too many address results returned. (>%d)", maxNumAddresses));

		*ppAddressInfo = addressInfo;
	}

	return result;
}

int MFSockets_SetSocketOptions(MFSocket socket, MFSocketOptions option, const void* optval, int optlen)
{

	if(option == MFSO_NonBlocking)
	{
		MFDebug_Assert(optlen == sizeof(uint32), "optval must be an unsigned int defining the blocking mode.");

		int flags = fcntl((int)(intp)socket, F_GETFL, 0);
		return fcntl((int)(intp)socket, F_SETFL, *(const uint32*)optval ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK));
	}
	else
	{
		return setsockopt((int)(intp)socket, SOL_SOCKET, option, (const char *)optval, optlen);
	}
}

MFSocketError MFSockets_GetLastError()
{
	switch(errno)
	{
		case EINPROGRESS:
			return MFSockError_WouldBlock;
		case ETIMEDOUT:
			return MFSockError_TimedOut;
		case EINTR:
			return MFSockError_Interrupted;
		case EALREADY:
			return MFSockError_AlreadyInProgress;
		case ENETUNREACH:
			return MFSockError_NetworkUnreachable;
		case EHOSTUNREACH:
			return MFSockError_HostUnreachable;
		case ECONNREFUSED:
			return MFSockError_ConnectionRefused;
		case EISCONN:
			return MFSockError_IsConnected;
	}

	return MFSockError_Unknown;
}

#endif // MF_SOCKETS
