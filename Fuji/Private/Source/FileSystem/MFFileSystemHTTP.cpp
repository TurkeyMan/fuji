#include "Fuji.h"
#include "MFSystem.h"
#include "MFSockets.h"
#include "MFFileSystem_Internal.h"
#include "FileSystem/MFFileSystemHTTP_Internal.h"
#include "MFPtrList.h"

#include <stdio.h>

static MFPtrListDL<MFFileHTTPData> gHTTPFiles;
 
MFInitStatus MFFileSystemHTTP_InitModule()
{
	if(MFSockets_IsActive())
	{
		MFFileSystemCallbacks fsCallbacks;

		fsCallbacks.RegisterFS = NULL;
		fsCallbacks.UnregisterFS = NULL;
		fsCallbacks.FSMount = MFFileSystemHTTP_Mount;
		fsCallbacks.FSDismount = MFFileSystemHTTP_Dismount;
		fsCallbacks.FSOpen = MFFileSystemHTTP_Open;
		fsCallbacks.Open = MFFileHTTP_Open;
		fsCallbacks.Close = MFFileHTTP_Close;
		fsCallbacks.Read = MFFileHTTP_Read;
		fsCallbacks.Write = MFFileHTTP_Write;
		fsCallbacks.Seek = MFFileHTTP_Seek;
		fsCallbacks.FindFirst = NULL;
		fsCallbacks.FindNext = NULL;
		fsCallbacks.FindClose = NULL;

		hHTTPFileSystem = MFFileSystem_RegisterFileSystem("HTTP Filesystem", &fsCallbacks);

		// make a bunch of file buffers
		int gHTTPFileSize = sizeof(MFFileHTTPData)+gDefaults.filesys.maxHTTPFileCache;

		gHTTPFiles.Init("HTTP Files", gDefaults.filesys.maxHTTPFiles, gHTTPFileSize);
	}

	return MFAIC_Succeeded;
}

void MFFileSystemHTTP_DeinitModule()
{
	gHTTPFiles.Deinit();

	if(hHTTPFileSystem != -1)
		MFFileSystem_UnregisterFileSystem(hHTTPFileSystem);
}
// filesystem callbacks
int MFFileSystemHTTP_Mount(MFMount *pMount, MFMountData *pMountData)
{
	MFDebug_Assert(pMountData->cbSize == sizeof(MFMountDataHTTP), "Incorrect size for MFMountDataHTTP structure. Invalid pMountData.");
	MFDebug_Assert(pMount->volumeInfo.flags & MFMF_OnlyAllowExclusiveAccess, "HTTP Filesystem can not generate a directory list. The MFMF_OnlyAllowExclusiveAccess flag MUST be specified for HTTP mounts.");

//	MFMountDataHTTP *pMountHTTP = (MFMountDataHTTP*)pMountData;

	// HTTP has no way to evaluate the directory structire.
	pMount->volumeInfo.flags |= MFMF_DontCacheTOC;

	return 0;
}

int MFFileSystemHTTP_Dismount(MFMount *pMount)
{
	return 0;
}

MFFile* MFFileSystemHTTP_Open(MFMount *pMount, const char *pFilename, uint32 openFlags)
{
	MFFile *hFile = NULL;

	MFOpenDataHTTP openData;

	openData.cbSize = sizeof(MFOpenDataHTTP);
	openData.openFlags = openFlags;

	if(pFilename[0] == '/' && pFilename[1] == '/')
	{
		pFilename = MFStr("http:%s", pFilename);
	}

	openData.pURL = pFilename;
	openData.port = 80;

	hFile = MFFile_Open(hHTTPFileSystem, &openData);

	return hFile;
}

void MFFileHTTP_GetServerAndPath(const char *pURL, char **ppServer, char **ppPath)
{
	char *pServer = (char*)MFStr(pURL);

	// get the server name
	if(!MFString_CaseCmpN(pURL, "http://", 7))
	{
		pServer += 7;
	}

	char *pPath = pServer;

	while(*pPath != '/' && *pPath != 0)
		++pPath;

	if(*pPath == '/')
	{
		*pPath = 0;
		++pPath;
		pPath = (char*)MFStr("/%s", pPath);
	}
	else
		pPath = (char*)"/";

	*ppServer = pServer;
	*ppPath = pPath;
}

