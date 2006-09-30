#include "Fuji.h"
#include "MFSystem.h"
#include "MFFileSystem_Internal.h"
#include "FileSystem/MFFileSystemHTTP_Internal.h"
#include "MFSockets.h"
#include "MFPtrList.h"

static MFPtrListDL<MFFileHTTPData> gHTTPFiles;
 
void MFFileSystemHTTP_InitModule()
{
	if(MFSockets_IsActive())
	{
		MFFileSystemCallbacks fsCallbacks;

		fsCallbacks.RegisterFS = MFFileSystemHTTP_Register;
		fsCallbacks.UnregisterFS = MFFileSystemHTTP_Unregister;
		fsCallbacks.FSMount = MFFileSystemHTTP_Mount;
		fsCallbacks.FSDismount = MFFileSystemHTTP_Dismount;
		fsCallbacks.FSOpen = MFFileSystemHTTP_Open;
		fsCallbacks.Open = MFFileHTTP_Open;
		fsCallbacks.Close = MFFileHTTP_Close;
		fsCallbacks.Read = MFFileHTTP_Read;
		fsCallbacks.Write = MFFileHTTP_Write;
		fsCallbacks.Seek = MFFileHTTP_Seek;
		fsCallbacks.Tell = MFFileHTTP_Tell;
		fsCallbacks.Query = MFFileHTTP_Query;
		fsCallbacks.GetSize = MFFileHTTP_GetSize;

		hHTTPFileSystem = MFFileSystem_RegisterFileSystem(&fsCallbacks);

		// make a bunch of file buffers
		int gHTTPFileSize = sizeof(MFFileHTTPData)+gDefaults.filesys.maxHTTPFileCache;

		gHTTPFiles.Init("HTTP Files", gDefaults.filesys.maxHTTPFiles, gHTTPFileSize);
	}
}

void MFFileSystemHTTP_DeinitModule()
{
	gHTTPFiles.Deinit();

	if(hHTTPFileSystem != -1)
		MFFileSystem_UnregisterFileSystem(hHTTPFileSystem);
}
// filesystem callbacks
void MFFileSystemHTTP_Register()
{

}

void MFFileSystemHTTP_Unregister()
{

}

int MFFileSystemHTTP_Mount(MFMount *pMount, MFMountData *pMountData)
{
	MFDebug_Assert(pMountData->cbSize == sizeof(MFMountDataHTTP), "Incorrect size for MFMountDataHTTP structure. Invalid pMountData.");
	MFDebug_Assert(pMount->mountFlags & MFMF_OnlyAllowExclusiveAccess, "HTTP Filesystem can not generate a directory list. The MFMF_OnlyAllowExclusiveAccess flag MUST be specified for HTTP mounts.");

//	MFMountDataHTTP *pMountHTTP = (MFMountDataHTTP*)pMountData;

	pMount->numFiles = 0;
	pMount->pEntries = NULL;

	return 0;
}

int MFFileSystemHTTP_Dismount(MFMount *pMount)
{
	return 0;
}

MFFile* MFFileSystemHTTP_Open(MFMount *pMount, const char *pFilename, uint32 openFlags)
{
	MFFileHandle hFile = NULL;

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
		pPath = "/";

	*ppServer = pServer;
	*ppPath = pPath;
}

