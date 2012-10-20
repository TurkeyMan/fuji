module fuji.sound;

public import fuji.fuji;
import fuji.matrix;

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
	short bitsPerSample;	/**< Number of bits per sample. */
	short numChannels;		/**< Number of channels. */
}

/**
* Sound flags.
* Various flags related to the behaviour of MFSound buffers.
*/
enum MFSoundFlagsInternal
{
	Dynamic = MFBit!0,		/**< Specifies that the sound will be a dynamic sound buffer. */
	Circular = MFBit!1		/**< Specifies that the sound buffer is a circular buffer to be used for streaming. */
}

/**
* Sound play flags.
* Various flags related to playback of a sound.
*/
enum MFPlayFlags
{
	Looping = MFBit!0,		/**< Specifies that the sound is a looping sound. */
	_3D = MFBit!1,			/**< Specifies that the sound should be played in 3d space. */
	BeginPaused = MFBit!2,	/**< Specifies that the voice will be created paused. */

	Reserved = 0x7 << 28	/**< Bit mask is reserved for internal use. */
}

/**
* Load a sound.
* Loads a sound.
* @param pName Name of the sound to load.
* @return Returns s pointer to the newly created sound, or null on failure.
* @see MFSound_Destroy(), MFSound_Play()
*/
extern (C) MFSound* MFSound_Create(const char *pName);

//MFSound* MFSound_CreateFromMemory(const char *pName, const char *pData);

//MFSound* MFSound_CreateFromFile(const char *pName, MFFileHandle hFile);

/**
* Create a dynamic sound buffer.
* Creates a dynamic sound buffer.
* @param pName Name of the sound to create.
* @param numSamples Number of samples in the buffer.
* @param numChannels Number of channels in the sound buffer.
* @param bitsPerSample Bits per sample.
* @param samplerate Playback rate in samples per second.
* @param flags A combination of zero or more flags from the MFSoundFlags enum to control the behaviour of the sound buffer.
* @return Returns s pointer to the newly created sound buffer, or null on failure.
* @see MFSound_Destroy(), MFSound_Play(), MFSound_LockDynamic(), MFSound_UnlockDynamic()
*/
extern (C) MFSound* MFSound_CreateDynamic(const char *pName, int numSamples, int numChannels, int bitsPerSample, int samplerate, uint flags);

/**
* Destroy a sound.
* Destroys a sound.
* @param pSound Sound to destroy.
* @return Returns the new reference count of the sound. If the returned reference count is 0, the sound is destroyed.
* @see MFSound_Create()
*/
extern (C) int MFSound_Destroy(MFSound* pSound);

/**
* Find a sound.
* Finds a specified sound.
* @param pName Name of sound to locate.
* @return Returns a pointer to the sound or null if the sound was not found.
* @remarks MFSound_FindSound() does NOT increment the internal reference count of the object.
*/
extern (C) MFSound* MFSound_FindSound(const char *pName);

/**
* Lock a sound buffer.
* Locks a sound buffer for writing.
* @param pSound Pointer to the sound to lock.
* @param offset Offset into the sound buffer, in bytes.
* @param bytes Number of bytes to lock. If bytes is 0, the entire buffer is locked.
* @param ppData Pointer to a pointer that receives the address of the locked buffer portion.
* @param pSize Pointer to an int the receives the size of the locked buffer portion.
* @param ppData2 Pointer to a pointer that receives the second locked portion, or null. This is only used when locking a circular buffer and the lock length exceeds the buffers length. This parameter may be null.
* @param pSize2 Pointer to an int the receives the size of the locked buffer portion. This parameter may be null.
* @return Returns 0 on success.
* @see MFSound_Unlock(), MFSound_CreateDynamic()
*/
extern (C) int MFSound_Lock(MFSound* pSound, int offset, int bytes, void **ppData, uint *pSize, void **ppData2 = null, uint *pSize2 = null);

/**
* Unlock a sound buffer.
* Unlocks a previously locked sound buffer.
* @return None.
* @see MFSound_Lock()
*/
extern (C) void MFSound_Unlock(MFSound* pSound);

