#if !defined(_MODEL_H)
#define _MODEL_H

#include "Matrix.h"

class Animation;

class ModelData
{
public:
	uint32 IDtag;
	char *pName;

	uint16 subobjectCount;
	uint16 materialCount;
	uint16 boneCount;
	uint16 customDataCount;
	uint32 flags;

	struct Subobject
	{
		uint16 vertexFormat;
		uint16 vertexSize;
		uint32 vertexCount;
		void *pVertexData;
	} *pSubobject;

	struct CustomData
	{
		uint16 customDataType;
		uint16 ustomDataCount;
		void *pData;
		uint32 res;
		uint32 res2;
	} *pCustomData;

	void FixUpPointers();
	void CollapsePointers();
};

class Model
{
public:
	Matrix worldMatrix;

	ModelData *pModelData;
	Animation *pAnimation;
};

#endif
