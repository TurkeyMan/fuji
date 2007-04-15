/**
 * @file MFSound.h
 * @brief Provides access to the Mount Fuji Engine sound system.
 * @author Manu Evans
 * @defgroup MFSound Sound System
 * @{
 */

#if !defined(_MFSOUND_H)
#define _MFSOUND_H

#include "MFMatrix.h"

/**
 * @struct MFSound
 * Represents a Fuji sound.
 */
struct MFSound;

/**
 * @struct MFVoice
 * Represents a playing sound.
 */
struct MFVoice;

/**
 * @struct MFAudioStream
 * Represents a Fuji audio/music stream.
 */
struct MFAudioStream;

/**
 * Sound flags.
 * Various flags related to the behaviour of MFSound buffers.
 */
enum MFSoundFlagsInternal
{
	MFSF_Dynamic = MFBIT(0),		/**< Specifies that the sound will be a dynamic sound buffer. */
	MFSF_Circular = MFBIT(1),		/**< Specifies that the sound buffer is a circular buffer to be used for streaming. */

	MFSF_ForceInt = 0x7FFFFFFF		/**< Force MFSoundFlags to an int type. */
};

/**
 * Sound play flags.
 * Various flags related to playback of a sound.
 */
enum MFPlayFlags
{
	MFPF_Looping = MFBIT(0),		/**< Specifies that the sound is a looping sound. */
	MFPF_3D = MFBIT(1),				/**< Specifies that the sound should be played in 3d space. */
	MFPF_BeginPaused = MFBIT(2),	/**< Specifies that the voice will be created paused. */

	MFPF_Reserved = 0x7 << 28,		/**< Bit mask is reserved for internal use. */

	MFPF_ForceInt = 0x7FFFFFFF		/**< Force MFPlayFlags to an int type. */
};

/**
 * Load a sound.
 * Loads a sound.
 * @param pName Name of the sound to load.
 * @return Returns s pointer to the newly created sound, or NULL on failure.
 * @see MFSound_Destroy(), MFSound_Play()
 */
MFSound *MFSound_Create(const char *pName);

//MFSound *MFSound_CreateFromMemory(const char *pName, const char *pData);

//MFSound *MFSound_CreateFromFile(const char *pName, MFFileHandle hFile);

/**
 * Create a dynamic sound buffer.
 * Creates a dynamic sound buffer.
 * @param pName Name of the sound to create.
 * @param numSamples Number of samples in the buffer.
 * @param numChannels Number of channels in the sound buffer.
 * @param bitsPerSample Bits per sample.
 * @param samplerate Playback rate in samples per second.
 * @param flags A combination of zero or more flags from the MFSoundFlags enum to control the behaviour of the sound buffer.
 * @return Returns s pointer to the newly created sound buffer, or NULL on failure.
 * @see MFSound_Destroy(), MFSound_Play(), MFSound_LockDynamic(), MFSound_UnlockDynamic()
 */
MFSound *MFSound_CreateDynamic(const char *pName, int numSamples, int numChannels, int bitsPerSample, int samplerate, uint32 flags);

/**
 * Destroy a sound.
 * Destroys a sound.
 * @param pSound Sound to destroy.
 * @return Returns the new reference count of the sound. If the returned reference count is 0, the sound is destroyed.
 * @see MFSound_Create()
 */
int MFSound_Destroy(MFSound *pSound);

/**
 * Find a sound.
 * Finds a specified sound.
 * @param pName Name of sound to locate.
 * @return Returns a pointer to the sound or NULL if the sound was not found.
 * @remarks MFSound_FindSound() does NOT increment the internal reference count of the object.
 */
MFSound *MFSound_FindSound(const char *pName);

/**
 * Lock a sound buffer.
 * Locks a sound buffer for writing.
 * @param pSound Pointer to the sound to lock.
 * @param offset Offset into the sound buffer, in bytes.
 * @param bytes Number of bytes to lock. If bytes is 0, the entire buffer is locked.
 * @param ppData Pointer to a pointer that receives the address of the locked buffer portion.
 * @param pSize Pointer to an int the receives the size of the locked buffer portion.
 * @param ppData2 Pointer to a pointer that receives the second locked portion, or NULL. This is only used when locking a circular buffer and the lock length exceeds the buffers length. This parameter may be NULL.
 * @param pSize2 Pointer to an int the receives the size of the locked buffer portion. This parameter may be NULL.
 * @return Returns 0 on success.
 * @see MFSound_Unlock(), MFSound_CreateDynamic()
 */
int MFSound_Lock(MFSound *pSound, int offset, int bytes, void **ppData, uint32 *pSize, void **ppData2 = NULL, uint32 *pSize2 = NULL);

/**
 * Unlock a sound buffer.
 * Unlocks a previously locked sound buffer.
 * @return None.
 * @see MFSound_Lock()
 */
void MFSound_Unlock(MFSound *pSound);

/**
 * Play a sound.
 * Begin playback of a sound.
 * @param pSound Pointer to the sound to play.
 * @param playFlags A combination of zero or moew flags from the MFPlayFlags to control the way the sound is played.
 * @return Returns the voice ID.
 * @see MFSound_Stop(), MFSound_Create()
 */