bool MFFileHTTP_RequestHeader(const MFSocketAddress *pAddr, const char *pHost, int port, const char *pPath, char *pOutputBuffer, int maxSize)
{
	const char *pHeaderRequest = MFStr("HEAD %s HTTP/1.1\nFrom: mtfuji@dotblip.com\nUser-Agent: Mount Fuji Engine/1.0\nHost: %s:%d\n\n", pPath, pHost, port);

	MFSocket socket = MFSockets_CreateSocket(MFAF_Inet, MFSockType_Stream, MFProtocol_TCP);

	MFDebug_Warn(4, MFStr("Connecting to: '%s'", MFSockets_GetAddressString(*pAddr)));

	if(MFSockets_Connect(socket, *pAddr))
		return true;

	MFSockets_Send(socket, pHeaderRequest, MFString_Length(pHeaderRequest) + 1, 0);

	int lastChar;
	lastChar = MFSockets_Recv(socket, pOutputBuffer, maxSize-1, 0);
	pOutputBuffer[lastChar] = 0;

	MFSockets_CloseSocket(socket);

	return false;
}

int MFFileHTTP_Open(MFFile *pFile, MFOpenData *pOpenData)
{
	MFCALLSTACK;

	MFDebug_Assert(pOpenData->cbSize == sizeof(MFOpenDataHTTP), "Incorrect size for MFOpenDataHTTP structure. Invalid pOpenData.");
	MFOpenDataHTTP *pHTTP = (MFOpenDataHTTP*)pOpenData;

	// try and connect and get the header..

	char *pServer;
	char *pPath;

	MFFileHTTP_GetServerAndPath(pHTTP->pURL, &pServer, &pPath);

	// resolve this address..
	MFAddressInfo addrInfo, *pAddrInfo;
	MFZeroMemory(&addrInfo, sizeof(MFAddressInfo));
	addrInfo.family = MFAF_Inet;
	addrInfo.type = MFSockType_Stream;
	addrInfo.protocol = MFProtocol_TCP;

	MFSockets_GetAddressInfo(pServer, MFStr("%d", pHTTP->port), &addrInfo, &pAddrInfo);

	char headerBuffer[2048];
	bool foundServer = false;
	for(; pAddrInfo; pAddrInfo = pAddrInfo->pNext)
	{
		if(MFFileHTTP_RequestHeader(pAddrInfo->pAddress, pServer, pHTTP->port, pPath, headerBuffer, 2048))
			continue;

		if(!MFString_CaseCmpN(headerBuffer, "HTTP", 4))
		{
			const char *pErrorCode = MFStrN(headerBuffer + 9, 3);
			int errorCode = atoi(pErrorCode);

			if(errorCode == 200)
			{
				// success..
				foundServer = true;
				break;
			}
			else if(errorCode >= 300 && errorCode < 400)
			{
				// server has redirected us
				const char *pNewLocation = NULL;
				const char *pNextLine = headerBuffer;

				while(!pNewLocation && pNextLine)
				{
					pNextLine = MFSeekNewline(pNextLine);

					if(!MFString_CaseCmpN(pNextLine, "Location:", 9))
					{
						// we have found the location line
						pNewLocation = pNextLine + 10;

						int a=0;
						while(pNewLocation[a] && pNewLocation[a] != '\n' && pNewLocation[a] != '\r')
							++a;

						pNewLocation = MFStrN(pNewLocation, a);

						MFFileHTTP_GetServerAndPath(pNewLocation, &pServer, &pPath);

						MFDebug_Warn(4, MFStr("HTTP response code %d: Redirecting to '%s'", errorCode, pNewLocation));
					}
				}
			}
			else if(errorCode == 404)
				MFDebug_Warn(2, MFStr("HTTP response code 404: Not Found", errorCode));
			else if(errorCode == 400)
				MFDebug_Warn(2, MFStr("HTTP response code 400: Bad Request", errorCode));
			else if(errorCode == 401)
				MFDebug_Warn(2, MFStr("HTTP response code 401: Unauthorized", errorCode));
			else if(errorCode == 403)
				MFDebug_Warn(2, MFStr("HTTP response code 403: Forbidden", errorCode));
			else if(errorCode >= 100 && errorCode < 200)
			{
				// continue request
			}
			else if(errorCode > 200 && errorCode < 300)
			{

				// success or something
			}
			else
				MFDebug_Warn(2, MFStr("HTTP server has returned an unknown response code %d", errorCode));
		}
		else
			MFDebug_Warn(2, "Invalid HTTP response, server is likely NOT a HTTP server...");
	}

	if(!foundServer)
	{
		MFDebug_Warn(2, MFStr("Couldnt locate host: '%s' on port %d", pServer, pHTTP->port));
		return 1;
	}

	MFFileHTTPData *pHTTPData = gHTTPFiles.Create();
	pFile->pFilesysData = pHTTPData;

	int serverLen = MFString_Length(pServer) + 1;
	int pathLen = MFString_Length(pPath) + 1;
	int argLen = 0;

	MFCopyMemory(&pHTTPData->address, pAddrInfo->pAddress, sizeof(MFSocketAddressInet));
	pHTTPData->pServer = (char*)&pHTTPData[1];
	pHTTPData->pPath = pHTTPData->pServer + serverLen;
	pHTTPData->port = pHTTP->port;

	// calculate args
	pHTTPData->pArgString = NULL;

	if(pHTTP->pPostArgs)
	{
		pHTTPData->pArgString = pHTTPData->pPath + pathLen;

		// build the string from the supplied args
		for(int a=0; a<pHTTP->numPostArgs; ++a)
		{
			switch(pHTTP->pPostArgs[a].type)
			{
				case MFFileHTTPRequestArg::AT_Int:
					argLen += sprintf(pHTTPData->pArgString + argLen, "%s%s=%d", argLen ? "&" : "", pHTTP->pPostArgs[a].pArg, pHTTP->pPostArgs[a].iValue);
					break;
				case MFFileHTTPRequestArg::AT_Float:
					argLen += sprintf(pHTTPData->pArgString + argLen, "%s%s=%g", argLen ? "&" : "", pHTTP->pPostArgs[a].pArg, pHTTP->pPostArgs[a].fValue);
					break;
				case MFFileHTTPRequestArg::AT_String:
					argLen += sprintf(pHTTPData->pArgString + argLen, "%s%s=%s", argLen ? "&" : "", pHTTP->pPostArgs[a].pArg, pHTTP->pPostArgs[a].pValue);
					break;
			}
		}

		// URL encode the arg string
		const char *pEncoded = MFStr_URLEncodeString(pHTTPData->pArgString);

		// and copy the encoded version into the buffer
		MFString_Copy(pHTTPData->pArgString, pEncoded);
		argLen = MFString_Length(pHTTPData->pArgString);
	}

	pHTTPData->pCache = pHTTPData->pPath + pathLen + argLen;
	pHTTPData->cacheSize = gDefaults.filesys.maxHTTPFileCache - (serverLen + pathLen + argLen);
	pHTTPData->cacheStart = 0;

	MFString_Copy(pHTTPData->pServer, pServer);
	MFString_Copy(pHTTPData->pPath, pPath);

	// find file length
	pFile->length = 0;

	char *pNextLine = headerBuffer;

	while(*pNextLine && !MFIsNewline(*pNextLine))
	{
		if(!MFString_CaseCmpN("Content-Length:", pNextLine, 15))
		{
			const char *pLengthString = pNextLine + 15;

			int a=0;
			while(pLengthString[a] && !MFIsNewline(pLengthString[a]))
				++a;

			pLengthString = MFStrN(pLengthString, a);

			pFile->length = atoi(pLengthString);
		}

		pNextLine = MFSeekNewline(pNextLine);
	}

	pFile->createFlags = pOpenData->openFlags;
	pFile->offset = 0;

#if defined(_DEBUG)
	MFString_Copy(pFile->fileIdentifier, pHTTP->pURL);
#endif

	return 0;
}

