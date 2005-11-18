#include "Fuji.h"
#include "MFSockets.h"

MFInetAddress MFSockets_MakeInetAddress(uint8 b1, uint8 b2, uint8 b3, uint8 b4)
{
	MFInetAddress address;

	address.b1 = b1;
	address.b2 = b2;
	address.b3 = b3;
	address.b4 = b4;

	return address;
}

MFInetAddress MFSockets_MakeInetAddrssFromString(const char *pIPString)
{
	MFInetAddress a;

	const char *pToken = strtok(MFStr(pIPString), ".");
	MFDebug_Assert(pToken, "Malformed IP string.");
	a.b1 = (uint8)atoi(pToken);

	pToken = strtok(NULL, ".");
	MFDebug_Assert(pToken, "Malformed IP string.");
	a.b2 = (uint8)atoi(pToken);

	pToken = strtok(NULL, ".");
	MFDebug_Assert(pToken, "Malformed IP string.");
	a.b3 = (uint8)atoi(pToken);

	pToken = strtok(NULL, ".");
	MFDebug_Assert(pToken, "Malformed IP string.");
	a.b4 = (uint8)atoi(pToken);

	return a;
}

MFInet6Address MFSockets_MakeInet6Address(uint16 s1, uint16 s2, uint16 s3, uint16 s4, uint16 s5, uint16 s6, uint16 s7, uint16 s8)
{
	MFInet6Address address;

	address.s[0] = s1;
	address.s[1] = s2;
	address.s[2] = s3;
	address.s[3] = s4;
	address.s[4] = s5;
	address.s[5] = s6;
	address.s[6] = s7;
	address.s[7] = s8;

	return address;
}

MFInet6Address MFSockets_MakeInet6AddrssFromString(const char *pIP6String)
{
	MFInet6Address a;
	memset(&a, 0, sizeof(MFInet6Address));

	MFDebug_Assert(false, "Not written!");
/*
	int numColons = 0;

	while(a=strlen(pIP6String)-1; a--;)
	{
		if(pIP6String[a] == ':')
			numColons++;
	}

	const char *pToken = strtok(pIP6String, ":");

	while(pToken)
	{

		pToken = strtok(NULL, ":");
	}
*/
	return a;
}

const char* MFSockets_GetInetAddressString(const MFSocketAddress &address)
{
	if(address.family != MFAF_Inet)
		return "";

	MFDebug_Assert(address.cbSize == sizeof(MFSocketAddressInet), "Invalid size for MFSocketAddressInet structure");

	MFSocketAddressInet &inet = (MFSocketAddressInet&)address;

	return MFStr("%d.%d.%d.%d", inet.address.b1, inet.address.b2, inet.address.b3, inet.address.b4);
}

const char* MFSockets_GetInet6AddressString(const MFSocketAddress &address)
{
	if(address.family != MFAF_Inet6)
		return "";

	MFDebug_Assert(address.cbSize == sizeof(MFSocketAddressInet6), "Invalid size for MFSocketAddressInet6 structure");

	MFSocketAddressInet6 &inet = (MFSocketAddressInet6&)address;

	return MFStr("%04X:%04X:%04X:%04X:%04X:%04X:%04X:%04X", inet.address.s[0], inet.address.s[1], inet.address.s[2], inet.address.s[3], inet.address.s[4], inet.address.s[5], inet.address.s[6], inet.address.s[7]);
}

const char* MFSockets_GetAddressString(const MFSocketAddress &address)
{
	switch(address.family)
	{
		case MFAF_Inet:
			return MFSockets_GetInetAddressString(address);
		case MFAF_Inet6:
			return MFSockets_GetInet6AddressString(address);
		default:
			break;
	}

	return "";
}
