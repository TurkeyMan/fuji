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
 * Load a sound bank.
 * Loads a sound bank.
 * @param pFilename Filename of the osund bank to load.
 * @return Returns the sound bank ID.
 */
int MFSound_LoadBank(const char *pFilename);

/**
 * Load a sound bank.
 * Loads a sound bank.
 * @param bankID ID of the bank to unload.
 * @return None.
 */
void MFSound_UnloadBank(int bankID);

/**
 * Find a sound.
 * Finds a specified sound.
 * @param pSoundName Name of sound to locate.
 * @param searchBankID Optional bank ID. If 0, all loaded sounds will be searched.
 * @return Returns the ID of the sound. -1 if the sound was not found.
 */
int MFSound_FindSound(const char *pSoundName, int searchBankID = 0);

/**
 * Play a sound.
 * Begin playback of a sound.
 * @param soundID ID of the sound.
 * @return Returns the voice ID.
 */
int MFSound_Play(int soundID);

/**
 * Play a sound in the 3D environment.
 * Begin playback of a sound in the 3D environment.
 * @param soundID ID of the sound.
 * @return Returns the voice ID.
 */
int MFSound_Play3D(int soundID);

/**
 * Stop a sound.
 * Stops playback of a sound.
 * @param voice Voice ID of the playing sound.
 * @return None.
 */
void MFSound_Stop(int voice);

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
 * @param voice Voice ID of the playing sound.
 * @param volume Volume of the voice. The volume can range from 0.0f to 1.0f.
 * @return None.
 */
void MFSound_SetVolume(int voice, float volume);

/**
 * Set the playback rate for a voice.
 * Sets the playback rate for a voice.
 * @param voice Voice ID of the playing sound.
 * @param rate Playback rate for the sound. Default is 1.0f.
 * @return None.
 */
void MFSound_SetPlaybackRate(int voice, float rate);

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
 * @return Returns an ID for the music track.
 */
int MFSound_MusicPlay(const char *pFilename, bool pause = false);

/**
 * Unload a music track.
 * Unloads a music track.
 * @param track Track ID of the music stream.
 * @return None.
 */
void MFSound_MusicUnload(int track);

/**
 * Seek the music track.
 * Seeks the music track.
 * @param track Track ID of the music stream.
 * @param seconds Seek offset in seconds from the start of the track.
 * @return None.
 */
void MFSound_MusicSeek(int track, float seconds);

/**
 * Pause music playback.
 * Pauses music playback.
 * @param track Track ID of the music stream.
 * @param pause true to pause the track, false to resume playback.
 * @return None.
 */
void MFSound_MusicPause(int track, bool pause);

/**
 * Set the music track volume.
 * Sets the music track volume.
 * @param track Track ID of the music stream.
 * @param volume Volume of the track. The volume can range from 0.0f to 1.0f.
 * @return None.
 */
void Sound_MusicSetVolume(int track, float volume);

#endif // _MFSOUND_H

/** @} */
