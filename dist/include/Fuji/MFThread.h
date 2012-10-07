/**
 * @file MFThread.h
 * @brief Fuji multi-threading interface.
 * @author Manu Evans
 * @defgroup MFThread Thread Management
 * @{
 */

#if !defined(_MFTHREAD_H)
#define _MFTHREAD_H

struct MFThreadInfo;

/**
 * Thread priority.
 * Thread priority.
 */
enum MFThreadPriority
{
	MFPriority_Maximum = 10,	/**< Maximum priority - This thread will be executed before all others. */
	MFPriority_AboveNormal = 6, /**< Above normal priority. */
	MFPriority_Normal = 5,		/**< Normal priority. */
	MFPriority_BelowNormal = 4,	/**< Below normal priority. */
	MFPriority_Low = 2,			/**< Low priority. */
	MFPriority_Idle = 0,		/**< Idle thread. */

	MFPriority_ForceInt = 0x7FFFFFFF	/**< Force enum to a uint type. */
};

/**
 * Thread flags.
 * Thread flags.
 */
enum MFThreadFlags
{
	MFTF_Joinable = 1,		/**< This thread will be joinable. */

	MFTF_ForceInt = 0x7FFFFFFF	/**< Force enum to a uint type. */
};

/**
 * Represents a Fuji Thread.
 * Represents a Fuji Thread.
 */
typedef MFThreadInfo* MFThread;

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
 * Represents a Fuji thread local store slot.
 * Represents a Fuji thread local store slot.
 */
typedef void* MFTls;


/**
 * Thread entry point function.
 * Thread entry point function.
 */
typedef int (*MFThreadEntryPoint)(void *);

/**
 * Create and start a new thread.
 * Creates and start a new thread.
 * @param pName Name to identify the new thread.
 * @param pEntryPoint Pointer to the entry point function for the new thread.
 * @param pUserData Pointer to be passed to the new threads entry point.
 * @param priority The threads execution priority.
 * @param flags Optional creation flags from \a MFThreadFlags.
 * @param stackSize Size of the new threads stack.
 * @return Returns a handle to the new thread.
 * @see MFThread_TerminateThread(), MFThread_ExitThread(), MFThread_GetExitCode(), MFThread_DestroyThread()
 */
MF_API MFThread MFThread_CreateThread(const char *pName, MFThreadEntryPoint pEntryPoint, void *pUserData, int priority = MFPriority_Normal, uint32 flags = 0, uint32 stackSize = 0);

/**
 * Exit from the current thread.
 * Exits the current thread.
 * @param exitCode The code to return from the thread, this is the same as the return value for the threads entry point function.
 * @return None.
 */
MF_API void MFThread_ExitThread(int exitCode);

/**
 * Terminate a thread.
 * Terminates a thread.
 * @param thread Thread to be terminated.
 * @return None.
 * @remarks Caution: Terminating a thread can be dangerous. Users should take care to make sure the thread has no locks on mutual exclusive data when it is terminated.
 */
MF_API void MFThread_TerminateThread(MFThread thread);

/**
 * Get a threads exit code.
 * Get the exit code for a finished thread.
 * @param thread Handle to a finished thread.
 * @return Returns the threads exit code.
 */
MF_API int MFThread_GetExitCode(MFThread thread);

/**
 * Destroy a thread.
 * Destroys a thread.
 * @param thread Handle to a thread to destroy.
 * @return None.
 */
MF_API void MFThread_DestroyThread(MFThread thread);

/**
 * Join threads.
 * Waits for the specified thread to complete.
 * @param thread Handle to a thread to join.
 * @return None.
 */
MF_API void MFThread_Join(MFThread thread);

/**
 * Create a mutex object.
 * Creates a mutex object that can be used to block a thread from accessing mutually exclusive data while another thread is already accessing it.
 * @param pName Name to identify the mutex.
 * @return Returns a handle to the created mutex.
 */
MF_API MFMutex MFThread_CreateMutex(const char *pName);

/**
 * Destroy a mutex object.
 * Destroys a mutex object.
 * @param mutex Handle of mutex to destroy.
 * @return None.
 */
MF_API void MFThread_DestroyMutex(MFMutex mutex);

/**
 * Lock a mutex object.
 * Locks a mutex object.
 * @param mutex Handle of mutex to lock.
 * @return None.
 */
MF_API void MFThread_LockMutex(MFMutex mutex);

/**
 * Release a mutex object.
 * Releases a mutex object.
 * @param mutex Handle of mutex to release.
 * @return None.
 */
MF_API void MFThread_ReleaseMutex(MFMutex mutex);

/**
 * Create a semaphore object.
 * Createa a semaphore object that can be used for thread syncronisation.
 * @param pName Name to identify the semaphore.
 * @param maxCount Maximum count for the semaphore.
 * @param startCount Starting count for the semaphore.
 * @return Returns a handle to the created semaphore.
 */
MF_API MFSemaphore MFThread_CreateSemaphore(const char *pName, int maxCount, int startCount);

/**
 * Destroy a semaphore object.
 * Destroys a semaphore object.
 * @param semaphore Handle of semaphore to destroy.
 * @return None.
 */
MF_API void MFThread_DestroySemaphore(MFSemaphore semaphore);

/**
 * Wait on a semaphore object.
 * Waits for a semaphore object to be signaled.
 * @param semaphore Handle of semaphore to wait on.
 * @return None.
 * @remarks MFThread_WaitSemaphore will succeed when the semaphore count is greater than zero. When a wait successfully completes, the semaphore is decremented by 1.
 */
MF_API uint32 MFThread_WaitSemaphore(MFSemaphore semaphore);

/**
 * Signal a semaphore object.
 * Signala a semaphore object.
 * @param semaphore Handle of semaphore to signal.
 * @return None.
 * @remarks Signaling a semaphore increments the semaphores count by 1. If the semaphore already contains the maximum value specified by CreateSemaphore, SignalSemaphore will wait until it has been decremented.
 */
MF_API void MFThread_SignalSemaphore(MFSemaphore semaphore);

/**
 * Allocate a TLS slot.
 * Allocates a thread local storage slot.
 * @return Returns a handle to the allocated local store slot, or NULL if MFThread_TlsAlloc() failed.
 */
MF_API MFTls MFThread_TlsAlloc();

/**
 * Free a TLS slot.
 * Frees a previously allocated thread local storage slot.
 * @param tls Handle to a TLS.
 * @return Allocated MFTls object.
 */
MF_API void MFThread_TlsFree(MFTls tls);

/**
 * Get the value from a TLS.
 * Gets the value stored in a thread local store slot.
 * @param tls Handle to a TLS.
 * @return Value stored in the requested TLS.
 */
MF_API void *MFThread_GetTls(MFTls tls);

/**
 * Set the value of a tls.
 * Sets the value stored in a thread local store slot.
 * @param tls Handle to a TLS.
 * @param pValue Value to set.
 * @return Returns the old value stored in the TLS slot.
 */
MF_API void *MFThread_SetTls(MFTls tls, void *pValue);

#endif // _MFTHREAD_H

/** @} */
