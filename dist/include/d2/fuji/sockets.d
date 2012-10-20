module fuji.socket;

enum MFSOCKET_ERROR = -1;

/**
 * Represents a Fuji socket.
 * Represents a Fuji socket.
 */
alias void* MFSocket;

/**
 * Address Family.
 * Socket Address Family.
 */
enum MFAddressFamily
{
	Unknown = -1,	/**< Unknown Address Family */

	Unspecified = 0,/**< Unspecified Address Family */

	Unix = 1,		/**< Local to host (pipes, portals) */
	Inet = 2,		/**< Internetwork: UDP, TCP, etc. // **** Fuji only supports Inet currently */
	IPX = 6,		/**< IPX protocols: IPX, SPX, etc. */
	NetBios = 17,	/**< NetBios-style addresses */
	Inet6 = 23,		/**< Internetwork Version 6 */
	IRDA = 26,		/**< IrDA */

	Max = 32		/**< Maximum Address Family */
}

/**
 * Socket Type.
 * Socket Type.
 */
enum MFSocketType
{
	Unknown = -1,/**< Unknown socket type */

	Stream = 1,	/**< Stream socket */
	Datagram,	/**< Datagram socket */
	Raw,		/**< Raw-protocol interface */
	RDMessage,	/**< Reliably-delivered message */
	SeqPacket	/**< Sequenced packet stream */
}

/**
 * Socket Protocol.
 * Socket Protocol.
 */
enum MFSocketProtocol
{
	Unknown = -1,	/**< Unknown socket protocol */

	IP = 0,			/**< Dummy for IP / IPv6 hop-by-hop options */

	ICMP = 1,		/**< Control message protocol */
	IGMP = 2,		/**< Internet group management protocol */
	IPV4 = 4,		/**< IPv4 */
	TCP = 6,		/**< TCP */
	PUP = 12,		/**< PUP */
	UDP = 17,		/**< User datagram protocol */
	IDP = 22,		/**< XNS IDP */
	IPV6 = 41,		/**< IPv6 */
	ROUTING = 43,	/**< IPv6 routing header */
	FRAGMENT = 44,	/**< IPv6 fragmentation header */
	ESP = 50,		/**< IPsec ESP header */
	AH = 51,		/**< IPsec AH */
	ICMPV6 = 58,	/**< ICMPv6 */
	NONE = 59,		/**< IPv6 no next header */
	DSTOPTS = 60,	/**< IPv6 destination options */

	Raw = 255		/**< Raw IP packet */
}

/**
 * Socket Options.
 * Socket Options.
 */
enum MFSocketOptions
{
	NonBlocking		/**< Socket is non-blocking */
}

/**
 * Socket errors.
 * Socket errors.
 */
enum MFSocketError
{
	Unknown = -1,

	None = 0,
	WouldBlock,
	TimedOut,
	Interrupted,
	AlreadyInProgress,
	NetworkUnreachable,
	HostUnreachable,
	ConnectionRefused,
	IsConnected
}

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
			ubyte b1;	/**< First address byte (xx.0.0.0) */
			ubyte b2;	/**< Second address byte (0.xx.0.0) */
			ubyte b3;	/**< Third address byte (0.0.xx.0) */
			ubyte b4;	/**< Fourth address byte (0.0.0.xx) */
		}

		uint address;	/**< Address as a 32 bit value */
	}
}

/**
 * Inet6 Address.
 * Represents an Inet6 Address.
 */
struct MFInet6Address
{
	ushort s[8];	/**< 8 uint16 address components */
}

/**
 * Socket Address.
 * Represents a socket address. This is the base structure that is included in each derived socket address structure for each addressing family.
 */
struct MFSocketAddress
{
	int cbSize = typeof(this).sizeof;						/**< Sive of the structure */
	MFAddressFamily family = MFAddressFamily.Unspecified;	/**< Address family for the socket address */
}

/**
 * Inet Socket Address.
 * Represents an Inet socket address. This structure is derived from MFSocketAddress.
 */
struct MFSocketAddressInet
{
	MFSocketAddress base = MFSocketAddress(typeof(this).sizeof, MFAddressFamily.Inet);
	alias base this;

	int port;				/**< Internet Protocol (IP) port. */
	MFInetAddress address;	/**< IP address in network byte order. */
}

