/**
 * @file MFAnimScript.h
 * @brief Provides a layer above model animation which maintains the concept of animation sequences, and handles triggering scripted events in sync with animation playback.
 * @author Manu Evans
 * @defgroup MFAnimScript Animation Script System
 * @{
 */

#if !defined(_MFANIMSCRIPT_H)
#define _MFANIMSCRIPT_H

struct MFModel;

/**
 * @struct MFAnimScript
 * Represents a Fuji animation script.
 */
struct MFAnimScript;

/**
 * Create an animscript from the filesystem.
 * Creates an animscript from the filesystem.
 * @param pFilename Filename of the animscript to load.
 * @param pModel MFModel instance the animscript instance will refer to.
 * @return A new instance of the specified animscript.
 * @see MFAnimScript_Destroy()
 */
MF_API MFAnimScript* MFAnimScript_Create(const char *pFilename, MFModel *pModel);

/**
 * Destroy an animscript.
 * Destroys an animscript instance.
 * @param pAnimScript Anim script instance to be destroyed.
 * @return None.
 * @see MFAnimScript_Create()
 */
MF_API void MFAnimScript_Destroy(MFAnimScript *pAnimScript);

/**
 * Get the number of animation sequences in the anim script.
 * Gets the number of animation sequences in the anim script.
 * @param pAnimScript Anim script instance.
 * @return The number of sequences in the anim script.
 * @see MFAnimScript_Create(), MFAnimScript_SetSequence()
 */
MF_API int MFAnimScript_GetNumSequences(MFAnimScript *pAnimScript);

/**
 * Play an animation sequence.
 * Begin playback of an animation sequence.
 * @param pAnimScript Anim script instance.
 * @param sequence Sequence ID to begin playing.
 * @param tweenTime The amout of time to tween into the new sequence.
 * @return None.
 * @see MFAnimScript_Create(), MFAnimScript_GetNumSequences(), MFAnimScript_FindSequence()
 */
MF_API void MFAnimScript_PlaySequence(MFAnimScript *pAnimScript, int sequence, float tweenTime = 0.0f);

/**
 * Get the name of an animation sequence.
 * Gets the name of the specified animation sequence.
 * @param pAnimScript Anim script instance.
 * @param sequence Sequence ID.
 * @return The name of the specified.
 * @see MFAnimScript_FindSequence()
 */
MF_API const char* MFAnimScript_GetSequenceName(MFAnimScript *pAnimScript, int sequence);

/**
 * Find an animation sequence.
 * Find the ID of the named animation sequence.
 * @param pAnimScript Anim script instance.
 * @param pSequenceName Name of the sequence to find.
 * @return The ID of the named sequence, or -1 of the sequence does not exist.
 * @see MFAnimScript_GetSequenceName()
 */
MF_API int MFAnimScript_FindSequence(MFAnimScript *pAnimScript, const char *pSequenceName);

#endif

/** @} */
