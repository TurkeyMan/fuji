#if !defined(_READMEMD2_H)
#define _READMEMD2_H

#define MEMD_ID (('M')|('E'<<8)|('M'<<16)|('D'<<24))

#define MEMD_NOCULL			0x01
#define MEMD_NOZ			0x02
#define MEMD_NOZWRITE		0x04
#define MEMD_NOALPHABLEND	0x08
#define MEMD_NOLIGHT		0x10
#define MEMD_NOTEXTURE		0x20
#define MEMD_SHOWNORMALS	0x40

#define MEMD202_HASTAGS		0x00100000
#define MEMD202_HASCAMERA	0x00200000
#define MEMD202_HASLIGHTS	0x00400000

struct v4
{
	float r,g,b,a;
};

struct v3
{
	float x,y,z;
};

struct v2
{
	float x,y;
};

struct MEHeader
{
	int ID;
	short Version;
	short Flags;
	char Name[64];
	char Creator[64];

	int SequenceCount;
	int MaterialCount;
	int FrameCount;

	int MeshCount;

	unsigned long SequenceStart;
	unsigned long MaterialStart;
	unsigned long FrameStart;
};

struct MESequence2
{
	char Name[64];
	int StartFrame;
	int FrameCount;
	bool Looping;
	int FrameRate;
};

struct MEMaterial2
{
	char Name[64];
	v4 matColours[4];
	float matPower;
	int TextureCount;

	char TextureFileName[256];
	unsigned long ImageLen;
	char ImageBuffer[1];
};

struct MEMD2_VERTEX
{
	unsigned short v[3];
	unsigned char n[2];
	unsigned short uv[2];
};

struct MEMesh2
{
	char Name[64];
	int MaterialID;

	int vcount;
	int icount;

	v3 Base;
	v3 Scale;

	v2 UVBase;
	v2 UVScale;
};

#endif
