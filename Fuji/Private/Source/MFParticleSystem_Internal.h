#if !defined(_MFPARTICLESYSTEM_INTERNAL_H)
#define _MFPARTICLESYSTEM_INTERNAL_H

#include "MFParticleSystem.h"
#include "MFPtrList.h"
#include "MFMaterial.h"

// internal functions
void MFParticleSystem_InitModule();
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
	MFParticleEmitter emitter;
	MFParticleParameters params;

	MFMaterial *pMaterial;

	MFPtrListDL<MFParticle> particles;
};

#endif
