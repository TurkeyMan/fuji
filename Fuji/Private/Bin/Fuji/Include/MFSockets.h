/**
 * @file MFSockets.h
 * @brief Mount Fuji Engine Sockets implementation.
 * This provides sockets and network access.
 * @author Manu Evans
 * @defgroup MFSockets Networking/Sockets Interface
 * @{
 */

#if !defined(_MFSOCKETS_H)
#define _MFSOCKETS_H

/**
 * Represents a Fuji socket.
 * Represents a Fuji socket.
 */
typedef void* MFSocket;

/**
 * Address Family.
 * Socket Address Family.
 */
enum MFAddressFamily
{
	MFAF_Unknown = -1,	/**< Unknown Address Family */

	MFAF_Unspecified = 0, /**< Unspecified Address Family */

	MFAF_Unix = 1,		/**< Local to host (pipes, portals) */
	MFAF_Inet = 2,		/**< Internetwork: UDP, TCP, etc. // **** Fuji only supports Inet currently */
	MFAF_IPX = 6,		/**< IPX protocols: IPX, SPX, etc. */
	MFAF_NetBios = 17,	/**< NetBios-style addresses */
	MFAF_Inet6 = 23,	/**< Internetwork Version 6 */
	MFAF_IRDA = 26,		/**< IrDA */

	MFAF_Max = 32,		/**< Maximum Address Family */
	MFAF_ForceInt = 0x7FFFFFFF	/**< Force AddressFamily to an int type */
};

/**
 * Socket Type.
 * Socket Type.
 */
enum MFSocketType
{
	MFSockType_Unknown = -1,/**< Unknown socket type */

	MFSockType_Stream = 1,	/**< Stream socket */
	MFSockType_Datagram,	/**< Datagram socket */
	MFSockType_Raw,			/**< Raw-protocol interface */
	MFSockType_RDMessage,	/**< Reliably-delivered message */
	MFSockType_SeqPacket,	/**< Sequenced packet stream */

	MFSockType_Max,			/**< Max socket type */
	MFSockType_ForceInt = 0x7FFFFFFF	/**< Force SocketType to int type */
};

/**
 * Socket Protocol.
 * Socket Protocol.
 */
enum MFSocketProtocol
{
	MFProtocol_Unknown = -1,	/**< Unknown socket protocol */

	MFProtocol_IP = 0,			/**< Dummy for IP / IPv6 hop-by-hop options */

	MFProtocol_ICMP = 1,		/**< Control message protocol */
	MFProtocol_IGMP = 2,		/**< Internet group management protocol */
	MFProtocol_IPV4 = 4,		/**< IPv4 */
	MFProtocol_TCP = 6,			/**< TCP */
	MFProtocol_PUP = 12,		/**< PUP */
	MFProtocol_UDP = 17,		/**< User datagram protocol */
	MFProtocol_IDP = 22,		/**< XNS IDP */
	MFProtocol_IPV6 = 41,		/**< IPv6 */
	MFProtocol_ROUTING = 43,	/**< IPv6 routing header */
	MFProtocol_FRAGMENT = 44,	/**< IPv6 fragmentation header */
	MFProtocol_ESP = 50,		/**< IPsec ESP header */
	MFProtocol_AH = 51,			/**< IPsec AH */
	MFProtocol_ICMPV6 = 58,		/**< ICMPv6 */
	MFProtocol_NONE = 59,		/**< IPv6 no next header */
	MFProtocol_DSTOPTS = 60,	/**< IPv6 destination options */

	MFProtocol_Raw = 255,		/**< Raw IP packet */

	MFProtocol_Max = 256,		/**< Maximum socket protocol */
	MFProtocol_ForceInt = 0x7FFFFFFF	/**< Force SocketProtocol to an int type */
};

/**
 * Socket Options.
 * Socket Options.
 */
enum MFSocketOptions
{
	MFSO_NonBlocking,	/**< Socket is non-blocking */

