#include "Fuji_Internal.h"
#include "MFVector.h"
#include "MFMatrix.h"
#include "MFCollision_Internal.h"
#include "MFHeap.h"
#include "MFModel_Internal.h"
#include "MFPtrList.h"
#include "MFPrimitive.h"

static MFPtrList<MFCollisionItem> gItemList;
static MenuItemBool gShowCollision;

/**** Internal Functions ****/

MFInitStatus MFCollision_InitModule(int moduleId, bool bPerformInitialisation)
{
	gItemList.Init("Collision Items", 256);

	DebugMenu_AddItem("Show Collision", "Fuji Options", &gShowCollision, NULL, NULL);

	return MFIS_Succeeded;
}

void MFCollision_DeinitModule()
{
	gItemList.Deinit();
}

MF_API void MFCollision_DebugDraw()
{
	if(!gShowCollision)
		return;

	// draw each item..
	MFCollisionItem **ppIterator = gItemList.Begin();

	while(*ppIterator)
	{
		MFCollision_DrawItem(*ppIterator);

		ppIterator++;
	}
}

void MFCollision_DrawItem(MFCollisionItem *pItem)
{
	// maybe reject it if its not in range..

	MFVector colour = (pItem->flags & MFCIF_Disabled) ? MakeVector(1,0,0,1) : ((pItem->flags & MFCIF_Dynamic) ? MakeVector(0,0,1,1) : MakeVector(0,1,0,1));

	MFMaterial_SetMaterial(MFMaterial_GetStockMaterial(MFMat_White));

	switch(pItem->pTemplate->type)
	{
		case MFCT_Sphere:
		{
			MFCollisionSphere *pSphere = (MFCollisionSphere*)pItem->pTemplate->pCollisionTemplateData;

			MFPrimitive_DrawSphere(MFVector::zero, pSphere->radius, 8, 7, colour, pItem->worldPos, true);
			break;
		}
		case MFCT_Mesh:
		{
			MFCollisionMesh *pMesh = (MFCollisionMesh*)pItem->pTemplate->pCollisionTemplateData;
			int a;

			// draw each triangle
			MFPrimitive(PT_TriList);
			MFSetMatrix(pItem->worldPos);
			MFBegin(pMesh->numTris * 3);
			MFSetColour(1.0f, 1.0f, 1.0f, 0.5f);

			for(a=0; a<pMesh->numTris; a++)
			{
				if(pMesh->pTriangles[a].flags)
					MFSetColour(1.0f, 0.0f, 0.0f, 0.5f);

				MFSetPositionV(pMesh->pTriangles[a].verts[0]);
				MFSetPositionV(pMesh->pTriangles[a].verts[1]);
				MFSetPositionV(pMesh->pTriangles[a].verts[2]);

				if(pMesh->pTriangles[a].flags)
				{
					MFSetColour(1.0f, 1.0f, 1.0f, 0.5f);
					pMesh->pTriangles[a].flags = 0;
				}
			}

			MFEnd();

			// draw each triangle outline
			MFPrimitive(PT_LineList);
			MFSetMatrix(pItem->worldPos);
			MFBegin(pMesh->numTris * 12);
			MFSetColourV(colour);

			for(a=0; a<pMesh->numTris; a++)
			{
				MFSetPositionV(pMesh->pTriangles[a].verts[0]);
				MFSetPositionV(pMesh->pTriangles[a].verts[1]);
				MFSetPositionV(pMesh->pTriangles[a].verts[1]);
				MFSetPositionV(pMesh->pTriangles[a].verts[2]);
				MFSetPositionV(pMesh->pTriangles[a].verts[2]);
				MFSetPositionV(pMesh->pTriangles[a].verts[0]);

				MFVector e1, e2, e3;

				e1 = (pMesh->pTriangles[a].verts[0] + pMesh->pTriangles[a].verts[1]) * 0.5f;
				e2 = (pMesh->pTriangles[a].verts[1] + pMesh->pTriangles[a].verts[2]) * 0.5f;
				e3 = (pMesh->pTriangles[a].verts[2] + pMesh->pTriangles[a].verts[0]) * 0.5f;

				MFSetPositionV(e1);
				MFSetPositionV(e1 + pMesh->pTriangles[a].edgePlanes[0] * 2.0f);
				MFSetPositionV(e2);
				MFSetPositionV(e2 + pMesh->pTriangles[a].edgePlanes[1] * 2.0f);
				MFSetPositionV(e3);
				MFSetPositionV(e3 + pMesh->pTriangles[a].edgePlanes[2] * 2.0f);

//				MFVector c = (pMesh->pTriangles[a].verts[0] + pMesh->pTriangles[a].verts[1] + pMesh->pTriangles[a].verts[2]) * (1.0f / 3.0f);

//				MFSetPosition(c);
//				MFSetPosition(c + pMesh->pTriangles[a].plane*3.0f);
			}

			MFEnd();
			break;
		}
		case MFCT_Field:
			break;
		default:
			break;
	}
}


/**** Support Functions ****/

MF_API void MFCollision_MakeCollisionTriangleFromPoints(const MFVector &p0, const MFVector& p1, const MFVector& p2, MFCollisionTriangle *pTri)
{
	// generate face plane
	pTri->plane = MFCollision_MakePlaneFromPoints(p0, p1, p2);

	// generate edge planes
	pTri->edgePlanes[0] = MFCollision_MakePlaneFromPointAndNormal(p0, (p0-p1).Cross3(pTri->plane));
	pTri->edgePlanes[1] = MFCollision_MakePlaneFromPointAndNormal(p1, (p1-p2).Cross3(pTri->plane));
	pTri->edgePlanes[2] = MFCollision_MakePlaneFromPointAndNormal(p2, (p2-p0).Cross3(pTri->plane));

	// copy verts
	pTri->verts[0] = p0;
	pTri->verts[1] = p1;
	pTri->verts[2] = p2;

	pTri->adjacent[0] = -1;
	pTri->adjacent[1] = -1;
	pTri->adjacent[2] = -1;

	pTri->flags = 0;
}


/**** Dynamic collision item functions ****/

MFCollisionItem* MFCollision_CreateCollisionItem()
{
	MFCollisionItem *pItem = (MFCollisionItem*)MFHeap_Alloc(sizeof(MFCollisionItem));
	gItemList.Create(pItem);
	return pItem;
}

void MFCollision_DestroyCollisionItem(MFCollisionItem *pItem)
{
	gItemList.Destroy(pItem);
	MFHeap_Free(pItem);
}

