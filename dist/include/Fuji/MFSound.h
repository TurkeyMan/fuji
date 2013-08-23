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
 * Describes an MFSound buffer.
 */
struct MFSoundInfo
{
	int sampleRate;			/**< Sample playback rate. */
	int numSamples;			/**< Number of samples in the buffer. */
	int16 bitsPerSample;	/**< Number of bits per sample. */
	int16 numChannels;		/**< Number of channels. */
};

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
MF_API MFSound *MFSound_Create(const char *pName);

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
MF_API MFSound *MFSound_CreateDynamic(const char *pName, int numSamples, int numChannels, int bitsPerSample, int samplerate, uint32 flags);

/**
 * Destroy a sound.
 * Destroys a sound.
 * @param pSound Sound to destroy.
 * @return Returns the new reference count of the sound. If the returned reference count is 0, the sound is destroyed.
 * @see MFSound_Create()
 */
MF_API int MFSound_Destroy(MFSound *pSound);

/**
 * Find a sound.
 * Finds a specified sound.
 * @param pName Name of sound to locate.
 * @return Returns a pointer to the sound or NULL if the sound was not found.
 * @remarks MFSound_FindSound() does NOT increment the internal reference count of the object.
 */
MF_API MFSound *MFSound_FindSound(const char *pName);

/**
 * Fill a sound buffer with sample data.
 * Fills a sound buffer with sample data.
 * @param pSound Pointer to the sound to lock.
 * @param pData Pointer to a buffer containing sample data.
 * @param size Size in bytes of the buffer pointed to by \a pData.
 * @return Returns number of bytes written.
 */
MF_API size_t MFSound_SetBufferData(MFSound *pSound, const void *pData, size_t size);

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
MF_API int MFSound_Lock(MFSound *pSound, size_t offset, size_t bytes, void **ppData, size_t *pSize, void **ppData2 = NULL, size_t *pSize2 = NULL);

/**
 * Unlock a sound buffer.
 * Unlocks a previously locked sound buffer.
 * @return None.
 * @see MFSound_Lock()
 */
MF_API void MFSound_Unlock(MFSound *pSound);

/**
 * Play a sound.
 * Begin playback of a sound.
 * @param pSound Pointer to the sound to play.
 * @param playFlags A combination of zero or moew flags from the MFPlayFlags to control the way the sound is played.
 * @return Returns the voice ID.
 * @see MFSound_Stop(), MFSound_Create()
 */
MF_API MFVoice *MFSound_Play(MFSound *pSound, uint32 playFlags = 0);

/**
 * Pause a sound.
 * Pause or resume playback of a sound.
 * @param pVoice The voice to pause/resume.
 * @param pause Specifies weather to pause or resume playback. If \a pause is \c true, the sound will be paused, otherwise it will be resumed.
 * @return Returns None.
 * @see MFSound_Play()
 */
MF_API void MFSound_Pause(MFVoice *pVoice, bool pause = true);

/**
 * Stop a sound.
 * Stops playback of a sound.
 * @param pVoice Pointer to a playing voice.
 * @return None.
 * @remarks The void ID is obtained when calling MFSound_Play()
 * @see MFSound_Play()
 */
MF_API void MFSound_Stop(MFVoice *pVoice);

/**
 * Set the sound listener position.
 * Sets the sound listener position.
 * @param listenerPos A matrix representing the position and orientation of the listener in world space.
 * @return None.
 */
MF_API void MFSound_SetListenerPos(const MFMatrix& listenerPos);

/**
 * Set the volume of a voice.
 * Set the volume of a voice.
 * @param pVoice Pointer to a playing voice.
 * @param volume Volume of the voice. The volume can range from 0.0f to 1.0f.
 * @return None.
 */
MF_API void MFSound_SetVolume(MFVoice *pVoice, float volume);

/**
 * Set the playback rate for a voice.
 * Sets the playback rate for a voice.
 * @param pVoice Pointer to a playing voice.
 * @param rate Playback rate for the sound. Default is 1.0f.
 * @return None.
 */
MF_API void MFSound_SetPlaybackRate(MFVoice *pVoice, float rate);

/**
 * Set the voices pan.
 * Sets the voices pan.
 * @param pVoice Pointer to a playing voice.
 * @param pan Pan value where 0 is centered, -1 is fully to the left speaker, and +1 is fully to the right.
 * @return None.
 */
MF_API void MFSound_SetPan(MFVoice *pVoice, float pan);

/**
 * Set playback offset.
 * Sets the playback offset.
 * @param pVoice Pointer to a playing voice.
 * @param seconds Playback offset, in seconds.
 * @return None.
 */
MF_API void MFSound_SetPlaybackOffset(MFVoice *pVoice, float seconds);

/**
 * Set the master volume.
 * Sets the master volume.
 * @param volume Master volume for the sound system.
 * @return None.
 * @remarks The master volume modulates ALL playing audio, including all playing voices and music tracks.
 */
MF_API void MFSound_SetMasterVolume(float volume);

/**
 * Get the current play cursor, in samples.
 * Gets the current play cursor and write cursor, in samples.
 * @param pVoice Pointer to a playing voice.
 * @param pWriteCursor Optional pointer to a uint32 that received the position of the write cursor.
 * @return Returns the play cursor's position, in samples.
 */
