#include "Fuji.h"
#include "MFSockets.h"

void MFSockets_InitModule()
{
}

void MFSockets_DeinitModule()
{
}

MFSocket MFSockets_CreateSocket(MFAddressFamily addressFamily, MFSocketType type, int protocol)
{
	return 0;
}

int MFSockets_CloseSocket(MFSocket socket)
{
	return 0;
}

int MFSockets_Bind(MFSocket socket, const MFSocketAddress &address)
{
	return 0;
}

int MFSockets_Connect(MFSocket socket, const MFSocketAddress &address)
{
	return 0;
}

int MFSockets_Listen(MFSocket socket, int backlog)
{
	return 0;
}

MFSocket MFSockets_Accept(MFSocket socket, MFSocketAddress *pConnectingSocketAddress)
{
	return 0;
}

int MFSockets_Send(MFSocket socket, const char *pBuffer, int bufferLength, uint32 flags)
{
	return bufferLength;
}

int MFSockets_SendTo(MFSocket socket, const char *pBuffer, int bufferLength, uint32 flags, const MFSocketAddress *pAddress)
{
	return bufferLength;
}

int MFSockets_Recv(MFSocket socket, char *pBuffer, int bufferSize, uint32 flags)
{
	return bufferSize;
}

int MFSockets_RecvFrom(MFSocket socket, char *pBuffer, int bufferSize, uint32 flags, MFSocketAddress *pSenderAddress)
{
	return bufferSize;
}

int MFSockets_GetAddressInfo(const char *pAddress, const char *pServiceName, const MFAddressInfo *pHint, MFAddressInfo **ppAddressInfo)
{
/*
	addressInfo[0].family = (MFAddressFamily)pAI->ai_family;
	addressInfo[0].flags = pAI->ai_flags;
	addressInfo[0].type = (MFSocketType)pAI->ai_socktype;
	addressInfo[0].protocol = (MFSocketProtocol)pAI->ai_protocol;
	addressInfo[0].pCanonName = pAI->ai_canonname;
	addressInfo[0].pAddress = MFSocketsPC_GetSocketAddress(pAI->ai_addr);
	addressInfo[0].pNext = pAI->ai_next ? &addressInfo[numAddresses+1] : 0;
*/
	*ppAddressInfo = NULL;
	return 0;
}

int MFSockets_SetSocketOptions(MFSocket socket, MFSocketOptions option, const void* optval, int optlen)
{
	return 0;
}
