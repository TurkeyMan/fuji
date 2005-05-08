#include "Common.h"
#include "MFFileSystem_Internal.h"
#include "FileSystem/MFFileSystemNative.h"

void MFFileSystemNative_InitModule()
{
	MFFileSystemCallbacks fsCallbacks;

	fsCallbacks.RegisterFS = MFFileSystemNative_Register;
	fsCallbacks.UnregisterFS = MFFileSystemNative_Unregister;
	fsCallbacks.FSMount = MFFileSystemNative_Mount;
	fsCallbacks.FSOpen = MFFileSystemNative_Open;
	fsCallbacks.Open = MFFileNative_Open;
	fsCallbacks.Close = MFFileNative_Close;
	fsCallbacks.Read = MFFileNative_Read;
	fsCallbacks.Write = MFFileNative_Write;
	fsCallbacks.Seek = MFFileNative_Seek;
	fsCallbacks.Tell = MFFileNative_Tell;
	fsCallbacks.Query = MFFileNative_Query;
	fsCallbacks.GetSize = MFFileNative_GetSize;

	hNativeFileSystem = MFFileSystem_RegisterFileSystem(&fsCallbacks);
}

void MFFileSystemNative_DeinitModule()
{
	MFFileSystem_UnregisterFileSystem(hNativeFileSystem);
}