MF_API MFCollisionItem* MFCollision_CreateDynamicCollisionMesh(const char *pItemName, int numTris)
{
	MFDebug_Assert(numTris > 0, "Cant create collision mesh with no triangles..");

	MFCollisionItem *pItem;
	MFCollisionTemplate *pTemplate;
	MFCollisionMesh *pMesh;
	MFCollisionTriangle *pTris;

	pItem = MFCollision_CreateCollisionItem();
	pTemplate = (MFCollisionTemplate*)MFHeap_Alloc(MFALIGN16(sizeof(MFCollisionTemplate)) + MFALIGN16(sizeof(MFCollisionMesh)) + sizeof(MFCollisionTriangle)*numTris + MFString_Length(pItemName) + 1);
	pMesh = (MFCollisionMesh*)MFALIGN16(&pTemplate[1]);
	pTris = (MFCollisionTriangle*)MFALIGN16(&pMesh[1]);

	pItem->pTemplate = pTemplate;
	pItem->flags = 0;
	pItem->refCount = 1;
	pItem->worldPos = MFMatrix::identity;

	pTemplate->pCollisionTemplateData = pMesh;
	pTemplate->type = MFCT_Mesh;
	pTemplate->pName = (char*)&pTris[numTris];
	MFString_Copy((char*)pTemplate->pName, pItemName);

	pMesh->numTris = numTris;
	pMesh->pTriangles = pTris;

	return pItem;
}

void MFCollision_CalculateDynamicBoundingVolume(MFCollisionItem *pItem)
{
	switch(pItem->pTemplate->type)
	{
		case MFCT_Mesh:
		{
			MFCollisionMesh *pMesh = (MFCollisionMesh*)pItem->pTemplate->pCollisionTemplateData;
			MFBoundingVolume &vol = pItem->pTemplate->boundingVolume;

			vol.min = vol.max = vol.boundingSphere = MakeVector(pMesh->pTriangles[0].verts[0], 0.0f);

			for(int a=0; a<pMesh->numTris; a++)
			{
				MFCollisionTriangle &tri = pMesh->pTriangles[a];

				for(int b=0; b<3; b++)
				{
					vol.min = MFMin(vol.min, tri.verts[b]);
					vol.max = MFMin(vol.max, tri.verts[b]);
					vol.min.w = vol.max.w = 0.0f;

					// if point is outside bounding sphere
					MFVector diff = tri.verts[b] - vol.boundingSphere;
					float mag = diff.MagSquared3();

					if(mag > vol.boundingSphere.w*vol.boundingSphere.w)
					{
						// fit sphere to include point
						mag = MFSqrt(mag) - vol.boundingSphere.w;
						mag *= 0.5f;
						diff.Normalise3();
						vol.boundingSphere.Mad3(diff, mag, vol.boundingSphere);
						vol.boundingSphere.w += mag;
					}
				}
			}
			break;
		}

		default:
			MFDebug_Assert(false, "Invalid item type");
	}
}

MF_API MFCollisionTriangle* MFCollision_LockDynamicCollisionMeshTriangleBuffer(MFCollisionItem *pDynamicCollisionMesh)
{
	MFDebug_Assert(pDynamicCollisionMesh->pTemplate->type == MFCT_Mesh, "Collision item is not an MFCollisionMesh.");

	MFCollisionMesh *pMesh = (MFCollisionMesh*)pDynamicCollisionMesh->pTemplate->pCollisionTemplateData;
	return pMesh->pTriangles;
}

MF_API void MFCollision_UnlockDynamicCollisionMeshTriangleBuffer(MFCollisionItem *pDynamicCollisionMesh)
{
	MFCollision_CalculateDynamicBoundingVolume(pDynamicCollisionMesh);
}

MF_API void MFCollision_DestroyDynamicCollisionItem(MFCollisionItem *pItem)
{
	MFHeap_Free(pItem->pTemplate);
	MFCollision_DestroyCollisionItem(pItem);
}

/**** General collision tests ****/

bool MFCollision_RaySlabTest(const MFVector& rayPos, const MFVector& rayDir, const MFVector& plane, float slabHalfWidth, MFRayIntersectionResult *pResult)
{
	float a = plane.Dot3(rayDir);

	// if ray is parallel to plane
	if(a > -MFALMOST_ZERO && a < MFALMOST_ZERO)
	{
		// TODO: this is intentionally BROKEN
		// this is a near impossible case, and it adds a lot of junk to the function
/*
		if(MFAbs(rayPos.DotH(plane)) <= slabHalfWidth)
		{
			if(pResult)
			{
				pResult->time = 0.0f;
			}

			return true;
		}
*/
		return false;
	}

	// otherwise we can do the conventional test
	float inva = MFRcp(a);
	float t = -rayPos.DotH(plane);
	float t1 = (t + slabHalfWidth) * inva;
	float t2 = (t - slabHalfWidth) * inva;

	t = MFMin(t1, t2);
	t2 = MFMax(t1, t2);

	if(t > 1.0f || t2 < 0.0f)
		return false;

	if(pResult)
	{
		pResult->time = MFMax(t, 0.0f);
		pResult->surfaceNormal = a > 0.0f ? -plane : plane;
	}

	return true;
}

bool MFCollision_RaySphereTest(const MFVector& rayPos, const MFVector& rayDir, const MFVector& spherePos, float radius, MFRayIntersectionResult *pResult)
{
	MFVector diff = rayPos - spherePos;

	// calcuate the coefficients
	float a = rayDir.MagSquared3();
	float b = (2.0f*rayDir).Dot3(diff);
	float c = diff.MagSquared3() - radius*radius;

	// calculate the stuff under the root sign, if it's negative no (real) solutions exist
	float d = b*b - 4.0f*a*c;
	if(d < 0.0f) // this means ray misses cylinder
		return false;

	float root = MFSqrt(d);
	float rcp2a = MFRcp(2.0f*a);
	float t1 = (-b - root)*rcp2a;
	float t2 = (-b + root)*rcp2a;

	if(t2 < 0.0f || t1 > 1.0f)
		return false;

	if(pResult)
	{
		pResult->time = MFMax(t1, 0.0f);
		pResult->surfaceNormal.Mad3(rayDir, pResult->time, diff);
		pResult->surfaceNormal.Normalise3();
	}

	return true;
}