/**
* Play a sound.
* Begin playback of a sound.
* @param pSound Pointer to the sound to play.
* @param playFlags A combination of zero or moew flags from the MFPlayFlags to control the way the sound is played.
* @return Returns the voice ID.
* @see MFSound_Stop(), MFSound_Create()
*/
extern (C) MFVoice* MFSound_Play(MFSound* pSound, uint playFlags = 0);

/**
* Pause a sound.
* Pause or resume playback of a sound.
* @param pVoice The voice to pause/resume.
* @param pause Specifies weather to pause or resume playback. If \a pause is \c true, the sound will be paused, otherwise it will be resumed.
* @return Returns None.
* @see MFSound_Play()
*/
extern (C) void MFSound_Pause(MFVoice* pVoice, bool pause = true);

/**
* Stop a sound.
* Stops playback of a sound.
* @param pVoice Pointer to a playing voice.
* @return None.
* @remarks The void ID is obtained when calling MFSound_Play()
* @see MFSound_Play()
*/
extern (C) void MFSound_Stop(MFVoice* pVoice);

/**
* Set the sound listener position.
* Sets the sound listener position.
* @param listenerPos A matrix representing the position and orientation of the listener in world space.
* @return None.
*/
extern (C) void MFSound_SetListenerPos(ref const(MFMatrix) listenerPos);

/**
* Set the volume of a voice.
* Set the volume of a voice.
* @param pVoice Pointer to a playing voice.
* @param volume Volume of the voice. The volume can range from 0.0f to 1.0f.
* @return None.
*/
extern (C) void MFSound_SetVolume(MFVoice* pVoice, float volume);

/**
* Set the playback rate for a voice.
* Sets the playback rate for a voice.
* @param pVoice Pointer to a playing voice.
* @param rate Playback rate for the sound. Default is 1.0f.
* @return None.
*/
extern (C) void MFSound_SetPlaybackRate(MFVoice* pVoice, float rate);

/**
* Set the voices pan.
* Sets the voices pan.
* @param pVoice Pointer to a playing voice.
* @param pan Pan value where 0 is centered, -1 is fully to the left speaker, and +1 is fully to the right.
* @return None.
*/
extern (C) void MFSound_SetPan(MFVoice* pVoice, float pan);

/**
* Set playback offset.
* Sets the playback offset.
* @param pVoice Pointer to a playing voice.
* @param seconds Playback offset, in seconds.
* @return None.
*/
extern (C) void MFSound_SetPlaybackOffset(MFVoice* pVoice, float seconds);

/**
* Set the master volume.
* Sets the master volume.
* @param volume Master volume for the sound system.
* @return None.
* @remarks The master volume modulates ALL playing audio, including all playing voices and music tracks.
*/
extern (C) void MFSound_SetMasterVolume(float volume);

/**
* Get the current play cursor, in samples.
* Gets the current play cursor and write cursor, in samples.
* @param pVoice Pointer to a playing voice.
* @param pWriteCursor Optional pointer to a uint that received the position of the write cursor.
* @return Returns the play cursor's position, in samples.
*/
extern (C) uint MFSound_GetPlayCursor(MFVoice* pVoice, uint *pWriteCursor = null);

/**
* Get the sound buffer from a voice.
* Gets the sound buffer from a playing voice.
* @param pVoice Pointer to a playing voice.
* @return Returns the voices associated sound buffer.
*/
extern (C) MFSound* MFSound_GetSoundFromVoice(MFVoice* pVoice);

/**
* Get info about a sound.
* Gets info about an MFSound.
* @param pSound Pointer to an MFSound.
* @param pInfo Pointer to an MFSoundInfo struct that receives information about the MFSound.
* @return None.
*/
extern (C) void MFSound_GetSoundInfo(MFSound* pSound, MFSoundInfo *pInfo);

/*** Music playback ***/

/**
* Stream create flags.
* Flags used to specify various properties of an audio stream when created.
*/
enum MFAudioStreamFlags
{
	QueryLength = MFBit!0,		/**< Allows the user to query the stream length. */
	AllowSeeking = MFBit!1,		/**< Allows seeking within the stream. */
	AllowBuffering = MFBit!2,	/**< Allows buffering of the compressed data if the driver chooses. (May use a lot of memory) */
	DecodeOnly = MFBit!3		/**< The stream is created for decode only. Streams created with the MFASF_DecodeOnly flag may not be played. */
}

