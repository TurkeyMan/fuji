#include "Fuji.h"
#include "F3D.h"

extern F3DFile *pModel;

/* vector */
typedef float vec3_t[3];

/* texture name */
struct md2_skin_t
{
	char name[64];   /* texture file name */
};

/* texture coords */
struct md2_texCoord_t
{
	short s;
	short t;
};

/* triangle data */
struct md2_triangle_t
{
	unsigned short vertex[3];   /* vertex indices of the triangle */
	unsigned short st[3];       /* tex. coord. indices */
};

/* vertex data */
struct md2_vertex_t
{
	unsigned char v[3];         /* position */
	unsigned char normalIndex;  /* normal vector index */
};

/* frame data */
struct md2_frame_t
{
	vec3_t          scale;      /* scale factor */
	vec3_t          translate;  /* translation vector */
	char            name[16];   /* frame name */
	md2_vertex_t    verts[1];     /* list of frame's vertices */
};

/* md2 header */
struct MD2Header
{
	int ident;          /* magic number: "IDP2" */
	int version;        /* version: must be 8 */

	int skinwidth;      /* texture width */
	int skinheight;     /* texture height */

	int framesize;      /* size in bytes of a frame */

	int num_skins;      /* number of skins */
	int num_vertices;   /* number of vertices per frame */
	int num_st;         /* number of texture coordinates */
	int num_tris;       /* number of triangles */
	int num_glcmds;     /* number of opengl commands */
	int num_frames;     /* number of frames */

	md2_skin_t		*skins;		/* offset skin data */
	md2_texCoord_t	*st;		/* offset texture coordinate data */
	md2_triangle_t	*tris;		/* offset triangle data */
	md2_frame_t		*frames;	/* offset frame data */
	unsigned int offset_glcmds;	/* offset OpenGL command data */
	unsigned int offset_end;	/* offset end of file */
};

const int numNormals = 162;

vec3_t normalTable[numNormals] =
{
	#include "MD2Normals.h"
};

