#include "Fuji.h"

#if MF_PARTICLESYSTEM == NULL

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

	if(!pParticleParams->pMaterial)
		pSystem->pMaterial = MFMaterial_Create("_None");
	else
		pSystem->pMaterial = MFMaterial_Create(pParticleParams->pMaterial);

	pSystem->emitPeriod = pSystem->emitter.emitRate == 0.0f ? 0.0f : 1.0f / pSystem->emitter.emitRate;
	pSystem->emitTimeout = 0.0f;

	pSystem->particles.Init("ParticleSystem", pSystem->params.maxActiveParticles);

	return pSystem;
}

void MFParticleSystem_Destroy(MFParticleSystem *pParticleSystem)
{
	pParticleSystem->particles.Deinit();

	MFHeap_Free(pParticleSystem);
}

void MFParticleSystem_DrawRotating(MFParticleSystem *pParticleSystem, const MFMatrix &ltv)
{
	int numParticles = pParticleSystem->particles.GetLength();
	if(!numParticles)
		return;

	float fadeStart = pParticleSystem->params.life - pParticleSystem->params.fadeDelay;

	MFMaterial_SetMaterial(pParticleSystem->pMaterial);

	MFPrimitive(PT_TriList, 0);
	MFBegin(numParticles * 6);

	MFParticle **ppI = pParticleSystem->particles.Begin();

	while(*ppI)
	{
		MFParticle *pParticle = *ppI;

		float dt = MFSystem_TimeDelta();

		pParticle->rot += pParticleSystem->params.rotationRate * dt;
		pParticle->size += pParticleSystem->params.scaleRate * dt;
		pParticle->velocity += pParticleSystem->params.force * dt;
		pParticle->pos += pParticle->velocity * dt;

		float t = pParticle->size * 0.5f;
		float rad = MFSqrt(t*t*2);
		float xoff = MFCos(-pParticle->rot + 0.7853981f)*rad;
		float yoff = MFSin(-pParticle->rot + 0.7853981f)*rad;
		float alpha = MFMin(pParticle->life / fadeStart, 1.0f);

		MFVector pos = ApplyMatrixH(pParticle->pos, ltv);

		MFSetColour(MakeVector(pParticle->colour, pParticle->colour.w * alpha));
		MFSetTexCoord1(1, 0);
		MFSetPosition(pos.x + xoff, pos.y + yoff, pos.z);
		MFSetTexCoord1(0, 1);
		MFSetPosition(pos.x - xoff, pos.y - yoff, pos.z);
		MFSetTexCoord1(0, 0);
		MFSetPosition(pos.x - yoff, pos.y + xoff, pos.z);
		MFSetTexCoord1(1, 0);
		MFSetPosition(pos.x + xoff, pos.y + yoff, pos.z);
		MFSetTexCoord1(1, 1);
		MFSetPosition(pos.x + yoff, pos.y - xoff, pos.z);
		MFSetTexCoord1(0, 1);
		MFSetPosition(pos.x - xoff, pos.y - yoff, pos.z);

		pParticle->life -= dt;
		if(pParticle->life < 0.0f)
			pParticleSystem->particles.Destroy(ppI);

		ppI++;
	}

	MFEnd();
}

void MFParticleSystem_Draw(MFParticleSystem *pParticleSystem)
{
	MFParticleEmitter *pE = &pParticleSystem->emitter;
	MFParticleParameters *pP = &pParticleSystem->params;

	// emit new particles
	pParticleSystem->emitTimeout += MFSystem_TimeDelta();

	while(pParticleSystem->emitTimeout > pParticleSystem->emitPeriod && pParticleSystem->particles.GetLength() < pParticleSystem->params.maxActiveParticles)
	{
		MFParticle *pNew = pParticleSystem->particles.Create();

		pNew->colour = pP->colour;
		pNew->life = pP->life;
		pNew->rot = 0.0f;
		pNew->size = pP->size;

		switch(pE->type)
		{
			case MFET_Point:
				pNew->pos = pE->position.GetTrans();
				break;

			case MFET_Sphere:
			case MFET_Disc:
			{
				MFVector offset;

				do
				{
					offset = MakeVector(MFRand_Range(-pE->radius, pE->radius), MFRand_Range(-pE->radius, pE->radius), MFRand_Range(-pE->radius, pE->radius));
				}
				while(offset.MagSquared3() > pE->radius*pE->radius);

				if(pE->type == MFET_Disc)
				{
					// flatten it on to the disc
					float dist = offset.Dot3(pE->position.GetYAxis());
					offset -= pE->position.GetYAxis()*dist;
				}

				pNew->pos = pE->position.GetTrans() + offset;

				break;
			}
		}

		switch(pE->behaviour)
		{
			case MFEB_Direction:
				pNew->velocity.Normalise3(pE->startVector);
				break;
			case MFEB_TargetAttract:
				pNew->velocity.Normalise3(pE->startVector - pE->position.GetTrans());
				break;
			case MFEB_TargetRepel:
				pNew->velocity.Normalise3(pE->position.GetTrans() - pE->startVector);
				break;
		}

		pNew->velocity *= pE->velocity + MFRand_Range(-pE->velocityScatter, pE->velocityScatter);

		if(pE->directionScatter)
		{
			MFVector scatter;

			do
			{
				scatter = MakeVector(MFRand_Range(-1, 1), MFRand_Range(-1, 1), MFRand_Range(-1, 1));

				float dist = scatter.Dot3(pE->position.GetYAxis());
				scatter -= pE->position.GetYAxis()*dist;
			}
			while(scatter.MagSquared3() < 0.000001f);

			scatter.Normalise3();

			MFMatrix scatterMat;
			scatterMat.SetRotation(scatter, MFRand_Unit()*pE->directionScatter);

			pNew->velocity = ApplyMatrixH(pNew->velocity, scatterMat);
		}

		pParticleSystem->emitTimeout -= pParticleSystem->emitPeriod;
	}

	int numParticles = pParticleSystem->particles.GetLength();
	if(numParticles == 0)
		return;

	// render particles
	MFView_Push();

	MFMatrix ltv = MFView_GetWorldToViewMatrix();
	MFView_SetCameraMatrix(MFMatrix::identity);

	// update and draw each particle
	if(pParticleSystem->params.rotationRate != 0.0f)
	{
		MFParticleSystem_DrawRotating(pParticleSystem, ltv);
		MFView_Pop();
		return;
	}

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

		MFVector pos = ApplyMatrixH(pParticle->pos, ltv);

		MFSetColour(MakeVector(pParticle->colour, pParticle->colour.w * alpha));
		MFSetTexCoord1(0, 0);
		MFSetPosition(pos.x - halfSize, pos.y + halfSize, pos.z);
		MFSetTexCoord1(1, 1);
		MFSetPosition(pos.x + halfSize, pos.y - halfSize, pos.z);

		pParticle->life -= dt;
		if(pParticle->life < 0.0f)
			pParticleSystem->particles.Destroy(ppI);

		ppI++;
	}

	MFEnd();

	MFView_Pop();
}

#endif