	MFSO_Max,			/**< Maximum socket option */
	MFSO_ForceInt = 0x7FFFFFFF	/**< Force SocketOptions to int type */
};


/**
 * Inet Address.
 * Represents an Inet Address.
 */
struct MFInetAddress
{
	union
	{
		struct
		{
			uint8 b1;	/**< First address byte (xx.0.0.0) */
			uint8 b2;	/**< Second address byte (0.xx.0.0) */
			uint8 b3;	/**< Third address byte (0.0.xx.0) */
			uint8 b4;	/**< Fourth address byte (0.0.0.xx) */
		};

		uint32 address;	/**< Address as a 32 bit value */
	};
};

/**
 * Inet6 Address.
 * Represents an Inet6 Address.
 */
struct MFInet6Address
{
	union
	{
		uint16  s[8];	/**< 8 uint16 address components */
		uint128 address;/**< Address as a 128bit value */
	};
};

/**
 * Socket Address.
 * Represents a socket address. This is the base structure that is included in each derived socket address structure for each addressing family.
 */
struct MFSocketAddress
{
	int cbSize;				/**< Sive of the structure */
	MFAddressFamily family;	/**< Address family for the socket address */
};

/**
 * Inet Socket Address.
 * Represents an Inet socket address. This structure is derived from MFSocketAddress.
 */
struct MFSocketAddressInet : public MFSocketAddress
{
	int port;				/**< Internet Protocol (IP) port. */
	MFInetAddress address;	/**< IP address in network byte order. */
};

/**
 * Inet6 Socket Address.
 * Represents an Inet6 socket address. This structure is derived from MFSocketAddress.
 */
struct MFSocketAddressInet6 : public MFSocketAddress
{
	int port;				/**< Transport-level port number */
	int flowInfo;			/**< Ipv6 flow information */
	MFInet6Address address;	/**< IPv6 address */
};

/**
 * Address information.
 * The MFAddressInfo structure is used by MFSockets_GetAddressInfo() to hold host address information.
 */
struct MFAddressInfo
{
	uint32 flags;				/**< Flags that indicate options used in the getaddrinfo function. See AI_PASSIVE, AI_CANONNAME, and AI_NUMERICHOST. */
	MFAddressFamily family;		/**< Protocol family, such as MFAF_Inet. */
	MFSocketType type;			/**< Socket type, such as MFSockType_Raw, MFSockType_Stream, or MFSockType_Datagram. */
	MFSocketProtocol protocol;	/**< Protocol, such as MFProtocol_TCP or MFProtocol_UDP. For protocols other than IPv4 and IPv6, set ai_protocol to zero. */
	const char *pCanonName;		/**< Canonical name for the host. */
	const MFSocketAddress *pAddress;	/**< Pointer to an MFSocketAddress structure. */

	MFAddressInfo *pNext;		/**< Pointer to the next structure in a linked list. This parameter is set to NULL in the last MFAddressInfo structure of a linked list. */
};

// helper functions

/**
 * Build an MFInetAddress structure from 4 byte values.
 * Build an MFInetAddress structure from 4 byte values.
 * @param b1 First byte.
 * @param b2 Second byte.
 * @param b3 Third byte.
 * @param b4 Fourth byte.
 * @return Returns an MFInetAddress structure representing the IP address specified by the 4 bytes.
 */
MFInetAddress MFSockets_MakeInetAddress(uint8 b1, uint8 b2, uint8 b3, uint8 b4);

/**
 * Build an MFInetAddress structure from a string.
 * Build an MFInetAddress structure from a string.
 * @param pIPString String representing the IP address in "0.0.0.0" format.
 * @return Returns an MFInetAddress structure representing the IP address specified by the string.
 */
MFInetAddress MFSockets_MakeInetAddressFromString(const char *pIPString);