bool MFCollision_RayCylinderTest(const MFVector& rayPos, const MFVector& rayDir, const MFVector& cylinderPos, const MFVector& cylinderDir, float cylinderRadius, bool capped, MFRayIntersectionResult *pResult, float *pCylinderTime)
{
	MFVector local = rayPos - cylinderPos;

	float rayD = rayDir.Dot3(cylinderDir);
	float T0 = local.Dot3(cylinderDir);

	// bring T0 into 0.0-1.0 range
	float invMagSq = MFRcp(cylinderDir.MagSquared3());
	rayD *= invMagSq;
	T0 *= invMagSq;

	// calculate some intermediate vectors
	MFVector v1 = rayDir - rayD*cylinderDir;
	MFVector v2 = local - T0*cylinderDir;

	// calculate coeff in quadratic formula
	float a = v1.MagSquared3();
	float b = (2.0f*v1).Dot3(v2);
	float c = v2.MagSquared3() - cylinderRadius*cylinderRadius;

	// calculate the stuff under the root sign, if it's negative no (real) solutions exist
	float d = b*b - 4.0f*a*c;
	if(d < 0.0f) // this means ray misses cylinder
		return false;

	float root = MFSqrt(d);
	float rcp2a = MFRcp(2.0f*a);
	float t1 = (-b - root)*rcp2a;
	float t2 = (-b + root)*rcp2a;

	if(t1 > 1.0f || t2 < 0.0f)
		return false; // the cylinder is beyond the ray..

	if(capped || pCylinderTime || pResult)
	{
		float t = MFMax(t1, 0.0f);

		// get the t for the cylinders ray
		MFVector intersectedRay;
		intersectedRay.Mad3(rayDir, t, local);

		float ct = intersectedRay.Dot3(cylinderDir) * invMagSq;

		if(capped && (ct < 0.0f || ct > 1.0f))
		{
			// we need to test the caps

			// TODO: this is REALLY slow!! can be majorly improved!!

			// generate a plane for the cap
			MFVector point, plane;

			if(rayD > 0.0f)
			{
				// the near one
				point = cylinderPos;
				plane = MFCollision_MakePlaneFromPointAndNormal(point, -cylinderDir);
			}
			else
			{
				// the far one
				point = cylinderPos + cylinderDir;
				plane = MFCollision_MakePlaneFromPointAndNormal(point, cylinderDir);
			}

			// test the ray against the plane
			bool collide = MFCollision_RayPlaneTest(rayPos, rayDir, plane, pResult);

			if(collide)
			{
				// calculate the intersection point
				intersectedRay.Mad3(rayDir, pResult->time, rayPos);
				intersectedRay.Sub3(intersectedRay, point);

				// and see if its within the cylinders radius
				if(intersectedRay.MagSquared3() <= cylinderRadius * cylinderRadius)
				{
					return true;
				}
			}

			return false;
		}

		if(pResult)
		{
			pResult->time = t;
			pResult->surfaceNormal.Mad3(cylinderDir, -ct, intersectedRay);
			pResult->surfaceNormal.Normalise3();
		}

		if(pCylinderTime)
		{
			*pCylinderTime = ct;
		}
	}

	return true;
}

bool MFCollision_RayCapsuleTest(const MFVector& rayPos, const MFVector& rayDir, const MFVector& capsulePos, const MFVector& capsuleDir, float capsuleRadius, MFRayIntersectionResult *pResult)
{
	float ct;

	// get collision on the cylinder
	bool collide = MFCollision_RayCylinderTest(rayPos, rayDir, capsulePos, capsuleDir, capsuleRadius, false, pResult, &ct);

	if(!collide)
		return false;

	if(ct >= 0.0f && ct <= 1.0f)
	{
		// we have an intersection on the cylinders surface
		return true;
	}

	// intersection not on cylinder, test against end sphere..
	return MFCollision_RaySphereTest(rayPos, rayDir, (ct>0.0f) ? capsulePos + capsuleDir : capsulePos, capsuleRadius, pResult);
}

bool MFCollision_RayBoxTest(const MFVector& rayPos, const MFVector& rayDir, const MFVector& boxPos, const MFVector& boxRadius, MFRayIntersectionResult *pResult)
{
	MFVector plane[6];
	plane[0] = MakeVector(MFVector::up, boxPos.x + boxRadius.x);
	plane[1] = MakeVector(-MFVector::up, boxPos.x - boxRadius.x);
	return false;
}

bool MFCollision_SpherePlaneTest(const MFVector& spherePos, float radius, const MFVector& plane, MFCollisionResult *pResult)
{
	if(!pResult)
	{
		return spherePos.DotH(plane) < radius;
	}
	else
	{
		float d = spherePos.DotH(plane);

		pResult->bCollide = d < radius;

		if(pResult->bCollide)
		{
			pResult->depth = radius - d;
			pResult->normal = plane;
			pResult->intersectionPoint.Mad3(pResult->normal, -(d + pResult->depth*0.5f), spherePos);
		}

		return pResult->bCollide;
	}
}

bool MFCollision_SphereSphereTest(const MFVector &pos1, float radius1, const MFVector &pos2, float radius2, MFCollisionResult *pResult)
{
	MFVector diff = pos2 - pos1;

	if(!pResult)
	{
		return diff.MagSquared3() < radius1*radius1 + radius2*radius2;
	}
	else
	{
		float length = diff.Magnitude3();
		float totalRadius = radius1 + radius2;

		pResult->bCollide = length < totalRadius;

		if(pResult->bCollide)
		{
			pResult->depth = totalRadius - length;
			pResult->normal = -diff.Normalise3();
			pResult->intersectionPoint = diff * ((length / totalRadius) * (radius1 / radius2));
		}

		return pResult->bCollide;
	}
}


bool MFCollision_SweepSphereSphereTest(const MFVector &sweepSphere, const MFVector &sweepSphereVelocity, float sweepSphereRadius, const MFVector &sphere, float sphereRadius, MFSweepSphereResult *pResult)
{
	MFRayIntersectionResult r;
	bool collide = MFCollision_RaySphereTest(sweepSphere, sweepSphereVelocity, sphere, sphereRadius + sweepSphereRadius, &r);

	if(collide)
	{
		pResult->time = r.time;
		pResult->surfaceNormal = r.surfaceNormal;
		pResult->intersectionReaction = MFVector::zero;
		return true;
	}

	return false;
}

