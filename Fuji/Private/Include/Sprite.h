#if !defined(_SPRITE_H)
#define _SPRITE_H

#include "Material.h"
#include "Vector3.h"

class Sprite
{
public:
	void Create(const char *filename, int xFrame = 1, int yFrames = 1, uint32 colourKey = 0x00000000);
	void Draw();
	void Release();

	void SetFlag(uint32 flag, bool enable = true);

	void Move(const Vector3 &pos);
	Vector3 GetPos(); 

	void Scale(const Vector3 &pos);
	Vector3 GetScale();

	void SetPivot(const Vector3 &pivot);
	Vector3 GetPivot();

	void Rotate(float angle);
	float GetRotation();

	void SetFrame(int frame);

	void Enable(bool enable = true);

private:
	Vector3 pivot;
	Vector3 position;
	Vector3 scale;
	float angle;

	int xFrames, yFrames;
	int frame;

	uint32 colourKey;

	uint32 flags;

	bool visible;

	Material *pMaterial;
};

#endif // _SPRITE_H
