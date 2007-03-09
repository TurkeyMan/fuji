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
 * Various flags related to a sound.
 */
enum MFSoundFlags
{
	MFSF_Looping = MFBIT(0),		/**< Specifies that the sound is a looping sound. */
	MFSF_3D = MFBIT(1),				/**< Specifies that the sound should be played in 3d space. */
	MFSF_BeginPaused = MFBIT(2),	/**< Specifies that the voice will be created paused. */

	MFSF_Reserved = 0x7 << 28,		/**< Bit mask is reserved for internal use. */

	MFSF_ForceInt = 0x7FFFFFFF		/**< Force MFSoundFlags to an int type. */
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
 * @return Returns s pointer to the newly created sound buffer, or NULL on failure.
 * @see MFSound_Destroy(), MFSound_Play(), MFSound_LockDynamic(), MFSound_UnlockDynamic()
 */
MFSound *MFSound_CreateDynamic(const char *pName, int numSamples, int numChannels, int bitsPerSample, int samplerate);

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
 * Lock a dynamic sound buffer.
 * Locks a dynamic sound buffer for writing.
 * @param pSound Pointer to the sound to lock.
 * @param offset Offset into the sound buffer, in bytes.
 * @param bytes Number of bytes to lock.
 * @param ppData Pointer to a pointer that receives the address of the locked buffer portion.
 * @param pSize Pointer to an int the receives the size of the locked buffer portion.
 * @param ppData2 Pointer to a pointer that receives the second locked portion, or NULL. This is only used when locking a circular buffer and the lock length exceeds the buffers length. This parameter may be NULL.
 * @param pSize2 Pointer to an int the receives the size of the locked buffer portion. This parameter may be NULL.
 * @return Returns 0 on success.
 * @see MFSound_UnlockDynamic(), MFSound_CreateDynamic()
 */
int MFSound_LockDynamic(MFSound *pSound, int offset, int bytes, void **ppData, uint32 *pSize, void **ppData2 = NULL, uint32 *pSize2 = NULL);

/**
 * Unlock a dynamic buffer.
 * Unlocks a previously locked dynamic buffer.
 * @return None.
 * @see MFSound_LockDynamic()
 */
void MFSound_UnlockDynamic(MFSound *pSound);

/**
 * Play a sound.
 * Begin playback of a sound.
 * @param pSound Pointer to the sound to play.
 * @param playFlags Flags to control the way the sound is played.
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
 * Begin music playback.
 * Begin playback of a music stream.
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
 * Seek the music track.
 * Seeks the music track.
 * @param pStream Pointer to an MFAudioStream.
 * @param seconds Seek offset in seconds from the start of the stream.
 * @return None.
 */
void MFSound_SeekStream(MFAudioStream *pStream, float seconds);

/**
 * Pause music playback.
 * Pauses music playback.
 * @param pStream Pointer to an MFAudioStream.
 * @param pause true to pause the stream, false to resume playback.
 * @return None.
 */
void MFSound_PauseStream(MFAudioStream *pStream, bool pause);

/**
 * Set the music track volume.
 * Sets the music track volume.
 * @param pStream Pointer to an MFAudioStream.
 * @param volume Volume of the stream. The volume can range from 0.0f to 1.0f.
 * @return None.
 */
void MFSound_SetStreamVolume(MFAudioStream *pStream, float volume);

/**
 * Set the playback rate for an audio stream.
 * Sets the playback rate for an audio stream.
 * @param pStream Pointer to an MFAudioStream.
 * @param rate Playback rate for the stream. Default is 1.0f.
 * @return None.
 */
void MFSound_SetStreamPlaybackRate(MFAudioStream *pStream, float rate);

#endif // _MFSOUND_H

/** @} */