bool MFCollision_SweepSphereTriTest(const MFVector &sweepSpherePos, const MFVector &sweepSphereVelocity, float sweepSphereRadius, const MFCollisionTriangle &tri, MFSweepSphereResult *pResult)
{
	MFRayIntersectionResult result;

	// test the triangle surface
	if(!MFCollision_RaySlabTest(sweepSpherePos, sweepSphereVelocity, tri.plane, sweepSphereRadius, &result))
		return false;

	MFVector intersection = sweepSpherePos + sweepSphereVelocity * result.time;

	// test if intersection is inside the triangle
	float dot0, dot1, dot2;

	dot0 = intersection.DotH(tri.edgePlanes[0]);
//	if(dot0 > sphereRadius)
//		return false;

	dot1 = intersection.DotH(tri.edgePlanes[1]);
//	if(dot1 > sphereRadius)
//		return false;

	dot2 = intersection.DotH(tri.edgePlanes[2]);
//	if(dot2 > sphereRadius)
//		return false;

	// test if intersection is inside the triangle face
	if(dot0 >= 0.0f && dot1 >= 0.0f && dot2 >= 0.0f)
		goto collision;

	// test the 3 edges
	if(dot0 < 0.0f)
	{
		if(MFCollision_RayCapsuleTest(sweepSpherePos, sweepSphereVelocity, tri.verts[0], tri.verts[1]-tri.verts[0], sweepSphereRadius, &result))
			goto collision;
	}

	if(dot1 < 0.0f)
	{
		if(MFCollision_RayCapsuleTest(sweepSpherePos, sweepSphereVelocity, tri.verts[1], tri.verts[2]-tri.verts[1], sweepSphereRadius, &result))
			goto collision;
	}

	if(dot2 < 0.0f)
	{
		if(MFCollision_RayCapsuleTest(sweepSpherePos, sweepSphereVelocity, tri.verts[2], tri.verts[0]-tri.verts[2], sweepSphereRadius, &result))
			goto collision;
	}

	return false;

collision:
	if(result.time == 0.0f)
	{

		// this is for double sided collision.
		float dot = sweepSpherePos.DotH(tri.plane);
		float amountResolve = sweepSphereRadius - MFAbs(dot);
		pResult->intersectionReaction = result.surfaceNormal * amountResolve;
	}
	else
		pResult->intersectionReaction = MFVector::zero;

	pResult->surfaceNormal = result.surfaceNormal;
	pResult->time = result.time;
	return true;
}

bool MFCollision_RayTriTest(const MFVector& rayPos, const MFVector& rayDir, const MFVector& p0,  const MFVector& p1, const MFVector& p2, float *pT, float *pU, float *pV)
{
	MFVector edge1, edge2, tvec, pvec, qvec;
	float det, inv_det;
	float u, v;

	/* find vectors for two edges sharing vert0 */
	edge1 = p1 - p0;
	edge2 = p2 - p0;

	/* begin calculating determinant - also used to calculate U parameter */
	pvec.Cross3(rayDir, edge2);

	/* if determinant is near zero, ray lies in plane of triangle */
	det = edge1.Dot3(pvec);

	if(det > -MFALMOST_ZERO && det < MFALMOST_ZERO)
		return false;
	inv_det = 1.0f / det;

	/* calculate distance from vert0 to ray origin */
	tvec = rayPos - p0;

	/* calculate U parameter and test bounds */
	u = tvec.Dot3(pvec) * inv_det;
	if(u < 0.0f || u > 1.0f)
		return false;

	/* prepare to test V parameter */
	qvec.Cross3(tvec, edge1);

	/* calculate V parameter and test bounds */
	v = rayDir.Dot3(qvec) * inv_det;
	if(v < 0.0f || u + v > 1.0f)
		return false;

	/* calculate t, ray intersects triangle */
	if(pT) *pT = edge2.Dot3(qvec) * inv_det;

	if(pU)
	{
		*pU = u;
		*pV = v;
	}

	return true;
}

// culls backfaces
bool MFCollision_RayTriCullTest(const MFVector& rayPos, const MFVector& rayDir, const MFVector& p0,  const MFVector& p1, const MFVector& p2, float *pT, float *pU, float *pV, MFVector *pIntersectionPoint)
{
	MFVector	edge1, edge2, tvec, pvec, qvec;
	float	det, inv_det;
	float	u, v;

	/* find vectors for two edges sharing vert0 */
	edge1 = p1 - p0;
	edge2 = p2 - p0;

	/* begin calculating determinant - also used to calculate U parameter */
	pvec.Cross3(rayDir, edge2);

	/* if determinant is near zero, ray lies in plane of triangle */
	det = edge1.Dot3(pvec);

	if (det < MFALMOST_ZERO)
		return false;

	/* calculate distance from vert0 to ray origin */
	tvec = rayPos - p0;

	/* calculate U parameter and test bounds */
	u = tvec.Dot3(pvec);
	if (u < 0.0f || u > det)
		return false;

	/* prepare to test V parameter */
	qvec.Cross3(tvec, edge1);

	/* calculate V parameter and test bounds */
	v = rayDir.Dot3(qvec);
	if (v < 0.0f || u + v > det)
		return false;

	/* calculate t, scale parameters, ray intersects triangle */
	if(pIntersectionPoint || pT || pU)
	{
		inv_det = 1.0f / det;
		u *= inv_det;
		v *= inv_det;

		if(pT) *pT = edge2.Dot3(qvec) * inv_det;

		if(pU)
		{
			*pU = u;
			*pV = v;
		}

		if(pIntersectionPoint)
		{
			*pIntersectionPoint = p0*(1.0f-u-v) + p1*u + p2*v;
		}
	}

	return true;
}

bool MFCollision_SphereTriTest(const MFVector& sphere, const MFVector& p0,  const MFVector& p1, const MFVector& p2, MFVector *pIntersectionPoint)
{
	MFDebug_Assert(false, "Not Written!");
	return false;
}

bool MFCollision_PlaneTriTest(const MFVector& plane, const MFVector& p0,  const MFVector& p1, const MFVector& p2, MFVector *pIntersectionPoint)
{
	float t0 = p0.DotH(plane);
	float t1 = p1.DotH(plane);
	float t2 = p2.DotH(plane);

	if(t0 <= 0.0f && t1 <= 0.0f && t2 <= 0)
		return false;

	if(t0 >= 0.0f && t1 >= 0.0f && t2 >= 0)
		return false;

	if(pIntersectionPoint)
	{
		// TODO: calculate point
	}

	return true;
}

/* sort so that a<=b */
#define SORT(a,b)       \
             if(a>b)    \
             {          \
               float c; \
               c=a;     \
               a=b;     \
               b=c;     \
             }

/* this edge to edge test is based on Franlin Antonio's gem:
   "Faster Line Segment Intersection", in Graphics Gems III,
   pp. 199-202 */ 
