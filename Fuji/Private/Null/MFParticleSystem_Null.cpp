#include "Fuji.h"
#include "MFSystem.h"
#include "MFParticleSystem_Internal.h"
#include "MFPrimitive.h"
#include "MFView.h"

MFParticleSystem* MFParticleSystem_Create(MFParticleParameters *pParticleParams)
{
	MFParticleSystem *pSystem = (MFParticleSystem*)MFHeap_Alloc(sizeof(MFParticleSystem));

	pSystem->params = *pParticleParams;
	pSystem->emitter = *pParticleParams->pEmitter;
	pSystem->params.pEmitter = &pSystem->emitter;

	pSystem->pMaterial = MFMaterial_Create(pParticleParams->pMaterial);

	pSystem->particles.Init("ParticleSystem", pSystem->params.maxActiveParticles);

	return pSystem;
}

void MFParticleSystem_Destroy(MFParticleSystem *pParticleSystem)
{
	pParticleSystem->particles.Deinit();

	MFHeap_Free(pParticleSystem);
}

void MFParticleSystem_DrawRotating(MFParticleSystem *pParticleSystem)
{
	int numParticles = pParticleSystem->particles.GetLength();
	float fadeStart = pParticleSystem->params.life - pParticleSystem->params.fadeDelay;

	MFPrimitive(PT_QuadList, 0);
	MFBegin(numParticles * 2);

	MFParticle **ppI = pParticleSystem->particles.Begin();

	while(*ppI)
	{
		MFParticle *pParticle = *ppI;

		float dt = MFSystem_TimeDelta();

		pParticle->rot += pParticleSystem->params.rotationRate * dt;
		pParticle->size += pParticleSystem->params.scaleRate * dt;
		pParticle->velocity += pParticleSystem->params.force * dt;
		pParticle->pos += pParticle->velocity * dt;

		float halfSize = pParticle->size * 0.5f;
		float alpha = MFMin(pParticle->life / fadeStart, 1.0f);

		MFSetColour(MakeVector(pParticle->colour, alpha));
		MFSetTexCoord1(0, 0);
		MFSetPosition(pParticle->pos.x - halfSize, pParticle->pos.y - halfSize, pParticle->pos.z);
		MFSetTexCoord1(1, 0);
		MFSetPosition(pParticle->pos.x - halfSize, pParticle->pos.y - halfSize, pParticle->pos.z);
		MFSetTexCoord1(0, 1);
		MFSetPosition(pParticle->pos.x - halfSize, pParticle->pos.y - halfSize, pParticle->pos.z);
		MFSetTexCoord1(1, 0);
		MFSetPosition(pParticle->pos.x - halfSize, pParticle->pos.y - halfSize, pParticle->pos.z);
		MFSetTexCoord1(1, 1);
		MFSetPosition(pParticle->pos.x + halfSize, pParticle->pos.y + halfSize, pParticle->pos.z);
		MFSetTexCoord1(0, 1);
		MFSetPosition(pParticle->pos.x - halfSize, pParticle->pos.y - halfSize, pParticle->pos.z);

		pParticle->life -= dt;
		if(pParticle->life < 0.0f)
			pParticleSystem->particles.Destroy(ppI);

		ppI++;

		MFEnd();
	}
}

void MFParticleSystem_Draw(MFParticleSystem *pParticleSystem)
{
	// emit new particles

	// update and draw each particle
	if(pParticleSystem->params.rotationRate != 0.0f)
	{
		MFParticleSystem_DrawRotating(pParticleSystem);
		return;
	}

	int numParticles = pParticleSystem->particles.GetLength();
	float fadeStart = pParticleSystem->params.life - pParticleSystem->params.fadeDelay;

	MFMaterial_SetMaterial(pParticleSystem->pMaterial);

	MFPrimitive(PT_QuadList, 0);
	MFBegin(numParticles * 2);

	MFParticle **ppI = pParticleSystem->particles.Begin();

	while(*ppI)
	{
		MFParticle *pParticle = *ppI;

		float dt = MFSystem_TimeDelta();

		pParticle->size += pParticleSystem->params.scaleRate * dt;
		pParticle->velocity += pParticleSystem->params.force * dt;
		pParticle->pos += pParticle->velocity * dt;

		float halfSize = pParticle->size * 0.5f;
		float alpha = MFMin(pParticle->life / fadeStart, 1.0f);

		MFSetColour(MakeVector(pParticle->colour, alpha));
		MFSetTexCoord1(0, 0);
		MFSetPosition(pParticle->pos.x - halfSize, pParticle->pos.y - halfSize, pParticle->pos.z);
		MFSetTexCoord1(1, 1);
		MFSetPosition(pParticle->pos.x + halfSize, pParticle->pos.y + halfSize, pParticle->pos.z);

		pParticle->life -= dt;
		if(pParticle->life < 0.0f)
			pParticleSystem->particles.Destroy(ppI);

		ppI++;
	}

	MFEnd();
}
