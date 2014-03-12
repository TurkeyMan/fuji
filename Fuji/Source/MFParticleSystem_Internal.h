#if !defined(_MFPARTICLESYSTEM_INTERNAL_H)
#define _MFPARTICLESYSTEM_INTERNAL_H

#include "MFParticleSystem.h"
#include "MFPtrList.h"
#include "MFMaterial.h"

// internal functions
MFInitStatus MFParticleSystem_InitModule(int moduleId, bool bPerformInitialisation);
void MFParticleSystem_DeinitModule();

struct MFParticle
{
	MFVector pos;
	MFVector velocity;
	MFVector colour;
	float life;
	float rot;
	float size;
};

struct MFParticleSystem
{
	MFParticleParameters params;

	MFMaterial *pMaterial;

	MFPtrListDL<MFParticle> particles;
};

struct MFParticleEmitter
{
	MFParticleEmitterParameters params;

	float emitPeriod;
	float emitTimeout;
};

#endif