/**
 * Build an MFInet6Address structure from 8 unsigned short values.
 * Build an MFInet6Address structure from 8 unsigned short values.
 * @param s1 First short.
 * @param s2 Second short.
 * @param s3 Third short.
 * @param s4 Fourth short.
 * @param s5 Fifth short.
 * @param s6 Sixth short.
 * @param s7 Seventh short.
 * @param s8 Eighth short.
 * @return Returns an MFInet6Address structure representing the IP6 address specified by the 8 shorts.
 */
MFInet6Address MFSockets_MakeInet6Address(uint16 s1, uint16 s2, uint16 s3, uint16 s4, uint16 s5, uint16 s6, uint16 s7, uint16 s8);

/**
 * Build an MFInet6Address structure from a string.
 * Build an MFInet6Address structure from a string.
 * @param pIP6String String representing the IP6 address in "0000:0000:0000:0000:0000:0000:0000:0000" format.
 * @return Returns an MFInet6Address structure representing the IP6 address specified by the string.
 */
MFInet6Address MFSockets_MakeInet6AddressFromString(const char *pIP6String);

/**
 * Generate a string representing the specified network address.
 * Generate a string representing the specified network address.
 * @param address An MFSocketAddress structure specifying a network address. This address may be from any supported address family.
 * @return Returns a string representing the specified network address.
 */
const char* MFSockets_GetAddressString(const MFSocketAddress &address);

// MFSockets interface functions

/**
 * Check if the Fuji socket layer is active.
 * Check if the Fuji socket layer is active.
 * @return Returns true if the Fuji socket layer is active and ready for use.
 */
bool MFSockets_IsActive();

/**
 * Create a socket.
 * Creates a socket that is bound to a specific service provider.
 * @param addressFamily Address family specification.
 * @param type Type specification for the new socket. This should be a member of the MFSocketType enumerated type.
 * @param protocol Protocol to be used with the socket that is specific to the indicated address family.
 * @return Returns a reference to the newly created socket. If CreateSocket failed, the return value is -1.
 */
MFSocket MFSockets_CreateSocket(MFAddressFamily addressFamily, MFSocketType type, int protocol);

/**
 * Close a socket.
 * Closes an existing socket.
 * @param socket The socket to close.
 * @return If no error occurs, 0 is returnd.
 */
int MFSockets_CloseSocket(MFSocket socket);

/**
 * Associate a local address with a socket.
 * Associates a local address with a socket.
 * @param socket Descriptor identifying an unbound socket.
 * @param address Address to assign to the socket.
 * @return If no error occurs, 0 is returnd.
 */
int MFSockets_Bind(MFSocket socket, const MFSocketAddress &address);

/**
 * Connect to a remote socket.
 * Establishes a connection to a specified socket.
 * @param socket Descriptor identifying an unconnected socket.
 * @param address Addres of socket to which the connection should be established.
 * @return If no error occurs, 0 is returned.
 */
int MFSockets_Connect(MFSocket socket, const MFSocketAddress &address);

/**
 * Listen for an incoming connection.
 * Places a socket in a state in which it is listening for an incoming connection.
 * @param socket Descriptor identifying a bound, unconnected socket.
 * @param backlog Maximum length of the queue of pending connections. If set to SOMAXCONN, the underlying service provider responsible for \a socket will set the backlog to a maximum reasonable value.
 * @return If no error occurs, 0 is returnd.
 */
int MFSockets_Listen(MFSocket socket, int backlog);

/**
 * Accept an incoming connection request.
 * Permits an incoming connection attempt on a socket.
 * @param socket Descriptor identifying a socket that has been placed in a listening state with the listen function. The connection is actually made with the socket that is returned by MFSockets_Accept().
 * @param pConnectingSocketAddress Optional pointer to a buffer that receives the address of the connecting entity, as known to the communications layer. The exact format of the \a pConnectingSocketAddress parameter is determined by the address family that was established when the socket from the MFSocketAddress structure was created.
 * @return If no error occurs, MFSockets_Accept() returns a value of type MFSocket that is a descriptor for the new socket. This returned value is a handle for the socket on which the actual connection is made.
 */
