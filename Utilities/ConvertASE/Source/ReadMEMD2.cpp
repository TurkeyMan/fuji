#include "Fuji.h"
#include "F3D.h"
#include "ReadMEMD2.h"

void ParseMEMD2File(char *pBuffer);

extern F3DFile *pModel;

static MFVector gNormTable[256][256];

void CalcNormTable()
{
	float pitch;
	float yaw;

	for(int i=0; i<256; ++i)
	{
		pitch = (i/255.0f)*MFPI;

		for(int j = 0; j<256; ++j)
		{
			yaw = (j/255.0f)*(MFPI*2);

			gNormTable[i][j].x = (float)(MFSin(yaw) * MFSin(pitch));
			gNormTable[i][j].y = (float)MFCos(pitch);
			gNormTable[i][j].z = (float)(MFCos(yaw) * MFSin(pitch));
		}
	}
}

int F3DFile::ReadMEMD2(char *pFilename)
{
	FILE *pFile;
	char *pBuffer = NULL;
	uint32 filesize = 0;

	pFile = fopen(pFilename, "rb");

	if(!pFile)
	{
		printf("Failed to open ASE file %s.\n", pFilename);
		return 1;
	}

	fseek(pFile, 0, SEEK_END);
	filesize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	pBuffer = (char*)malloc(filesize+1);
	fread(pBuffer, 1, filesize, pFile);
	pBuffer[filesize] = NULL;

	fclose(pFile);

	ParseMEMD2File(pBuffer);

	free(pBuffer);

	return 0;
}

void UnpackVertex(MEMD2_VERTEX *in, MEMesh2 *msh, float scale, F3DSubObject &sub)
{
	F3DVertex &v = sub.matSubobjects[0].vertices.push();
	v.position = sub.positions.size();
	v.normal = sub.normals.size();
	v.uv1 = sub.uvs.size();

	MFVector &pos = sub.positions.push();
	MFVector &normal = sub.normals.push();
	MFVector &uv = sub.uvs.push();

	pos.x = ((float)in->v[0]/msh->Scale.x)+msh->Base.x;
	pos.y = ((float)in->v[1]/msh->Scale.y)+msh->Base.y;
	pos.z = ((float)in->v[2]/msh->Scale.z)+msh->Base.z;
	pos.w = 0.0f;

	pos *= scale;

	uv.x = ((float)in->uv[0]/msh->UVScale.x)+msh->UVBase.x;
	uv.y = ((float)in->uv[1]/msh->UVScale.y)+msh->UVBase.y;
	uv.z = 0.0f;
	uv.w = 0.0f;

	normal = gNormTable[in->n[1]][in->n[0]];
}

int ReadMesh(char** buf, int frmcount, float scale, uint32 flags)
{
	if(!*buf) return 1;

	MEMesh2 *mesh=(MEMesh2*)*buf;
	MEMD2_VERTEX *vb;

	if(!scale) scale=1.0f;

	*buf+=sizeof(MEMesh2);

	if(!mesh->vcount || !mesh->icount) return 1;

	mesh->Scale.x=65525.0f/mesh->Scale.x;
	mesh->Scale.y=65525.0f/mesh->Scale.y;
	mesh->Scale.z=65525.0f/mesh->Scale.z;

	mesh->UVScale.x=65525.0f/mesh->UVScale.x;
	mesh->UVScale.y=65525.0f/mesh->UVScale.y;

	F3DSubObject &sub = pModel->GetMeshChunk()->subObjects.push();
	strcpy(sub.name, mesh->Name);
	sub.matSubobjects[0].materialIndex = mesh->MaterialID;

	// read vertex data
	for(int a=0; a<frmcount; a++)
	{
		vb=(MEMD2_VERTEX*)*buf;

		if(a == 0) // only first frame for now...
		{
			for(int b=0; b<mesh->vcount; b++)
			{
				UnpackVertex(&vb[b], mesh, scale, sub);
			}
		}

		*buf+=sizeof(MEMD2_VERTEX)*mesh->vcount;
	}

	// read triangles
	MFDebug_Assert(mesh->icount % 3 == 0, "Incorrect number of indices...");

	F3DMaterialSubobject &msub = sub.matSubobjects[0];

	uint16 *pI = (uint16*)*buf;
	int tris =0;
	int v = 0;

	for(int a=0; a<mesh->icount; a++)
	{
		msub.triangles[tris].v[v] = pI[a];

		++v;
		if(v>=3)
		{
			v = 0;
			++tris;
		}
	}

	*buf+=sizeof(unsigned short)*mesh->icount;

	return 0;
}

void ParseMEMD2File(char *pBuffer)
{
	char *buf;
	int a;
	MEHeader *head;
	MEMaterial2 *material;

	bool bWriteOutImage = false;

	buf=pBuffer;
	head=(MEHeader*)pBuffer;

	if(head->ID!=MEMD_ID)
	{
		return;
	}

	if(!(head->Version>=200 && head->Version<300))
	{
		return;
	}

	CalcNormTable();

	strcpy(pModel->name, head->Name);
	strcpy(pModel->author, head->Creator);

	if(head->SequenceCount)
	{
		MESequence2 *pSequences = (MESequence2*)(pBuffer+head->SequenceStart);

		// do some shit for each one
	}

	if(head->MaterialCount)
	{
		buf=pBuffer+head->MaterialStart;
		material=(MEMaterial2*)buf;

		for(a=0; a<head->MaterialCount; a++)
		{
			F3DMaterial &mat = pModel->GetMaterialChunk()->materials.push();

			// material name
//			strcpy(mat.name, material->Name);

			// use texture name instead of material name
			char *pTex = material->TextureFileName;
			pTex += strlen(pTex);

			while(pTex > material->TextureFileName && pTex[-1] != '/' && pTex[-1] != '\\')
			{
				--pTex;
			}

			pTex[strlen(pTex) - 4] = NULL;

			strcpy(mat.name, pTex);

			// material paramaters
			mat.specularLevel = material->matPower;
			mat.ambient.x = material->matColours[0].r;
			mat.ambient.y = material->matColours[0].g;
			mat.ambient.z = material->matColours[0].b;
			mat.ambient.w = material->matColours[0].a;

 			buf += 64 + sizeof(v4[4]) + sizeof(float) + sizeof(int);

			if(material->TextureCount)
			{
				buf+=260;

				if(material->ImageLen)
				{
					if(bWriteOutImage)
					{
						FILE *pFile = fopen(material->TextureFileName, "wb");
						if(pFile)
						{
							fwrite(material->ImageBuffer, 1, material->ImageLen, pFile);
							fclose(pFile);
						}
					}

					buf+=material->ImageLen;
				}
			}

			buf+=8;

			material=(MEMaterial2*)buf;
		}

	}

	if(head->FrameCount&&head->MeshCount)
	{
		buf=pBuffer+head->FrameStart;

		for(a=0; a<head->MeshCount; a++)
		{
			ReadMesh(&buf, head->FrameCount, 1.0f, 0);
		}
	}

	return;
}
