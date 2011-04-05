#if !defined(_SPRITE_H)
#define _SPRITE_H

#include "MFMaterial.h"
#include "MFVector.h"

class Sprite
{
public:
	void Create(const char *filename, int xFrame = 1, int yFrames = 1, uint32 colourKey = 0x00000000);
	void Draw();
	void Release();

	void SetFlag(uint32 flag, bool enable = true);

	void Move(const MFVector &pos);
	MFVector GetPos(); 

	void Scale(const MFVector &pos);
	MFVector GetScale();

	void SetPivot(const MFVector &pivot);
	MFVector GetPivot();

	void Rotate(float angle);
	float GetRotation();

	void SetFrame(int frame);

	void Enable(bool enable = true);

private:
	MFVector pivot;
	MFVector position;
	MFVector scale;
	float angle;

	int xFrames, yFrames;
	int frame;

	uint32 colourKey;

	uint32 flags;

	bool visible;

	MFMaterial *pMaterial;
};

#endif // _SPRITE_H