#define EDGE_EDGE_TEST(V0,U0,U1)                      \
  Bx=U0[i0]-U1[i0];                                   \
  By=U0[i1]-U1[i1];                                   \
  Cx=V0[i0]-U0[i0];                                   \
  Cy=V0[i1]-U0[i1];                                   \
  f=Ay*Bx-Ax*By;                                      \
  d=By*Cx-Bx*Cy;                                      \
  if((f>0 && d>=0 && d<=f) || (f<0 && d<=0 && d>=f))  \
  {                                                   \
    e=Ax*Cy-Ay*Cx;                                    \
    if(f>0)                                           \
    {                                                 \
      if(e>=0 && e<=f) return true;                   \
    }                                                 \
    else                                              \
    {                                                 \
      if(e<=0 && e>=f) return true;                   \
    }                                                 \
  }

#define EDGE_AGAINST_TRI_EDGES(V0,V1,U0,U1,U2) \
{                                              \
  float Ax,Ay,Bx,By,Cx,Cy,e,d,f;               \
  Ax=V1[i0]-V0[i0];                            \
  Ay=V1[i1]-V0[i1];                            \
  /* test edge U0,U1 against V0,V1 */          \
  EDGE_EDGE_TEST(V0,U0,U1);                    \
  /* test edge U1,U2 against V0,V1 */          \
  EDGE_EDGE_TEST(V0,U1,U2);                    \
  /* test edge U2,U1 against V0,V1 */          \
  EDGE_EDGE_TEST(V0,U2,U0);                    \
}

#define POINT_IN_TRI(V0,U0,U1,U2)           \
{                                           \
  float a,b,c,d0,d1,d2;                     \
  /* is T1 completly inside T2? */          \
  /* check if V0 is inside tri(U0,U1,U2) */ \
  a=U1[i1]-U0[i1];                          \
  b=-(U1[i0]-U0[i0]);                       \
  c=-a*U0[i0]-b*U0[i1];                     \
  d0=a*V0[i0]+b*V0[i1]+c;                   \
                                            \
  a=U2[i1]-U1[i1];                          \
  b=-(U2[i0]-U1[i0]);                       \
  c=-a*U1[i0]-b*U1[i1];                     \
  d1=a*V0[i0]+b*V0[i1]+c;                   \
                                            \
  a=U0[i1]-U2[i1];                          \
  b=-(U0[i0]-U2[i0]);                       \
  c=-a*U2[i0]-b*U2[i1];                     \
  d2=a*V0[i0]+b*V0[i1]+c;                   \
  if(d0*d1>0.0)                             \
  {                                         \
    if(d0*d2>0.0) return true;              \
  }                                         \
}

bool coplanar_tri_tri(const MFVector& N, const MFVector& V0, const MFVector& V1, const MFVector& V2, const MFVector& U0, const MFVector& U1, const MFVector& U2)
{
	MFVector A;
	short i0,i1;

	/* first project onto an axis-aligned plane, that maximizes the area */
	/* of the triangles, compute indices: i0,i1. */
	A.x=fabsf(N.x);
	A.y=fabsf(N.y);
	A.z=fabsf(N.z);

	if(A.x>A.y)
	{
		if(A.x>A.z)  
		{
			i0=1;      /* A[0] is greatest */
			i1=2;
		}
		else
		{
			i0=0;      /* A[2] is greatest */
			i1=1;
		}
	}
	else   /* A[0]<=A[1] */
	{
		if(A.z>A.y)
		{
			i0=0;      /* A[2] is greatest */
			i1=1;
		}
		else
		{
			i0=0;      /* A[1] is greatest */
			i1=2;
		}
	}

	/* test all edges of triangle 1 against the edges of triangle 2 */
	EDGE_AGAINST_TRI_EDGES(V0,V1,U0,U1,U2);
	EDGE_AGAINST_TRI_EDGES(V1,V2,U0,U1,U2);
	EDGE_AGAINST_TRI_EDGES(V2,V0,U0,U1,U2);

	/* finally, test if tri1 is totally contained in tri2 or vice versa */
	POINT_IN_TRI(V0,U0,U1,U2);
	POINT_IN_TRI(U0,V0,V1,V2);

	return false;
}

#define NEWCOMPUTE_INTERVALS(VV0,VV1,VV2,D0,D1,D2,D0D1,D0D2,A,B,C,X0,X1) \
{ \
	if(D0D1>0.0f) \
	{ \
		/* here we know that D0D2<=0.0 */ \
		/* that is D0, D1 are on the same side, D2 on the other or on the plane */ \
		A=VV2; B=(VV0-VV2)*D2; C=(VV1-VV2)*D2; X0=D2-D0; X1=D2-D1; \
	} \
	else if(D0D2>0.0f)\
	{ \
		/* here we know that d0d1<=0.0 */ \
		A=VV1; B=(VV0-VV1)*D1; C=(VV2-VV1)*D1; X0=D1-D0; X1=D1-D2; \
	} \
	else if(D1*D2>0.0f || D0!=0.0f) \
	{ \
		/* here we know that d0d1<=0.0 or that D0!=0.0 */ \
		A=VV0; B=(VV1-VV0)*D0; C=(VV2-VV0)*D0; X0=D0-D1; X1=D0-D2; \
	} \
	else if(D1!=0.0f) \
	{ \
		A=VV1; B=(VV0-VV1)*D1; C=(VV2-VV1)*D1; X0=D1-D0; X1=D1-D2; \
	} \
	else if(D2!=0.0f) \
	{ \
		A=VV2; B=(VV0-VV2)*D2; C=(VV1-VV2)*D2; X0=D2-D0; X1=D2-D1; \
	} \
	else \
	{ \
		/* triangles are coplanar */ \
		return coplanar_tri_tri(N1,V0,V1,V2,U0,U1,U2); \
	} \
}

