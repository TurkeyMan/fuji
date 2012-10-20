module fuji.quaternion;

public import fuji.vector;

struct MFQuaternion
{
	float x = 0;
	float y = 0;
	float z = 0;
	float w = 1;

	immutable MFQuaternion identity = MFQuaternion.init;

/+
	// general purpose quaternion operators
	bool operator==(const MFQuaternion &q) const;
	bool operator!=(const MFQuaternion &q) const;

	void SetIdentity();

	void Set(float x, float y, float z, float w);

	MFQuaternion& operator=(const MFQuaternion &q);

	MFQuaternion operator+(const MFQuaternion &q) const;
	MFQuaternion operator-(const MFQuaternion &q) const;
	MFQuaternion operator*(float f) const;
	MFQuaternion operator*(const MFQuaternion &q) const;

	MFQuaternion& operator+=(const MFQuaternion &q);
	MFQuaternion& operator-=(const MFQuaternion &q);
	MFQuaternion& operator*=(float f);
	MFQuaternion& operator*=(const MFQuaternion &q);

	float Dot(const MFQuaternion &q) const;

	MFQuaternion& Multiply(const MFQuaternion &q1, const MFQuaternion &q2);
	MFQuaternion& Multiply(const MFQuaternion &q);

	MFQuaternion& Conjugate(const MFQuaternion &q);
	MFQuaternion& Conjugate();

	MFVector Apply(const MFVector &v) const;

	MFQuaternion& Slerp(const MFQuaternion &q1, const MFQuaternion &q2, float t);
	MFQuaternion& Slerp(const MFQuaternion &q, float t);

	const char * ToString() const;
+/
}

