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