bool MFCollision_TriTriTest(const MFVector& V0,  const MFVector& V1, const MFVector& V2, const MFVector& U0,  const MFVector& U1, const MFVector& U2)
{
	MFVector E1, E2;
	MFVector N1, N2;
	MFVector D;

	float d1,d2;
	float du0,du1,du2,dv0,dv1,dv2;
	float isect1[2], isect2[2];
	float du0du1,du0du2,dv0dv1,dv0dv2;
	short index;
	float vp0,vp1,vp2;
	float up0,up1,up2;
	float bb,cc,max;
	float a,b,c,x0,x1;
	float d,e,f,y0,y1;
	float xx,yy,xxyy,tmp;

	/* compute plane equation of triangle(V0,V1,V2) */
	E1 = V1-V0;
	E2 = V2-V0;
	N1.Cross3(E1, E2);
	d1 = -N1.Dot3(V0);
	/* plane equation 1: N1.X+d1=0 */

	/* put U0,U1,U2 into plane equation 1 to compute signed distances to the plane*/
	du0 = N1.Dot3(U0) + d1;
	du1 = N1.Dot3(U1) + d1;
	du2 = N1.Dot3(U2) + d1;

	/* coplanarity robustness check */
	if(fabsf(du0)<MFALMOST_ZERO) du0=0.0f;
	if(fabsf(du1)<MFALMOST_ZERO) du1=0.0f;
	if(fabsf(du2)<MFALMOST_ZERO) du2=0.0f;

	du0du1 = du0*du1;
	du0du2 = du0*du2;

	if(du0du1>0.0f && du0du2>0.0f)	/* same sign on all of them + not equal 0 ? */
		return false;				/* no intersection occurs */

	/* compute plane of triangle (U0,U1,U2) */
	E1 = U1 - U0;
	E2 = U2 - U0;
	N2.Cross3(E1, E2);
	d2 = -N2.Dot3(U0);
	/* plane equation 2: N2.X+d2=0 */

	/* put V0,V1,V2 into plane equation 2 */
	dv0 = N2.Dot3(V0) + d2;
	dv1 = N2.Dot3(V1) + d2;
	dv2 = N2.Dot3(V2) + d2;

	if(fabsf(dv0)<MFALMOST_ZERO) dv0=0.0;
	if(fabsf(dv1)<MFALMOST_ZERO) dv1=0.0;
	if(fabsf(dv2)<MFALMOST_ZERO) dv2=0.0;

	dv0dv1 = dv0*dv1;
	dv0dv2 = dv0*dv2;

	if(dv0dv1>0.0f && dv0dv2>0.0f)	/* same sign on all of them + not equal 0 ? */
		return false;				/* no intersection occurs */

	/* compute direction of intersection line */
	D.Cross3(N1, N2);

	/* compute and index to the largest component of D */
	max = fabsf(D.x);
	index = 0;
	bb = fabsf(D.y);
	cc = fabsf(D.z);
	if(bb>max) max = bb, index = 1;
	if(cc>max) max = cc, index = 2;

	/* this is the simplified projection onto L*/
	vp0 = V0[index];
	vp1 = V1[index];
	vp2 = V2[index];

	up0 = U0[index];
	up1 = U1[index];
	up2 = U2[index];

	/* compute interval for triangle 1 */
	NEWCOMPUTE_INTERVALS(vp0,vp1,vp2,dv0,dv1,dv2,dv0dv1,dv0dv2,a,b,c,x0,x1);

	/* compute interval for triangle 2 */
	NEWCOMPUTE_INTERVALS(up0,up1,up2,du0,du1,du2,du0du1,du0du2,d,e,f,y0,y1);

	xx=x0*x1;
	yy=y0*y1;
	xxyy=xx*yy;

	tmp=a*xxyy;
	isect1[0]=tmp+b*x1*yy;
	isect1[1]=tmp+c*x0*yy;

	tmp=d*xxyy;
	isect2[0]=tmp+e*xx*y1;
	isect2[1]=tmp+f*xx*y0;

	SORT(isect1[0],isect1[1]);
	SORT(isect2[0],isect2[1]);

	if(isect1[1]<isect2[0] || isect2[1]<isect1[0]) return false;

	return true;
}

bool MFCollision_RayHull()
{
	MFDebug_Assert(false, "Not Written!");
	return false;
}

bool MFCollision_SphereHull()
{
	MFDebug_Assert(false, "Not Written!");
	return false;
}

bool MFCollision_HullHull()
{
	MFDebug_Assert(false, "Not Written!");
	return false;
}

bool MFCollision_TriHull()
{
	MFDebug_Assert(false, "Not Written!");
	return false;
}


/**** Manager ****/

MF_API MFCollisionItem* MFCollision_RayTest(const MFVector& rayPos, const MFVector& rayDir, MFCollisionItem *pItem, MFRayIntersectionResult *pResult)
{
	MFBoundingVolume rayVolume;

	MFVector rayEnd = rayPos+rayDir;
	rayVolume.min = MFMin(rayPos, rayEnd);
	rayVolume.max = MFMax(rayPos, rayEnd);
	rayVolume.boundingSphere = MakeVector(rayPos + rayDir*0.5f, rayDir.Magnitude3()*0.5f);

	if(!MFBoundingVolume_Test(rayVolume, pItem->pTemplate->boundingVolume))
	{
		return NULL;
	}

	switch(pItem->pTemplate->type)
	{
		case MFCT_Sphere:
		{
			MFCollisionSphere *pSphere = (MFCollisionSphere*)pItem->pTemplate->pCollisionTemplateData;
			if(!MFCollision_RaySphereTest(rayPos, rayDir, pItem->worldPos.GetTrans(), pSphere->radius, pResult))
				return NULL;
			break;
		}
		case MFCT_Mesh:
		{
			if(!MFCollision_RayMeshTest(rayPos, rayDir, pItem, pResult))
				return NULL;
			break;
		}
		case MFCT_Field:
		{
			pItem = MFCollision_RayFieldTest(rayPos, rayDir, pItem, pResult);
			break;
		}
		default:
			MFDebug_Assert(false, "Invalid target primitive");
	}

	return pItem;
}

MF_API MFCollisionItem* MFCollision_SphereTest(const MFVector &spherePos, float radius, MFCollisionItem *pItem, MFCollisionResult *pResult)
{
	MFBoundingVolume rayVolume;

	MFVector radiusVector = MakeVector(radius, radius, radius, 0.0f);
	rayVolume.min = spherePos - radiusVector;
	rayVolume.max = spherePos + radiusVector;
	rayVolume.boundingSphere = MakeVector(spherePos, radius);

	if(!MFBoundingVolume_Test(rayVolume, pItem->pTemplate->boundingVolume))
	{
		return NULL;
	}

	switch(pItem->pTemplate->type)
	{
		case MFCT_Sphere:
		{
			MFCollisionSphere *pSphere = (MFCollisionSphere*)pItem->pTemplate->pCollisionTemplateData;
			if(!MFCollision_SphereSphereTest(spherePos, radius, pItem->worldPos.GetTrans(), pSphere->radius, pResult))
				return NULL;
			break;
		}
		case MFCT_Mesh:
		{
			if(!MFCollision_SphereMeshTest(spherePos, radius, pItem, pResult))
				return NULL;
			break;
		}
		case MFCT_Field:
		{
			pItem = MFCollision_SphereFieldTest(spherePos, radius, pItem, pResult);
			break;
		}
		default:
			MFDebug_Assert(false, "Invalid target primitive");
	}

	return pItem;
}