MFSocket MFSockets_Accept(MFSocket socket, MFSocketAddress *pConnectingSocketAddress);

/**
 * Send data on a connected socket.
 * Sends data on a connected socket.
 * @param socket Descriptor identifying a connected socket.
 * @param pBuffer Buffer containing the data to be transmitted.
 * @param bufferLength Length of the data in \a pBuffer, in bytes.
 * @param flags Indicator specifying the way in which the call is made.
 * @return Returns the total number of bytes sent, which can be less than the number indicated by \a bufferLength.
 */
int MFSockets_Send(MFSocket socket, const char *pBuffer, int bufferLength, uint32 flags);

/**
 * Send data to a specific destination.
 * Sends data to a specific destination.
 * @param socket Descriptor identifying a (possibly connected) socket.
 * @param pBuffer Buffer containing the data to be transmitted.
 * @param bufferLength Length of the data in \a pBuffer, in bytes.
 * @param flags Indicator specifying the way in which the call is made.
 * @param pAddress Optional pointer to a MFSocketAddress structure that contains the address of the target socket.
 * @return Returns the total number of bytes sent, which can be less than the number indicated by \a bufferLength.
 */
int MFSockets_SendTo(MFSocket socket, const char *pBuffer, int bufferLength, uint32 flags, const MFSocketAddress *pAddress);

/**
 * Receive data from a connected or bound socket.
 * Receives data from a connected or bound socket.
 * @param socket Descriptor identifying a connected socket.
 * @param pBuffer Buffer for the incoming data.
 * @param bufferSize Length of \a pBuffer, in bytes.
 * @param flags Flag specifying the way in which the call is made.
 * @return Returns the number of bytes received. If the connection has been gracefully closed, the return value is zero.
 */
int MFSockets_Recv(MFSocket socket, char *pBuffer, int bufferSize, uint32 flags);

/**
 * Receive a datagram and stores the source address.
 * Receives a datagram and stores the source address.
 * @param socket Descriptor identifying a bound socket.
 * @param pBuffer Buffer for the incoming data.
 * @param bufferSize Length of \a pBuffer, in bytes.
 * @param flags Flag specifying the way in which the call is made.
 * @param pSenderAddress Optional pointer to a buffer in a MFSocketAddress structure that will hold the source address upon return.
 * @return Returns the number of bytes received. If the connection has been gracefully closed, the return value is zero.
 */
int MFSockets_RecvFrom(MFSocket socket, char *pBuffer, int bufferSize, uint32 flags, MFSocketAddress *pSenderAddress);

/**
 * Get the address info for a network address.
 * Provides protocol-independent translation from host name to address.
 * @param pAddress String containing a host (node) name or a numeric host address string. The numeric host address string is a dotted-decimal IPv4 address or an IPv6 hex address.
 * @param pServiceName String containing either a service name or port number.
 * @param pHint Pointer to an MFAddressInfo structure that provides hints about the type of socket the caller supports.
 * @param ppAddressInfo Pointer to a linked list of one or more MFAddressInfo structures containing response information about the host.
 * @return Success returns zero. Failure returns a nonzero error code.
 */
int MFSockets_GetAddressInfo(const char *pAddress, const char *pServiceName, const MFAddressInfo *pHint, MFAddressInfo **ppAddressInfo);

/**
 * Set a socket option.
 * Sets a socket option.
 * @param socket Descriptor identifying a socket.
 * @param option Socket option for which the value is to be set.
 * @param optval Pointer to the buffer in which the value for the requested option is specified.
 * @param optlen Size of the optval buffer, in bytes.
 * @return If no error occurs, zero is returned.
 */
int MFSockets_SetSocketOptions(MFSocket socket, MFSocketOptions option, const void* optval, int optlen);

#endif

/** @} */