void ParseMD2File(char *pFile, uint32 length)
{
	int a, b;

	MD2Header *pHeader = (MD2Header*)pFile;

	MFDebug_Assert(pHeader->ident == (('2'<<24) | ('P'<<16) | ('D'<<8) | 'I'), "Invalid MD2 header.");
	MFDebug_Assert(pHeader->version == 8, "Invalid MD2 version.");

	(char*&)pHeader->skins += (uint32&)pHeader;
	(char*&)pHeader->st += (uint32&)pHeader;
	(char*&)pHeader->tris += (uint32&)pHeader;
	(char*&)pHeader->frames += (uint32&)pHeader;

	MFMatrix md2Mat;
	md2Mat.SetIdentity();
	md2Mat.SetXAxis3(MakeVector(1,0,0));
	md2Mat.SetYAxis3(MakeVector(0,0,1));
	md2Mat.SetZAxis3(MakeVector(0,1,0));

	// material
	F3DMaterialChunk *pMatChunk = pModel->GetMaterialChunk();

	MFString_Copy(pMatChunk->materials[0].name, pModel->name);
	MFString_Copy(pMatChunk->materials[0].maps[0], pModel->name);

	// mesh data
	F3DMeshChunk *pMC = pModel->GetMeshChunk();
	F3DSubObject &sub = pMC->subObjects[0];

	// copy name (TODO: remove file extension)
	MFString_Copy(sub.name, pModel->name);

	// colours
	sub.colours.resize(1);
	sub.colours[0] = MFVector::one;

	// tex coords
	sub.uvs.resize(pHeader->num_st);

	for(a=0; a<pHeader->num_st; a++)
	{
		sub.uvs[a].x = (float)pHeader->st[a].s / (float)pHeader->skinwidth;
		sub.uvs[a].y = (float)pHeader->st[a].t / (float)pHeader->skinheight;
		sub.uvs[a].z = 0.0f;
	}

	// normals
	sub.normals.resize(numNormals);

	for(a=0; a<numNormals; a++)
	{
		sub.normals[a].x = normalTable[a][0];
		sub.normals[a].y = normalTable[a][1];
		sub.normals[a].z = normalTable[a][2];

		sub.normals[a] = ApplyMatrix(sub.normals[a], md2Mat);
	}

//	for(a=0; a<pHeader->num_frames; a++)
//	{
		// read frame data

//		(char*&)pHeader->frames += pHeader->framesize;
//	}

	// vertices
	sub.positions.resize(pHeader->num_vertices);

	MFVector scale = MakeVector(pHeader->frames->scale[0], pHeader->frames->scale[1], pHeader->frames->scale[2], 0.0f);
	MFVector translate = MakeVector(pHeader->frames->translate[0], pHeader->frames->translate[1], pHeader->frames->translate[2]);

	for(b=0; b<pHeader->num_vertices; b++)
	{
		// verts
		sub.positions[b] = MakeVector((float)pHeader->frames->verts[b].v[0], (float)pHeader->frames->verts[b].v[1], (float)pHeader->frames->verts[b].v[2]);
		sub.positions[b] = sub.positions[b]*scale + translate;

		sub.positions[b] = ApplyMatrix(sub.positions[b], md2Mat);
	}

	// trnangles
	F3DMaterialSubobject &matSub = sub.matSubobjects[0];
	matSub.materialIndex = 0;

	matSub.vertices.resize(pHeader->num_tris * 3);

	for(a=0; a<pHeader->num_tris; a++)
	{
		matSub.vertices[a*3 + 0].colour = 0;
		matSub.vertices[a*3 + 1].colour = 0;
		matSub.vertices[a*3 + 2].colour = 0;

		matSub.vertices[a*3 + 0].position = pHeader->tris[a].vertex[0];
		matSub.vertices[a*3 + 0].uv1 = pHeader->tris[a].st[0];
		matSub.vertices[a*3 + 0].normal = pHeader->frames->verts[pHeader->tris[a].vertex[0]].normalIndex;

		matSub.vertices[a*3 + 1].position = pHeader->tris[a].vertex[1];
		matSub.vertices[a*3 + 1].uv1 = pHeader->tris[a].st[1];
		matSub.vertices[a*3 + 1].normal = pHeader->frames->verts[pHeader->tris[a].vertex[1]].normalIndex;

		matSub.vertices[a*3 + 2].position = pHeader->tris[a].vertex[2];
		matSub.vertices[a*3 + 2].uv1 = pHeader->tris[a].st[2];
		matSub.vertices[a*3 + 2].normal = pHeader->frames->verts[pHeader->tris[a].vertex[2]].normalIndex;
	}

	for(a=0; a<pHeader->num_tris; a++)
	{
		matSub.triangles[a].v[0] = a*3 + 0;
		matSub.triangles[a].v[1] = a*3 + 1;
		matSub.triangles[a].v[2] = a*3 + 2;

		// TODO: generate face normal
	}
}

int F3DFile::ReadMD2(char *pFilename)
{
	FILE *infile;
	char *file = NULL;
	uint32 filesize = 0;

	infile = fopen(pFilename, "r");

	if(!infile)
	{
		printf("Failed to open MD2 file %s", pFilename);
		return 1;
	}

	fseek(infile, 0, SEEK_END);
	filesize = ftell(infile);
	fseek(infile, 0, SEEK_SET);

	file = (char*)malloc(filesize+1);
	fread(file, 1, filesize, infile);
	file[filesize] = NULL;

	fclose(infile);

	int a;

	for(a=MFString_Length(pFilename)-1; a>=0; a--)
	{
		if(pFilename[a] == '/' || pFilename[a] == '\\')
		{
			break;
		}
	}

	MFString_Copy(pModel->name, &pFilename[a+1]);
	pModel->name[MFString_Length(pModel->name) - 4] = 0;

	ParseMD2File(file, filesize);

	free(file);

	return 0;
}