MFVoice *MFSound_Play(MFSound *pSound, uint32 playFlags = 0);

/**
 * Pause a sound.
 * Pause or resume playback of a sound.
 * @param pVoice The voice to pause/resume.
 * @param pause Specifies weather to pause or resume playback. If \a pause is \c true, the sound will be paused, otherwise it will be resumed.
 * @return Returns None.
 * @see MFSound_Play()
 */
void MFSound_Pause(MFVoice *pVoice, bool pause = true);

/**
 * Stop a sound.
 * Stops playback of a sound.
 * @param pVoice Pointer to a playing voice.
 * @return None.
 * @remarks The void ID is obtained when calling MFSound_Play()
 * @see MFSound_Play()
 */
void MFSound_Stop(MFVoice *pVoice);

/**
 * Set the sound listener position.
 * Sets the sound listener position.
 * @param listenerPos A matrix representing the position and orientation of the listener in world space.
 * @return None.
 */
void MFSound_SetListenerPos(const MFMatrix& listenerPos);

/**
 * Set the volume of a voice.
 * Set the volume of a voice.
 * @param pVoice Pointer to a playing voice.
 * @param volume Volume of the voice. The volume can range from 0.0f to 1.0f.
 * @return None.
 */
void MFSound_SetVolume(MFVoice *pVoice, float volume);

/**
 * Set the playback rate for a voice.
 * Sets the playback rate for a voice.
 * @param pVoice Pointer to a playing voice.
 * @param rate Playback rate for the sound. Default is 1.0f.
 * @return None.
 */
void MFSound_SetPlaybackRate(MFVoice *pVoice, float rate);

/**
 * Set the voices pan.
 * Sets the voices pan.
 * @param pVoice Pointer to a playing voice.
 * @param pam Pan value where 0 is centered, -1 is fully to the left speaker, and +1 is fully to the right.
 * @return None.
 */
void MFSound_SetPan(MFVoice *pVoice, float pan);

/**
 * Set playback offset.
 * Sets the playback offset.
 * @param pVoice Pointer to a playing voice.
 * @param offset Playback offset, in seconds.
 * @return None.
 */
void MFSound_SetPlaybackOffset(MFVoice *pVoice, float seconds);

/**
 * Set the master volume.
 * Sets the master volume.
 * @param volume Master volume for the sound system.
 * @return None.
 * @remarks The master volume modulates ALL playing audio, including all playing voices and music tracks.
 */
void MFSound_SetMasterVolume(float volume);


/*** Music playback ***/

/**
 * Stream callbacks.
 * Callbacks used to access the audio stream.
 */
struct MFStreamCallbacks
{
	void (*pCreateStream)(MFAudioStream*, const char *);	/**< Create stream callback. */
	int (*pGetSamples)(MFAudioStream*, void *, uint32);		/**< Callback to get samples from the stream. */
	void (*pDestroyStream)(MFAudioStream*);					/**< Destroy stream callback. */
	void (*pSeekStream)(MFAudioStream*, float);				/**< Seek stream callbacks. */
	float (*pGetTime)(MFAudioStream*);						/**< Get the current stream time. */
};

/**
 * Register audio stream handler.
 * Registers an audio stream handler.
 * @param pStreamType A name for the type of stream.
 * @param pStreamExtension The file extension found on this stream format.
 * @param pCallbacks Pointer to an MFStreamCallbacks structure which defines the stream access callbacks.
 * @return None.
 */
void MFSound_RegisterStreamHandler(const char *pStreamType, const char *pStreamExtension, MFStreamCallbacks *pCallbacks);

/**
 * Begin stream playback.
 * Begin playback of an audio stream.
 * @param pFilename Filename of music track.
 * @param pause Initial pause state.
 * @return Returns a pointer to the created MFAudioStream or NULL on failure.
 */
MFAudioStream *MFSound_PlayStream(const char *pFilename, bool pause = false);

/**
 * Destroy a music track.
 * Destroys a music track.
 * @param pStream Pointer to an MFAudioStream.
 * @return None.
 */
void MFSound_DestroyStream(MFAudioStream *pStream);

/**
 * Seek the stream.
 * Seeks the stream.
 * @param pStream Pointer to an MFAudioStream.
 * @param seconds Seek offset in seconds from the start of the stream.
 * @return None.
 */
void MFSound_SeekStream(MFAudioStream *pStream, float seconds);

/**
 * Pause stream playback.
 * Pauses stream playback.
 * @param pStream Pointer to an MFAudioStream.
 * @param pause true to pause the stream, false to resume playback.
 * @return None.
 */
void MFSound_PauseStream(MFAudioStream *pStream, bool pause);

/**
 * Get the voice associated with an MFAudioStream.
 * Gets the voice associated with an MFAudioStream.
 * @param pStream Pointer to an MFAudioStream.
 * @return Returns a pointer to the MFVoice associated with the stream.
 */
MFVoice *MFSound_GetStreamVoice(MFAudioStream *pStream);

#endif // _MFSOUND_H

/** @} */