MF_API MFCollisionItem* MFCollision_SweepSphereTest(const MFVector &sweepSpherePos, const MFVector &sweepSphereVelocity, float sweepSphereRadius, MFCollisionItem *pItem, MFSweepSphereResult *pResult)
{
	MFBoundingVolume rayVolume;

	MFVector radiusVector = MakeVector(sweepSphereRadius, sweepSphereRadius, sweepSphereRadius, 0.0f);
	MFVector sweepEnd = sweepSpherePos + sweepSphereVelocity;
	rayVolume.min = MFMin(sweepSpherePos, sweepEnd) - radiusVector;
	rayVolume.max = MFMax(sweepSpherePos, sweepEnd) + radiusVector;
	rayVolume.boundingSphere = MakeVector(sweepSpherePos + sweepSphereVelocity*0.5f, sweepSphereVelocity.Magnitude3()*0.5f + sweepSphereRadius);

	if(0)//!MFBoundingVolume_Test(rayVolume, pItem->pTemplate->boundingVolume))
	{
		return NULL;
	}

	switch(pItem->pTemplate->type)
	{
		case MFCT_Sphere:
		{
			MFCollisionSphere *pSphere = (MFCollisionSphere*)pItem->pTemplate->pCollisionTemplateData;
			if(!MFCollision_SweepSphereSphereTest(sweepSpherePos, sweepSphereVelocity, sweepSphereRadius, pItem->worldPos.GetTrans(), pSphere->radius, pResult))
				return NULL;
			break;
		}
		case MFCT_Mesh:
		{
			if(!MFCollision_SweepSphereMeshTest(sweepSpherePos, sweepSphereVelocity, sweepSphereRadius, pItem, pResult))
				return NULL;
			break;
		}
		case MFCT_Field:
		{
			pItem = MFCollision_SweepSphereFieldTest(sweepSpherePos, sweepSphereVelocity, sweepSphereRadius, pItem, pResult);
			break;
		}
		default:
			MFDebug_Assert(false, "Invalid target primitive");
	}

	return pItem;
}

bool MFCollision_RayMeshTest(const MFVector& rayPos, const MFVector& rayDir, MFCollisionItem *pMesh, MFRayIntersectionResult *pResult)
{
	MFDebug_Assert(pMesh->pTemplate->type == MFCT_Mesh, "Item is not a collision mesh");

	MFDebug_Assert(false, "No dice..");

	return false;
}

bool MFCollision_SphereMeshTest(const MFVector &spherePos, float radius, MFCollisionItem *pMesh, MFCollisionResult *pResult)
{
	MFDebug_Assert(pMesh->pTemplate->type == MFCT_Mesh, "Item is not a collision mesh");

	MFDebug_Assert(false, "No dice..");

	return false;
}

bool MFCollision_SweepSphereMeshTest(const MFVector &sweepSpherePos, const MFVector &sweepSphereVelocity, float sweepSphereRadius, MFCollisionItem *pMesh, MFSweepSphereResult *pResult)
{
	MFDebug_Assert(pMesh->pTemplate->type == MFCT_Mesh, "Item is not a collision mesh");

	MFCollisionMesh *pMeshData = (MFCollisionMesh*)pMesh->pTemplate->pCollisionTemplateData;

	MFSweepSphereResult result;
	result.time = 1.0f;
	bool intersected = false;
	int tri = -1;

	for(int a=0; a<pMeshData->numTris; a++)
	{
		MFSweepSphereResult r;
		bool collide = MFCollision_SweepSphereTriTest(sweepSpherePos, sweepSphereVelocity, sweepSphereRadius, pMeshData->pTriangles[a], &r);

		if(collide)
		{
			if(r.time <= result.time)
			{
				result.time = r.time;
				result.surfaceNormal = r.surfaceNormal;
				result.intersectionReaction = r.intersectionReaction;
				intersected = true;
				tri = a;
			}
		}
	}

	pMeshData->pTriangles[tri].flags = 1;

	if(intersected && pResult)
	{
		pResult->time = result.time;
		pResult->surfaceNormal = result.surfaceNormal;
		pResult->intersectionReaction = result.intersectionReaction;
	}

	return intersected;
}

MFCollisionItem* MFCollision_RayFieldTest(const MFVector& rayPos, const MFVector& rayDir, MFCollisionItem *pField, MFRayIntersectionResult *pResult)
{
	MFDebug_Assert(pField->pTemplate->type == MFCT_Field, "pField is not a collision field.");

	MFCollisionField *pFieldData = (MFCollisionField*)pField->pTemplate->pCollisionTemplateData;
	MFCollisionItem *pItem = NULL;

	MFCollisionItem **ppIterator = pFieldData->itemList.Begin();

	while(*ppIterator)
	{
		pItem = MFCollision_RayTest(rayPos, rayDir, *ppIterator, pResult);

		ppIterator++;
	}

	return pItem;
}

MFCollisionItem* MFCollision_SphereFieldTest(const MFVector &spherePos, float radius, MFCollisionItem *pField, MFCollisionResult *pResult)
{
	MFDebug_Assert(pField->pTemplate->type == MFCT_Field, "pField is not a collision field.");

	MFCollisionField *pFieldData = (MFCollisionField*)pField->pTemplate->pCollisionTemplateData;
	MFCollisionItem *pItem = NULL;

	MFCollisionItem **ppIterator = pFieldData->itemList.Begin();

	while(*ppIterator)
	{
		pItem = MFCollision_SphereTest(spherePos, radius, *ppIterator, pResult);

		ppIterator++;
	}

	return pItem;
}

MFCollisionItem* MFCollision_SweepSphereFieldTest(const MFVector &sweepSpherePos, const MFVector &sweepSphereVelocity, float sweepSphereRadius, MFCollisionItem *pField, MFSweepSphereResult *pResult)
{
	MFDebug_Assert(pField->pTemplate->type == MFCT_Field, "pField is not a collision field.");

	MFCollisionField *pFieldData = (MFCollisionField*)pField->pTemplate->pCollisionTemplateData;
	MFCollisionItem *pItem = NULL;

	MFCollisionItem **ppIterator = pFieldData->itemList.Begin();

	MFSweepSphereResult result;
	result.time = 1.0f;

	while(*ppIterator)
	{
		MFSweepSphereResult r;
		MFCollisionItem *pI = *ppIterator;

		pI = MFCollision_SweepSphereTest(sweepSpherePos, sweepSphereVelocity, sweepSphereRadius, pI, &r);

		if(pI)
		{
			if(r.time <= result.time)
			{
				pItem = pI;
				result.time = r.time;
				result.surfaceNormal = r.surfaceNormal;
				result.intersectionReaction = r.intersectionReaction;
			}
		}

		ppIterator++;
	}

	if(pItem && pResult)
	{
		pResult->time = result.time;
		pResult->surfaceNormal = result.surfaceNormal;
		pResult->intersectionReaction = result.intersectionReaction;
	}

	return pItem;
}