/**
 * Inet6 Socket Address.
 * Represents an Inet6 socket address. This structure is derived from MFSocketAddress.
 */
struct MFSocketAddressInet6
{
	MFSocketAddress base = MFSocketAddress(typeof(this).sizeof, MFAddressFamily.Inet6);
	alias base this;

	int port;				/**< Transport-level port number */
	uint flowInfo;			/**< Ipv6 flow information */
	MFInet6Address address;	/**< IPv6 address */
	uint scopeId;			/**< IPv6 scope ID */
}

/**
 * Address information.
 * The MFAddressInfo structure is used by MFSockets_GetAddressInfo() to hold host address information.
 */
struct MFAddressInfo
{
	uint flags;					/**< Flags that indicate options used in the getaddrinfo function. See AI_PASSIVE, AI_CANONNAME, and AI_NUMERICHOST. */
	MFAddressFamily family;		/**< Protocol family, such as Inet. */
	MFSocketType type;			/**< Socket type, such as Raw, Stream, or Datagram. */
	MFSocketProtocol protocol;	/**< Protocol, such as TCP or UDP. For protocols other than IPv4 and IPv6, set ai_protocol to zero. */
	const(char*) pCanonName;	/**< Canonical name for the host. */
	const(MFSocketAddress*) pAddress;	/**< Pointer to an MFSocketAddress structure. */

	MFAddressInfo* pNext;		/**< Pointer to the next structure in a linked list. This parameter is set to NULL in the last MFAddressInfo structure of a linked list. */
}

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
extern (C) MFInetAddress MFSockets_MakeInetAddress(ubyte b1, ubyte b2, ubyte b3, ubyte b4);

/**
 * Build an MFInetAddress structure from a string.
 * Build an MFInetAddress structure from a string.
 * @param pIPString String representing the IP address in "0.0.0.0" format.
 * @return Returns an MFInetAddress structure representing the IP address specified by the string.
 */
extern (C) MFInetAddress MFSockets_MakeInetAddressFromString(const(char*) pIPString);

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
extern (C) MFInet6Address MFSockets_MakeInet6Address(ushort s1, ushort s2, ushort s3, ushort s4, ushort s5, ushort s6, ushort s7, ushort s8);

/**
 * Build an MFInet6Address structure from a string.
 * Build an MFInet6Address structure from a string.
 * @param pIP6String String representing the IP6 address in "0000:0000:0000:0000:0000:0000:0000:0000" format.
 * @return Returns an MFInet6Address structure representing the IP6 address specified by the string.
 */
extern (C) MFInet6Address MFSockets_MakeInet6AddressFromString(const(char*) pIP6String);

/**
 * Generate a string representing the specified network address.
 * Generate a string representing the specified network address.
 * @param address An MFSocketAddress structure specifying a network address. This address may be from any supported address family.
 * @return Returns a string representing the specified network address.
 */
extern (C) const(char*) MFSockets_GetAddressString(ref const(MFSocketAddress) address);

// MFSockets interface functions

/**
 * Check if the Fuji socket layer is active.
 * Check if the Fuji socket layer is active.
 * @return Returns true if the Fuji socket layer is active and ready for use.
 */
extern (C) bool MFSockets_IsActive();

/**
 * Create a socket.
 * Creates a socket that is bound to a specific service provider.
 * @param addressFamily Address family specification.
 * @param type Type specification for the new socket. This should be a member of the MFSocketType enumerated type.
 * @param protocol Protocol to be used with the socket that is specific to the indicated address family.
 * @return Returns a reference to the newly created socket. If CreateSocket failed, the return value is -1.
 */
extern (C) MFSocket MFSockets_CreateSocket(MFAddressFamily addressFamily, MFSocketType type, int protocol);

/**
 * Close a socket.
 * Closes an existing socket.
 * @param socket The socket to close.
 * @return If no error occurs, 0 is returnd.
 */
extern (C) int MFSockets_CloseSocket(MFSocket socket);

/**
 * Associate a local address with a socket.
 * Associates a local address with a socket.
 * @param socket Descriptor identifying an unbound socket.
 * @param address Address to assign to the socket.
 * @return If no error occurs, 0 is returnd.
 */
extern (C) int MFSockets_Bind(MFSocket socket, ref const(MFSocketAddress) address);

