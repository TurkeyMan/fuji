module fuji.particlesystem;

import fuji.matrix;

/**
 * @struct MFParticleSystem
 * Represents a Fuji particle system.
 */
struct MFParticleSystem;

struct MFParticleEmitter;

/**
 * Fuji emitter types.
 * Fuji particle emitter types.
 */
enum MFEmitterType
{
	Point,		/**< Emitter will emit from a singular point. */
	Sphere,		/**< Emitter will emit from a random point within a sphere. */
	Disc		/**< Emitter will emit from a random point on a disc. Particles will be concentrated towards the center of the disc. */
}

/**
 * Fuji emitter behaviour.
 * Fuji particle emitter behaviour.
 */
enum MFEmitterBehaviour
{
	Direction,			/**< \a startingVector defines an absolute direction. */
	TargetAttract,		/**< \a startingVector defines a point to which particles will be attracted. */
	TargetRepel			/**< \a startingVector defines a point which particles will be repelled from. */
}

/**
 * Particle system parameters.
 * Particle system parameters structure.
 */
struct MFParticleParameters
{
	MFVector colour;				/**< Colour of particles. */
	MFVector force;					/**< An arbitrary force vector which may be applied over time. */

	float life;						/**< Life of a single particle. */
	float size;						/**< Particles size. */

	float fadeDelay;				/**< Delay before particle begins to fade. */
	float rotationRate;				/**< Rate of rotation in radians/second. */
	float scaleRate;				/**< Rate of scale in units/second. */

	int maxActiveParticles;			/**< Maximum number of active particles. */

	const(char*) pMaterial;		/**< Name of material to apply to particles. */
}

/**
 * Particle emitter configuration.
 * Particle emitter configuration structure.
 */
struct MFParticleEmitterParameters
{
	MFMatrix position;				/**< Position of the emitter. */
	MFVector startVector;			/**< Starting vector. */

	MFParticleSystem* pParticleSystem;

	MFEmitterType type;				/**< Emitter type. */
	MFEmitterBehaviour behaviour;	/**< Motion behaviour. */

	float radius;					/**< Radius of volume primitives */

	float velocity;					/**< Starting velocity. */
	float velocityScatter;			/**< Velocity scatter from initial velocity. */

	float directionScatter;			/**< Directional scatter from initial direction in radians. */

	float emitRate;					/**< Number of particles emitted per second. */
}

/**
 * Creates a particle system.
 * Creates a particle system from a set of creation parameters.
 * @param pParticleParams Pointer to an MFParticleParameters describing the particle system.
 * @return Returns an instance of the defined particle system.
 * @see MFParticleSystem_Destroy()
 * @see MFParticleSystem_Draw()
 */
extern (C) MFParticleSystem* MFParticleSystem_Create(MFParticleParameters* pParticleParams);

/**
 * Dstroy a particle system.
 * Destroys a particle system.
 * @param pParticleSystem Particle system to be destroyed.
 * @return None.
 * @see MFParticleSystem_Create()
 * @see MFParticleSystem_Draw()
 */
extern (C) void MFParticleSystem_Destroy(MFParticleSystem* pParticleSystem);

/**
 * Draw a particle system.
 * Renders a particle system using the current scene configuration.
 * @param pParticleSystem Particle system to render.
 * @return None.
 * @see MFParticleSystem_Create()
 */
extern (C) void MFParticleSystem_Draw(MFParticleSystem* pParticleSystem);



extern (C) MFParticleEmitter* MFParticleSystem_CreateEmitter(MFParticleEmitterParameters* pEmitterParams);

extern (C) void MFParticleSystem_DestroyEmitter(MFParticleEmitter* pEmitter);

extern (C) void MFParticleSystem_UpdateEmitter(MFParticleEmitter* pEmitter);

extern (C) void MFParticleSystem_BurstEmit(MFParticleEmitter* pEmitter, int numParticles);

/**
 * Set the particle emitter world matrix.
 * Sets the particle emitters local to world matrix.
 * @param pEmitter Particle emitter.
 * @param worldMatrix World matrix to assign to the particle emitter.
 * @return None.
 * @see MFParticleSystem_Draw()
 */
extern (C) void MFParticleSystem_SetWorldMatrix(MFParticleEmitter* pEmitter, ref const(MFMatrix) worldMatrix);