MF_API uint32 MFSound_GetPlayCursor(MFVoice *pVoice, uint32 *pWriteCursor = NULL);

/**
 * Get the sound buffer from a voice.
 * Gets the sound buffer from a playing voice.
 * @param pVoice Pointer to a playing voice.
 * @return Returns the voices associated sound buffer.
 */
MF_API MFSound *MFSound_GetSoundFromVoice(MFVoice *pVoice);

/**
 * Get info about a sound.
 * Gets info about an MFSound.
 * @param pSound Pointer to an MFSound.
 * @param pInfo Pointer to an MFSoundInfo struct that receives information about the MFSound.
 * @return None.
 */
MF_API void MFSound_GetSoundInfo(MFSound *pSound, MFSoundInfo *pInfo);

/*** Music playback ***/

/**
 * Stream create flags.
 * Flags used to specify various properties of an audio stream when created.
 */
enum MFAudioStreamFlags
{
	MFASF_QueryLength = MFBIT(0),		/**< Allows the user to query the stream length. */
	MFASF_AllowSeeking = MFBIT(1),		/**< Allows seeking within the stream. */
	MFASF_AllowBuffering = MFBIT(2),	/**< Allows buffering of the compressed data if the driver chooses. (May use a lot of memory) */
	MFASF_DecodeOnly = MFBIT(3)			/**< The stream is created for decode only. Streams created with the MFASF_DecodeOnly flag may not be played. */
};

/**
 * Stream info type.
 * Enums representing various information that can be fetched from audio streams.
 */
enum MFStreamInfoType
{
	MFSIT_TrackName,				/**< Track name. */
	MFSIT_AlbumName,				/**< Album name. */
	MFSIT_ArtistName,				/**< Artists name. */
	MFSIT_Genre,					/**< Track genre. */

	MFSIT_Max,						/**< Max info type. */
	MFSIT_ForceInt = 0x7FFFFFFF,	/**< Force MFStreamInfoType to an int type. */
};

/**
 * Stream callbacks.
 * Callbacks used to access the audio stream.
 */
struct MFStreamCallbacks
{
	void (*pCreateStream)(MFAudioStream*, const char *);	/**< Create stream callback. */
	size_t (*pGetSamples)(MFAudioStream*, void *, size_t);	/**< Callback to get samples from the stream. */
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
MF_API void MFSound_RegisterStreamHandler(const char *pStreamType, const char *pStreamExtension, MFStreamCallbacks *pCallbacks);

/**
 * Create audio stream.
 * Create an audio stream.
 * @param pFilename Filename of music track.
 * @param flags Optional combination of flags from the MFAudioStreamFlags enum defining various stream options.
 * @return Returns a pointer to the created MFAudioStream or NULL on failure.
 */
MF_API MFAudioStream *MFSound_CreateStream(const char *pFilename, uint32 flags = 0);

/**
 * Begin stream playback.
 * Begin playback of an audio stream.
 * @param pStream Pointer to an MFAudioStream.
 * @param playFlags Optional combination of flags from the MFPlayFlags enum defining playback state.
 * @return Returns a pointer to the created MFAudioStream or NULL on failure.
 */
MF_API void MFSound_PlayStream(MFAudioStream *pStream, uint32 playFlags = 0);

/**
 * Destroy a music track.
 * Destroys a music track.
 * @param pStream Pointer to an MFAudioStream.
 * @return None.
 */
MF_API void MFSound_DestroyStream(MFAudioStream *pStream);

/**
 * Seek the stream.
 * Seeks the stream.
 * @param pStream Pointer to an MFAudioStream.
 * @param seconds Seek offset in seconds from the start of the stream.
 * @return None.
 */
MF_API void MFSound_SeekStream(MFAudioStream *pStream, float seconds);

/**
 * Pause stream playback.
 * Pauses stream playback.
 * @param pStream Pointer to an MFAudioStream.
 * @param pause true to pause the stream, false to resume playback.
 * @return None.
 */
MF_API void MFSound_PauseStream(MFAudioStream *pStream, bool pause);

/**
 * Get the voice associated with an MFAudioStream.
 * Gets the voice associated with an MFAudioStream.
 * @param pStream Pointer to an MFAudioStream.
 * @return Returns a pointer to the MFVoice associated with the stream.
 */
MF_API MFVoice *MFSound_GetStreamVoice(MFAudioStream *pStream);

/**
 * Get information associated with an audio stream.
 * Gets various information that maybe associated with an audio stream.
 * @param pStream Pointer to an MFAudioStream.
 * @param infoType The type of information to fetch.
 * @return Returns a string containing the requested infomation.
 */
MF_API const char *MFSound_GetStreamInfo(MFAudioStream *pStream, MFStreamInfoType infoType);

/**
 * Read sample data from an audio stream.
 * Reads raw samples from an audio stream.
 * @param pStream Pointer to an MFAudioStream.
 * @param pBuffer Pointer to a buffer that will receive the sample data.
 * @param bytes number of bytes to read from the stream.
 * @return Returns the number of bytes read.
 */
MF_API size_t MFSound_ReadStreamSamples(MFAudioStream *pStream, void *pBuffer, size_t bytes);

#endif // _MFSOUND_H

/** @} */