MF_API MFCollisionItem* MFCollision_CreateField(const char *pFieldName, int maximumItemCount, const MFVector &cellSize)
{
	MFCollisionItem *pItem;
	MFCollisionTemplate *pTemplate;
	MFCollisionField *pField;

	pItem = MFCollision_CreateCollisionItem();
	pTemplate = (MFCollisionTemplate*)MFHeap_Alloc(sizeof(MFCollisionTemplate) + sizeof(MFCollisionField) + MFString_Length(pFieldName) + 1);
	pField = (MFCollisionField*)&pTemplate[1];

	pItem->pTemplate = pTemplate;
	pItem->flags = 0;
	pItem->refCount = 1;
	pItem->worldPos = MFMatrix::identity;

	pTemplate->pCollisionTemplateData = pField;
	pTemplate->type = MFCT_Field;
	pTemplate->pName = (char*)&pField[1];
	MFString_Copy((char*)pTemplate->pName, pFieldName);

	pField->itemList.Init("Collision Field Items", maximumItemCount);
	pField->pppItems = NULL;
	pField->cellSize = cellSize;

	return pItem;
}

MF_API void MFCollision_AddItemToField(MFCollisionItem *pField, MFCollisionItem *pItem, uint32 itemFlags)
{
	MFDebug_Assert(pField->pTemplate->type == MFCT_Field, "pField is not a collision field.");

	MFCollisionField *pFieldData = (MFCollisionField*)pField->pTemplate->pCollisionTemplateData;

	pItem->flags = (uint16)itemFlags;
	pFieldData->itemList.Create(pItem);
}

MF_API void MFCollision_AddModelToField(MFCollisionItem *pField, MFModel *pModel)
{
	MFDebug_Assert(pField->pTemplate->type == MFCT_Field, "pField is not a collision field.");

	MFModelDataChunk *pCollision = MFModel_GetDataChunk(pModel->pTemplate, MFChunkType_Collision);

	if(pCollision)
	{
		MFCollisionTemplate *pTemplate = (MFCollisionTemplate*)pCollision->pData;

		for(int a=0; a<pCollision->count; a++)
		{
			MFCollisionItem *pItem = MFCollision_CreateCollisionItem();

			pItem->pTemplate = &pTemplate[a];
			pItem->worldPos = pModel->worldMatrix;
			pItem->flags = 0;
			pItem->refCount = 1;

			MFCollision_AddItemToField(pField, pItem, 0);
		}
	}
}

MF_API void MFCollision_BuildField(MFCollisionItem *pField)
{
	MFCollisionField *pFieldData = (MFCollisionField*)pField->pTemplate;

	int numItems = pFieldData->itemList.GetLength();

	if(numItems <= 0)
	{
		MFDebug_Warn(4, "EmptyField can not be generated.");
		return;
	}

	// find the min and max range of the objects
	MFVector fieldMin = MakeVector(10e+30f), fieldMax = MakeVector(-10e+30f);

	MFCollisionItem **ppI = pFieldData->itemList.Begin();

	while(*ppI)
	{
		MFCollisionItem *pI = *ppI;
		MFCollisionTemplate *pT = pI->pTemplate;

		MFVector tMin = ApplyMatrixH(pT->boundingVolume.min, pI->worldPos);
		MFVector tMax = ApplyMatrixH(pT->boundingVolume.max, pI->worldPos);

		fieldMin = MFMin(fieldMin, tMin);
		fieldMax = MFMax(fieldMax, tMax);

		ppI++;
	}

	pFieldData->fieldMin = fieldMin;
	pFieldData->fieldMax = fieldMin;

	MFVector numCells;
	MFVector fieldRange = fieldMax - fieldMin;
	numCells.Rcp3(pFieldData->cellSize);
	numCells.Mul3(fieldRange, numCells);

	pFieldData->width = (int)MFCeil(numCells.x);
	pFieldData->height = (int)MFCeil(numCells.y);
	pFieldData->depth = (int)MFCeil(numCells.z);

	// this is TOTALLY broken!! .. if a big object lies in many cell's, it could easilly overflow the array.
	int totalCells = pFieldData->width * pFieldData->height * pFieldData->depth;
	int numPointers = totalCells * 2 + numItems * 16;

	MFCollisionItem **ppItems = (MFCollisionItem**)MFHeap_Alloc(sizeof(MFCollisionItem*) * numPointers);
	pFieldData->pppItems = (MFCollisionItem***)ppItems;
	ppItems += totalCells;

	for(int z=0; z<pFieldData->depth; z++)
	{
		for(int y=0; y<pFieldData->height; y++)
		{
			for(int x=0; x<pFieldData->width; x++)
			{
				pFieldData->pppItems[z*pFieldData->height*pFieldData->width + y*pFieldData->width + x] = ppItems;

				MFVector thisCell = fieldMin + pFieldData->cellSize * MakeVector((float)x, (float)y, (float)z);
				MFVector thisCellEnd = thisCell + pFieldData->cellSize;

				MFCollisionItem **ppI = pFieldData->itemList.Begin();

				while(*ppI)
				{
					MFCollisionItem *pI = *ppI;
					MFCollisionTemplate *pT = pI->pTemplate;

					// if this item fits in this cell, insert it into this cells list.
					MFVector tMin = ApplyMatrixH(pT->boundingVolume.min, pI->worldPos);
					MFVector tMax = ApplyMatrixH(pT->boundingVolume.max, pI->worldPos);

					// test of bounding boxes overlap
					if(MFCollision_TestAABB(tMin, tMax, thisCell, thisCellEnd))
					{
						*ppItems = pI;
						++ppItems;
					}

					ppI++;
				}

				*ppItems = NULL;
				++ppItems;
			}
		}
	}

	MFHeap_ValidateMemory(pFieldData->pppItems);
}

MF_API void MFCollision_ClearField(MFCollisionItem *pField)
{
	MFCollisionField *pFieldData = (MFCollisionField*)pField->pTemplate->pCollisionTemplateData;

	if(pFieldData->pppItems)
	{
		MFHeap_Free(pFieldData->pppItems);
		pFieldData->pppItems = NULL;
	}

	pFieldData->itemList.Clear();
}

MF_API void MFCollision_DestroyField(MFCollisionItem *pField)
{
	MFCollision_ClearField(pField);

	MFCollisionField *pFieldData = (MFCollisionField*)pField->pTemplate->pCollisionTemplateData;
	pFieldData->itemList.Deinit();

	MFHeap_Free(pField->pTemplate);
	MFCollision_DestroyCollisionItem(pField);
}
