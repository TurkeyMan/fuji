/**
 * @file MFThread.h
 * @brief Fuji multi-threading interface.
 * @author Manu Evans
 * @defgroup MFThread Thread Management
 * @{
 */

#if !defined(_MFTHREAD_H)
#define _MFTHREAD_H

/**
 * Represents a Fuji Thread.
 * Represents a Fuji Thread.
 */
typedef void* MFThread;

/**
 * Represents a Fuji Mutex.
 * Represents a Fuji Mutex.
 */
typedef void* MFMutex;

/**
 * Represents a Fuji Semaphore.
 * Represents a Fuji Semaphore.
 */
typedef void* MFSemaphore;


/**
 * Callback function for a thread entry point.
 * Callback function for a thread entry point.
 */
typedef uint32 (*MFThreadEntryPoint)(void *);

/**
 *
 */
MFThread MFThread_CreateThread(const char *pName, MFThreadEntryPoint pEntryPoint, void *pUserData);

/**
 *
 */
void MFThread_ExitThread(uint32 exitCode);

/**
 *
 */
void MFThread_TerminateThread(MFThread thread, uint32 exitCode);

/**
 *
 */
uint32 MFThread_GetExitCode(MFThread thread);

/**
 *
 */
void MFThread_CloseThread(MFThread thread);

/**
 *
 */
MFMutex MFThread_CreateMutex(const char *pName);

/**
 *
 */
void MFThread_DestroyMutex(MFMutex mutex);

/**
 *
 */
void MFThread_LockMutex(MFMutex mutex);

/**
 *
 */
void MFThread_ReleaseMutex(MFMutex mutex);

/**
 *
 */
MFSemaphore MFThread_CreateSemaphore(const char *pName);

/**
 *
 */
void MFThread_DestroySemaphore(MFSemaphore semaphore);

/**
 *
 */
void MFThread_WaitSemaphore(MFSemaphore semaphore);

/**
 *
 */
void MFThread_ReleaseSemaphore(MFSemaphore semaphore);

#endif // _MFTHREAD_H

/** @} */
