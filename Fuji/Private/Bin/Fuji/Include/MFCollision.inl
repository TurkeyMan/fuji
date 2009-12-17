// inline function definitions

// returns nearest point on a line segment [a,b]
inline MFVector& MFCollision_NearestPointOnLine(const MFVector& lineStart, const MFVector& lineEnd, const MFVector& point, MFVector *pOutput)
{
	// calculate lines ray
	MFVector ray = lineEnd - lineStart;

	// see if a is the nearest point
	float dot_ta = ray.Dot3(point - lineStart);
	if(dot_ta <= 0.0f)
		return *pOutput = lineStart;

	// see if b is the nearest point
	float dot_tb = (-ray).Dot3(point - lineEnd);
	if(dot_tb <= 0.0f)
		return *pOutput = lineEnd;

	// return nearest point on line segment
	*pOutput = lineStart + (ray * dot_ta*(1.0f / (dot_ta + dot_tb)));
	return *pOutput;
}

inline MFVector MFCollision_MakePlaneFromPoints(const MFVector &p0, const MFVector &p1, const MFVector &p2)
{
	MFVector p;

	p.Cross3(p1-p0, p2-p0);
	p.Normalise3();
	p.w = -p.Dot3(p0);

	return p;
}

inline MFVector MFCollision_MakePlaneFromPointAndNormal(const MFVector &point, const MFVector &normal)
{
	MFVector p;

	p = normal;
	p.Normalise3();
	p.w = -point.Dot3(p);

	return p;
}

inline bool MFCollision_RayPlaneTest(const MFVector& rayPos, const MFVector& rayDir, const MFVector& plane, MFRayIntersectionResult *pResult)
{
	float a = plane.Dot3(rayDir);

	// if ray is parallel to plane
	if(a > -MFALMOST_ZERO && a < MFALMOST_ZERO)
		return false;

	float t = -rayPos.DotH(plane) / a;

	if(t < 0.0f || t > 1.0f)
		return false;

	if(pResult)
	{
		pResult->time = t;
		pResult->surfaceNormal = plane;
	}

	return true;
}

inline bool MFCollision_TestAABB(const MFVector &min1, const MFVector &max1, const MFVector &min2, const MFVector &max2)
{
	if(max1.x > min2.x && min1.x < max2.x &&
		max1.y > min2.y && min1.y < max2.y &&
		  max1.z > min2.z && min1.z < max2.z)
	{
		return true;
	}

	return false;
}