/**
 * Connect to a remote socket.
 * Establishes a connection to a specified socket.
 * @param socket Descriptor identifying an unconnected socket.
 * @param address Addres of socket to which the connection should be established.
 * @return If no error occurs, 0 is returned.
 */
extern (C) int MFSockets_Connect(MFSocket socket, ref const(MFSocketAddress) address);

/**
 * Listen for an incoming connection.
 * Places a socket in a state in which it is listening for an incoming connection.
 * @param socket Descriptor identifying a bound, unconnected socket.
 * @param backlog Maximum length of the queue of pending connections. If set to SOMAXCONN, the underlying service provider responsible for \a socket will set the backlog to a maximum reasonable value.
 * @return If no error occurs, 0 is returnd.
 */
extern (C) int MFSockets_Listen(MFSocket socket, int backlog);

/**
 * Accept an incoming connection request.
 * Permits an incoming connection attempt on a socket.
 * @param socket Descriptor identifying a socket that has been placed in a listening state with the listen function. The connection is actually made with the socket that is returned by MFSockets_Accept().
 * @param pConnectingSocketAddress Optional pointer to a buffer that receives the address of the connecting entity, as known to the communications layer. The exact format of the \a pConnectingSocketAddress parameter is determined by the address family that was established when the socket from the MFSocketAddress structure was created.
 * @return If no error occurs, MFSockets_Accept() returns a value of type MFSocket that is a descriptor for the new socket. This returned value is a handle for the socket on which the actual connection is made.
 */
extern (C) MFSocket MFSockets_Accept(MFSocket socket, MFSocketAddress* pConnectingSocketAddress);

/**
 * Send data on a connected socket.
 * Sends data on a connected socket.
 * @param socket Descriptor identifying a connected socket.
 * @param pBuffer Buffer containing the data to be transmitted.
 * @param bufferLength Length of the data in \a pBuffer, in bytes.
 * @param flags Indicator specifying the way in which the call is made.
 * @return Returns the total number of bytes sent, which can be less than the number indicated by \a bufferLength.
 */
extern (C) int MFSockets_Send(MFSocket socket, const(char*) pBuffer, int bufferLength, uint flags);

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
extern (C) int MFSockets_SendTo(MFSocket socket, const(char*) pBuffer, int bufferLength, uint flags, const(MFSocketAddress*) pAddress);

/**
 * Receive data from a connected or bound socket.
 * Receives data from a connected or bound socket.
 * @param socket Descriptor identifying a connected socket.
 * @param pBuffer Buffer for the incoming data.
 * @param bufferSize Length of \a pBuffer, in bytes.
 * @param flags Flag specifying the way in which the call is made.
 * @return Returns the number of bytes received. If the connection has been gracefully closed, the return value is zero.
 */
extern (C) int MFSockets_Recv(MFSocket socket, char* pBuffer, int bufferSize, uint flags);

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
extern (C) int MFSockets_RecvFrom(MFSocket socket, char* pBuffer, int bufferSize, uint flags, MFSocketAddress* pSenderAddress);

/**
 * Get the address info for a network address.
 * Provides protocol-independent translation from host name to address.
 * @param pAddress String containing a host (node) name or a numeric host address string. The numeric host address string is a dotted-decimal IPv4 address or an IPv6 hex address.
 * @param pServiceName String containing either a service name or port number.
 * @param pHint Pointer to an MFAddressInfo structure that provides hints about the type of socket the caller supports.
 * @param ppAddressInfo Pointer to a linked list of one or more MFAddressInfo structures containing response information about the host.
 * @return Success returns zero. Failure returns a nonzero error code.
 */
extern (C) int MFSockets_GetAddressInfo(const(char*) pAddress, const(char*) pServiceName, const(MFAddressInfo*) pHint, MFAddressInfo** ppAddressInfo);

/**
 * Set a socket option.
 * Sets a socket option.
 * @param socket Descriptor identifying a socket.
 * @param option Socket option for which the value is to be set.
 * @param optval Pointer to the buffer in which the value for the requested option is specified.
 * @param optlen Size of the optval buffer, in bytes.
 * @return If no error occurs, zero is returned.
 */
extern (C) int MFSockets_SetSocketOptions(MFSocket socket, MFSocketOptions option, const(void*) optval, int optlen);

/**
 * Get the last socket error.
 * Gets the last socket error.
 * @return Returns the last socket error.
 */
extern (C) MFSocketError MFSockets_GetLastError();