/**
* Stream info type.
* Enums representing various information that can be fetched from audio streams.
*/
enum MFStreamInfoType
{
	TrackName,		/**< Track name. */
	AlbumName,		/**< Album name. */
	ArtistName,		/**< Artists name. */
	Genre			/**< Track genre. */
}

/**
* Stream callbacks.
* Callbacks used to access the audio stream.
*/
struct MFStreamCallbacks
{
	void function(MFAudioStream*, const(char*)) pCreateStream;	/**< Create stream callback. */
	int function(MFAudioStream*, void*, uint) pGetSamples;		/**< Callback to get samples from the stream. */
	void function(MFAudioStream*) pDestroyStream;				/**< Destroy stream callback. */
	void function(MFAudioStream*, float) pSeekStream;			/**< Seek stream callbacks. */
	float function(MFAudioStream*) pGetTime;					/**< Get the current stream time. */
}

/**
* Register audio stream handler.
* Registers an audio stream handler.
* @param pStreamType A name for the type of stream.
* @param pStreamExtension The file extension found on this stream format.
* @param pCallbacks Pointer to an MFStreamCallbacks structure which defines the stream access callbacks.
* @return None.
*/
extern (C) void MFSound_RegisterStreamHandler(const char *pStreamType, const char *pStreamExtension, MFStreamCallbacks *pCallbacks);

/**
* Create audio stream.
* Create an audio stream.
* @param pFilename Filename of music track.
* @param flags Optional combination of flags from the MFAudioStreamFlags enum defining various stream options.
* @return Returns a pointer to the created MFAudioStream or null on failure.
*/
extern (C) MFAudioStream* MFSound_CreateStream(const char *pFilename, uint flags = 0);

/**
* Begin stream playback.
* Begin playback of an audio stream.
* @param pStream Pointer to an MFAudioStream.
* @param playFlags Optional combination of flags from the MFPlayFlags enum defining playback state.
* @return Returns a pointer to the created MFAudioStream or null on failure.
*/
extern (C) void MFSound_PlayStream(MFAudioStream *pStream, uint playFlags = 0);

/**
* Destroy a music track.
* Destroys a music track.
* @param pStream Pointer to an MFAudioStream.
* @return None.
*/
extern (C) void MFSound_DestroyStream(MFAudioStream *pStream);

/**
* Seek the stream.
* Seeks the stream.
* @param pStream Pointer to an MFAudioStream.
* @param seconds Seek offset in seconds from the start of the stream.
* @return None.
*/
extern (C) void MFSound_SeekStream(MFAudioStream *pStream, float seconds);

/**
* Pause stream playback.
* Pauses stream playback.
* @param pStream Pointer to an MFAudioStream.
* @param pause true to pause the stream, false to resume playback.
* @return None.
*/
extern (C) void MFSound_PauseStream(MFAudioStream *pStream, bool pause);

/**
* Get the voice associated with an MFAudioStream.
* Gets the voice associated with an MFAudioStream.
* @param pStream Pointer to an MFAudioStream.
* @return Returns a pointer to the MFVoice associated with the stream.
*/
extern (C) MFVoice* MFSound_GetStreamVoice(MFAudioStream *pStream);

/**
* Get information associated with an audio stream.
* Gets various information that maybe associated with an audio stream.
* @param pStream Pointer to an MFAudioStream.
* @param infoType The type of information to fetch.
* @return Returns a string containing the requested infomation.
*/
extern (C) const(char*) MFSound_GetStreamInfo(MFAudioStream *pStream, MFStreamInfoType infoType);

/**
* Read sample data from an audio stream.
* Reads raw samples from an audio stream.
* @param pStream Pointer to an MFAudioStream.
* @param pBuffer Pointer to a buffer that will receive the sample data.
* @param bytes number of bytes to read from the stream.
* @return Returns the number of bytes read.
*/
extern (C) int MFSound_ReadStreamSamples(MFAudioStream *pStream, void *pBuffer, int bytes);