int MFFileHTTP_Close(MFFile* fileHandle)
{
	MFCALLSTACK;

	gHTTPFiles.Destroy((MFFileHTTPData*)fileHandle->pFilesysData);

	return 0;
}

int MFFileHTTP_Read(MFFile* fileHandle, void *pBuffer, int64 bytes)
{
	MFCALLSTACK;

	MFFileHTTPData *pHTTPData = (MFFileHTTPData*)fileHandle->pFilesysData;

	const char *pHeaderRequest = MFStr("$s %s HTTP/1.1\nFrom: mtfuji@dotblip.com\nUser-Agent: Mount Fuji Engine/1.0\nHost: %s:%d\nRange: bytes=%d-%d\nRequest-Range: bytes=%d-%d\n\n%s", pHTTPData->pArgString ? "POST" : "GET", pHTTPData->pPath, pHTTPData->pServer, pHTTPData->port, (uint32)fileHandle->offset, (uint32)(fileHandle->offset + bytes - 1), (uint32)fileHandle->offset, (uint32)(fileHandle->offset + bytes - 1), pHTTPData->pArgString ? pHTTPData->pArgString : "");

	MFSocket socket = MFSockets_CreateSocket(MFAF_Inet, MFSockType_Stream, MFProtocol_TCP);
	if(MFSockets_Connect(socket, pHTTPData->address))
		return 0;

	MFSockets_Send(socket, pHeaderRequest, MFString_Length(pHeaderRequest) + 1, 0);

	const int bufferLen = 256;
	char temp[bufferLen+1];
	int read = MFSockets_Recv(socket, temp, bufferLen, 0);
	temp[bufferLen] = 0;

	// validate HTTP response
	if(MFString_CompareN("HTTP/", temp, 5))
		return -1;

	// get response code
	const char *pResponse = MFString_Chr(temp, ' ');
	if(!pResponse) return -1;
	++pResponse;
	const char *pT = MFString_Chr(pResponse, ' ');
	if(!pT) return -1;

//	int response = atoi(MFStrN(pResponse, pT-pResponse));
	int dataStart = 0;
	int dataEnd = (int)fileHandle->length;
	int dataLength = dataEnd;
	int contentLength = 0;

	const char *pLineStart = MFSeekNewline(++pT);
	int numNewlines = 0;
	do
	{
		pT = MFSeekNewline(pLineStart);

		if(!*pT)
		{
			int numBytes = (int)(pT - pLineStart);
			for(int i=0; i<numBytes; ++i)
				temp[i] = pLineStart[i];
			read = MFSockets_Recv(socket, temp + numBytes, bufferLen - numBytes, 0);
			pLineStart = temp;
			pT = MFSeekNewline(pLineStart);
		}

		const char *pLastChar = pT-1;
		while(MFIsNewline(*pLastChar))
			--pLastChar;
		const char *pHeaderLine = MFStrN(pLineStart, (++pLastChar) - pLineStart);

		// read header... migth be interesting...
		if(!MFString_CompareN("Content-Range: bytes ", pHeaderLine, 21))
		{
			sscanf(pHeaderLine + 21, "%d-%d/%d", &dataStart, &dataEnd, &dataLength);
		}
		else if(!MFString_CompareN("Content-Length: ", pHeaderLine, 16))
		{
			contentLength = atoi(pHeaderLine + 16);
		}

		// count \n's between line end and line start

		numNewlines = 0;
		while(pLastChar < pT)
		{
			if(*pLastChar == '\n')
				++numNewlines;
			++pLastChar;
		}
		pLineStart = pT;
	}
	while(numNewlines == 1);

	MFDebug_Assert(dataStart == fileHandle->offset, "Offset is wrong..");
	MFDebug_Assert(contentLength == bytes, "Length is wrong..");

	int numBytes = MFMin(bufferLen - (int)(size_t)(pT - temp), (int)bytes);
	if(numBytes)
	{
		MFCopyMemory(pBuffer, pT, numBytes);
		(char*&)pBuffer += numBytes;
		bytes -= numBytes;
	}

	while(bytes)
	{
		int read = MFSockets_Recv(socket, (char*)pBuffer, (int)bytes, 0);
		if(!read)
			break;
		(char*&)pBuffer += read;
		bytes -= read;
		numBytes += read;
	}

	MFSockets_CloseSocket(socket);

	return numBytes;
}

int MFFileHTTP_Write(MFFile* fileHandle, const void *pBuffer, int64 bytes)
{
	MFCALLSTACK;

	return 0;
}

int MFFileHTTP_Seek(MFFile* fileHandle, int64 bytes, MFFileSeek relativity)
{
	MFCALLSTACK;

	int64 newPos = 0;

	switch(relativity)
	{
		case MFSeek_Begin:
			newPos = MFMin(bytes, fileHandle->length);
			break;
		case MFSeek_End:
			newPos = MFMax((int64)0, fileHandle->length - bytes);
			break;
		case MFSeek_Current:
			newPos = MFClamp((int64)0, fileHandle->offset + bytes, fileHandle->length);
			break;
		default:
			MFDebug_Assert(false, "Invalid 'relativity' for file seeking.");
	}

	fileHandle->offset = newPos;
	return (int)newPos;
}