bool MFFileHTTP_RequestHeader(const char *pServer, int port, const char *pPath, char *pOutputBuffer, int maxSize)
{
	const char *pHeaderRequest = MFStr("GET %s HTTP/1.0\nFrom: mtfuji@dotblip.com\nUser-Agent: Mount Fuji Engine/1.0\n\n", pPath);

	MFAddressInfo addrInfo, *pAddrInfo;
	MFZeroMemory(&addrInfo, sizeof(MFAddressInfo));
	addrInfo.family = MFAF_Inet;
	addrInfo.type = MFSockType_Stream;
	addrInfo.protocol = MFProtocol_TCP;

	MFSockets_GetAddressInfo(pServer, MFStr("%d", port), &addrInfo, &pAddrInfo);

	if(!pAddrInfo)
	{
		MFDebug_Warn(2, MFStr("Couldnt find host: '%s' on port %d", pServer, port));
		return true;
	}

	MFSocket socket = MFSockets_CreateSocket(MFAF_Inet, MFSockType_Stream, MFProtocol_TCP);

	MFDebug_Warn(4, MFStr("Connecting to: '%s'", MFSockets_GetAddressString(*pAddrInfo->pAddress)));

	int connected = MFSockets_Connect(socket, *pAddrInfo->pAddress);

	while(connected && pAddrInfo->pNext)
	{
		pAddrInfo = pAddrInfo->pNext;
		connected = MFSockets_Connect(socket, *pAddrInfo->pAddress);
	}

	if(connected)
	{
		MFDebug_Warn(2, MFStr("Couldnt create a connection to '%s' on port %d", pServer, port));
		return true;
	}

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

	char headerBuffer[2048];
	bool foundServer = false;

	while(!foundServer)
	{
		if(MFFileHTTP_RequestHeader(pServer, pHTTP->port, pPath, headerBuffer, 2048))
		{
			// there was an error
			return 1;
		}

		if(!MFString_CaseCmpN(headerBuffer, "HTTP", 4))
		{
			const char *pErrorCode = MFStrN(headerBuffer + 9, 3);
			int errorCode = atoi(pErrorCode);

			if(errorCode == 200)
			{
				// success..
				foundServer = true;
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
			{
				MFDebug_Warn(2, MFStr("HTTP response code 404: Not Found", errorCode));
				return 2;
			}
			else if(errorCode == 400)
			{
				MFDebug_Warn(2, MFStr("HTTP response code 400: Bad Request", errorCode));
				return 3;
			}
			else if(errorCode == 401)
			{
				MFDebug_Warn(2, MFStr("HTTP response code 401: Unauthorized", errorCode));
				return 4;
			}
			else if(errorCode == 403)
			{
				MFDebug_Warn(2, MFStr("HTTP response code 403: Forbidden", errorCode));
				return 5;
			}
			else if(errorCode >= 100 && errorCode < 200)
			{
				// continue request
			}
			else if(errorCode > 200 && errorCode < 300)
			{

				// success or something
			}
			else
			{
				MFDebug_Warn(2, MFStr("HTTP server has returned an unknown response code %d", errorCode));
				return 6;
			}
		}
		else
		{
			MFDebug_Warn(2, "Invalid HTTP response, server is likely NOT a HTTP server...");
			// not a HTTP server
		}
	}

	MFFileHTTPData *pHTTPData = gHTTPFiles.Create();
	pFile->pFilesysData = pHTTPData;

	int serverLen = MFString_Length(pServer) + 1;
	int pathLen = MFString_Length(pPath) + 1;

	pHTTPData->pServer = (char*)&pHTTPData[1];
	pHTTPData->pPath = pHTTPData->pServer + serverLen;
	pHTTPData->port = pHTTP->port;
	pHTTPData->pCache = pHTTPData->pPath + pathLen;
	pHTTPData->cacheSize = gDefaults.filesys.maxHTTPFileCache - (serverLen + pathLen);
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

	pFile->state = MFFS_Ready;
	pFile->operation = MFFO_None;
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

int MFFileHTTP_Read(MFFile* fileHandle, void *pBuffer, uint32 bytes, bool async)
{
	MFCALLSTACK;

	return 0;
}

int MFFileHTTP_Write(MFFile* fileHandle, const void *pBuffer, uint32 bytes, bool async)
{
	MFCALLSTACK;

	return 0;
}

int MFFileHTTP_Seek(MFFile* fileHandle, int bytes, MFFileSeek relativity)
{
	MFCALLSTACK;

	int newPos = 0;

	switch(relativity)
	{
		case MFSeek_Begin:
			newPos = MFMin(bytes, fileHandle->length);
			break;
		case MFSeek_End:
			newPos = MFMax(0, fileHandle->length - bytes);
			break;
		case MFSeek_Current:
			newPos = MFClamp(0, (int)fileHandle->offset + bytes, fileHandle->length);
			break;
		default:
			MFDebug_Assert(false, "Invalid 'relativity' for file seeking.");
	}

	fileHandle->offset = (uint32)newPos;
	return newPos;
}

int MFFileHTTP_Tell(MFFile* fileHandle)
{
	MFCALLSTACK;
	return (int)fileHandle->offset;
}

MFFileState MFFileHTTP_Query(MFFile* fileHandle)
{
	MFCALLSTACK;
	return fileHandle->state;
}

int MFFileHTTP_GetSize(MFFile* fileHandle)
{
	MFCALLSTACK;
	return fileHandle->length;
}
