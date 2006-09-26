/**
 * @file MFParticleSystem.h
 * @brief Provides a basic particle system.
 * @author Manu Evans
 * @defgroup MFParticleSystem Particle System
 * @{
 */

#if !defined(_MFPARTICLESYSTEM_H)
#define _MFPARTICLESYSTEM_H

#include "MFMatrix.h"

/**
 * @struct MFParticleSystem
 * Represents a Fuji particle system.
 */
struct MFParticleSystem;

/**
 * Fuji emitter types.
 * Fuji particle emitter types.
 */
enum MFEmitterType
{
	MFET_Point,					/**< Emitter will emit from a singular point. */
	MFET_Sphere,				/**< Emitter will emit from a random point within a sphere. */
	MFET_Disc,					/**< Emitter will emit from a random point on a disc. Particles will be concentrated towards the center of the disc. */

	MFET_Max,					/**< Max emitter type. */
	MFET_ForceInt = 0x7FFFFFFF	/**< Force MFEmitterType to an int type. */
};

/**
 * Fuji emitter behaviour.
 * Fuji particle emitter behaviour.
 */
enum MFEmitterBehaviour
{
	MFEB_Direction,				/**< \a startingVector defines an absolute direction. */
	MFEB_TargetAttract,			/**< \a startingVector defines a point to which particles will be attracted. */
	MFEB_TargetRepel,			/**< \a startingVector defines a point which particles will be repelled from. */

	MFEB_Max,					/**< Max emitter behaviour. */
	MFEB_ForceInt = 0x7FFFFFFF	/**< Force MFEmitterBehaviour to an int type. */
};

/**
 * Particle emitter configuration.
 * Particle emitter configuration structure.
 */
struct MFParticleEmitter
{
	MFMatrix position;				/**< Position of the emitter. */
	MFVector startVector;			/**< Starting vector. */

	MFEmitterType type;				/**< Emitter type. */
	MFEmitterBehaviour behaviour;	/**< Motion behaviour. */

	float radius;					/**< Radius of volume primitives */

	float velocity;					/**< Starting velocity. */
	float velocityScatter;			/**< Velocity scatter from initial velocity. */

	float directionScatter;			/**< Directional scatter from initial direction in radians. */

	float emitRate;					/**< Number of particles emitted per second. */
};

/**
 * Particle system parameters.
 * Particle system parameters structure.
 */
struct MFParticleParameters
{
	MFVector colour;				/**< Colour of particles. */
	MFVector force;					/**< An arbitrary force vector which may be applied over time. */

	MFParticleEmitter *pEmitter;	/**< Emitter used to emit particles. */

	float life;						/**< Life of a single particle. */
	float size;						/**< Particles size. */

	float fadeDelay;				/**< Delay before particle begins to fade. */
	float rotationRate;				/**< Rate of rotation in radians/second. */
	float scaleRate;				/**< Rate of scale in units/second. */

	int maxActiveParticles;			/**< Maximum number of active particles. */

	const char *pMaterial;			/**< Name of material to apply to particles. */
};

/**
 * Creates a particle system.
 * Creates a particle system from a set of creation parameters.
 * @param pParticleParams Pointer to an MFParticleParameters describing the particle system.
 * @return Returns an instance of the defined particle system.
 * @see MFParticleSystem_Destroy()
 * @see MFParticleSystem_Draw()
 */
MFParticleSystem* MFParticleSystem_Create(MFParticleParameters *pParticleParams);

/**
 * Dstroy a particle system.
 * Destroys a particle system.
 * @param pParticleSystem Particle system to be destroyed.
 * @return None.
 * @see MFParticleSystem_Create()
 * @see MFParticleSystem_Draw()
 */
void MFParticleSystem_Destroy(MFParticleSystem *pParticleSystem);

/**
 * Draw a particle system.
 * Renders a particle system using the current scene configuration.
 * @param pParticleSystem Particle system to render.
 * @return None.
 * @see MFParticleSystem_Create()
 */
void MFParticleSystem_Draw(MFParticleSystem *pParticleSystem);

/**
 * Set the particle system world matrix.
 * Sets the particle systems local to world matrix.
 * @param pParticleSystem Particle system.
 * @param worldMatrix World matrix to assign to the particle system.
 * @return None.
 * @see MFParticleSystem_Draw()
 */
void MFParticleSystem_SetWorldMatrix(MFParticleSystem *pParticleSystem, const MFMatrix &worldMatrix);

#endif

/** @} */
