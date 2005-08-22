#if !defined(_MFSOCKETS_H)
#define _MFSOCKETS_H

typedef void* MFSocket;

// MFSockets enum's
enum MFAddressFamily
{
	MFAF_Unknown = -1,

	MFAF_Unspecified = 0,

	MFAF_Unix = 1,		// local to host (pipes, portals)
	MFAF_Inet = 2,		// internetwork: UDP, TCP, etc. // **** Fuji only supports Inet currently
	MFAF_IPX = 6,		// IPX protocols: IPX, SPX, etc.
	MFAF_NetBios = 17,	// NetBios-style addresses
	MFAF_Inet6 = 23,	// Internetwork Version 6
	MFAF_IRDA = 26,		// IrDA

	MFAF_Max = 32,
	MFAF_ForceInt = 0x7FFFFFFF
};

enum MFSocketType
{
	MFSockType_Unknown = -1,

	MFSockType_Stream = 1,	// stream socket
	MFSockType_Datagram,	// datagram socket
	MFSockType_Raw,			// raw-protocol interface
	MFSockType_RDMessage,	// reliably-delivered message
	MFSockType_SeqPacket,	// sequenced packet stream

	MFSockType_Max,
	MFSockType_ForceInt = 0x7FFFFFFF
};

enum MFSocketProtocol
{
	MFProtocol_Unknown = -1,

	MFProtocol_IP = 0,			// dummy for IP / IPv6 hop-by-hop options

	MFProtocol_ICMP = 1,		// control message protocol
	MFProtocol_IGMP = 2,		// internet group management protocol
	MFProtocol_IPV4 = 4,		// IPv4
	MFProtocol_TCP = 6,			// tcp
	MFProtocol_PUP = 12,		// pup
	MFProtocol_UDP = 17,		// user datagram protocol
	MFProtocol_IDP = 22,		// xns idp
	MFProtocol_IPV6 = 41,		// IPv6
	MFProtocol_ROUTING = 43,	// IPv6 routing header
	MFProtocol_FRAGMENT = 44,	// IPv6 fragmentation header
	MFProtocol_ESP = 50,		// IPsec ESP header
	MFProtocol_AH = 51,			// IPsec AH
	MFProtocol_ICMPV6 = 58,		// ICMPv6
	MFProtocol_NONE = 59,		// IPv6 no next header
	MFProtocol_DSTOPTS = 60,	// IPv6 destination options

	MFProtocol_Raw = 255,		// raw IP packet

	MFProtocol_Max = 256,
	MFProtocol_ForceInt = 0x7FFFFFFF
};

enum MFSocketOptions
{
	MFSO_NonBlocking,

	MFSO_Max,
	MFSO_ForceInt = 0x7FFFFFFF
};


// MFSockets related structures
struct MFInetAddress
{
	union
	{
		struct
		{
			uint8 b1, b2, b3, b4;
		};

		uint32 address;
	};
};

struct MFInet6Address
{
	union
	{
		uint16  s[8];
		uint128 address;
	};
};

struct MFSocketAddress
{
	int cbSize;
	MFAddressFamily family;
};

struct MFSocketAddressInet : public MFSocketAddress
{
	int port;
	MFInetAddress address;
};

struct MFSocketAddressInet6 : public MFSocketAddress
{
	int port;
	int flowInfo;
	MFInet6Address address;
};

struct MFAddressInfo
{
	uint32 flags;
	MFAddressFamily family;
	MFSocketType type;
	MFSocketProtocol protocol;
	const char *pCanonName;
	const MFSocketAddress *pAddress;
	MFAddressInfo *pNext;
};

// helper functions
MFInetAddress MFSockets_MakeInetAddress(uint8 b1, uint8 b2, uint8 b3, uint8 b4);
MFInetAddress MFSockets_MakeInetAddrssFromString(const char *pIPString);
MFInet6Address MFSockets_MakeInet6Address(uint16 s1, uint16 s2, uint16 s3, uint16 s4, uint16 s5, uint16 s6, uint16 s7, uint16 s8);
MFInet6Address MFSockets_MakeInet6AddrssFromString(const char *pIP6String);

const char* MFSockets_GetAddressString(const MFSocketAddress &address);
const char* MFSockets_GetInetAddressString(const MFSocketAddress &address);
const char* MFSockets_GetInet6AddressString(const MFSocketAddress &address);

// MFSockets interface functions
MFSocket MFSockets_CreateSocket(MFAddressFamily addressFamily, MFSocketType type, int protocol);
int MFSockets_CloseSocket(MFSocket socket);

int MFSockets_Bind(MFSocket socket, const MFSocketAddress &address);

int MFSockets_Connect(MFSocket socket, const MFSocketAddress &address);

int MFSockets_Listen(MFSocket socket, int backlog);
MFSocket MFSockets_Accept(MFSocket socket, MFSocketAddress *pConnectingSocketAddress);

int MFSockets_Send(MFSocket socket, const char *pBuffer, int bufferLength, uint32 flags);
int MFSockets_SendTo(MFSocket socket, const char *pBuffer, int bufferLength, uint32 flags, const MFSocketAddress *pAddress);

int MFSockets_Recv(MFSocket socket, char *pBuffer, int bufferSize, uint32 flags);
int MFSockets_RecvFrom(MFSocket socket, char *pBuffer, int bufferSize, uint32 flags, MFSocketAddress *pSenderAddress);

int MFSockets_GetAddressInfo(const char *pAddress, const char *pServiceName, const MFAddressInfo *pHint, MFAddressInfo **ppAddressInfo);

int MFSockets_SetSocketOptions(MFSocket socket, MFSocketOptions option, const void* optval, int optlen);

#endif
