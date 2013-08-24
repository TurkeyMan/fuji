#include "Fuji.h"

#if defined(USE_ASSIMP)

#include "Util/F3D.h"
#include "MFFileSystem.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#pragma comment(lib, "assimp")

void ParseNode(aiNode *pNode, F3DFile &f3d)
{
//	pNode->
}

void ParseAssimpMesh(char *pFile, size_t size, const char *pExt, F3DFile *_pModel)
{
	Assimp::Importer importer;

	unsigned int options = aiProcess_ConvertToLeftHanded		|
							aiProcess_FlipWindingOrder			|
//							aiProcess_CalcTangentSpace			| 
							aiProcess_Triangulate				|
							aiProcess_GenUVCoords				|
							aiProcess_JoinIdenticalVertices		|
							aiProcess_FindDegenerates			|
							aiProcess_SortByPType				|
							aiProcess_RemoveRedundantMaterials	|
//							aiProcess_ImproveCacheLocality		| // this needs to be configured for each target...
							0;

	// if we want lines and points, but don't want degenerate triangles (some formats use degenerate triangles to emulate lines/points):
//	importer.SetPropertyInteger(AI_CONFIG_PP_FD_REMOVE, 1); // remove degenerate triangles
	// of we don't want lines and points at all:
	importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE); // remove all lines and points

	// set the post-transform cache size...
//	importer.SetPropertyInteger(AI_CONFIG_PP_ICL_PTCACHE_SIZE, postTransformCacheSize?);

	const aiScene *pScene = importer.ReadFileFromMemory(pFile, size, options, pExt);
	if(!pScene)
		return; // some sort of error?

	F3DFile &f3d = *_pModel;

	// copy all the materials
	F3DMaterialChunk &matc = *f3d.GetMaterialChunk();
	for(uint32 i=0; i<pScene->mNumMaterials; ++i)
	{
		aiMaterial &aiMat = *pScene->mMaterials[i];
		F3DMaterial &mat = matc.materials.push();

		aiString name;
		aiMat.Get(AI_MATKEY_NAME, name);
		mat.name = name.C_Str();

		aiColor4D color(1.f, 1.f, 1.f, 1.f);
		aiMat.Get(AI_MATKEY_COLOR_DIFFUSE, color);
		mat.diffuse = MakeVector(color.r, color.g, color.b, color.a);

		color = aiColor4D(1.f, 1.f, 1.f, 1.f);
		aiMat.Get(AI_MATKEY_COLOR_AMBIENT, color);
		mat.ambient = MakeVector(color.r, color.g, color.b, color.a);

		color = aiColor4D(1.f, 1.f, 1.f, 1.f);
		aiMat.Get(AI_MATKEY_COLOR_EMISSIVE, color);
		mat.emissive = MakeVector(color.r, color.g, color.b, color.a);

		color = aiColor4D(1.f, 1.f, 1.f, 1.f);
		aiMat.Get(AI_MATKEY_COLOR_SPECULAR, color);
		mat.specular = MakeVector(color.r, color.g, color.b, color.a);

		aiString diffuse;
		aiMat.Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), diffuse);
		mat.maps[0] = diffuse.C_Str();
	}

	// copy all the mesh
	F3DMeshChunk &mc = *f3d.GetMeshChunk();
	for(uint32 i=0; i<pScene->mNumMeshes; ++i)
	{
		aiMesh &mesh = *pScene->mMeshes[i];

		F3DSubObject &sub = mc.subObjects.push();
		sub.name = mesh.mName.C_Str();

		F3DMaterialSubobject &matSub = sub.matSubobjects.push();
		matSub.materialIndex = mesh.mMaterialIndex;

		// copy mesh data
		int firstVert = (int)matSub.vertices.size();
		matSub.vertices.resize(firstVert + mesh.mNumVertices);
		F3DVertex *verts = matSub.vertices.getPointer();

		for(uint32 j=0; j<mesh.mNumVertices; ++j)
		{
			verts[firstVert+j].position = (int)sub.positions.size();
			sub.positions.push(MakeVector(mesh.mVertices[j].x, mesh.mVertices[j].y, mesh.mVertices[j].z));
		}
		if(mesh.HasNormals())
		{
			for(uint32 j=0; j<mesh.mNumVertices; ++j)
			{
				verts[firstVert+j].normal = (int)sub.normals.size();
				sub.normals.push(MakeVector(mesh.mNormals[j].x, mesh.mNormals[j].y, mesh.mNormals[j].z));
			}
		}
		if(mesh.HasTangentsAndBitangents())
		{
			for(uint32 j=0; j<mesh.mNumVertices; ++j)
			{
				verts[firstVert+j].biNormal = (int)sub.biNormals.size();
				verts[firstVert+j].tangent = (int)sub.tangents.size();
				sub.biNormals.push(MakeVector(mesh.mBitangents[j].x, mesh.mBitangents[j].y, mesh.mBitangents[j].z));
				sub.tangents.push(MakeVector(mesh.mTangents[j].x, mesh.mTangents[j].y, mesh.mTangents[j].z));
			}
		}
		for(uint32 t=0; t<mesh.GetNumUVChannels(); ++t)
		{
			for(uint32 j=0; j<mesh.mNumVertices; ++j)
			{
				verts[firstVert+j].uv[t] = (int)sub.uvs.size();
				sub.uvs.push(MakeVector(mesh.mTextureCoords[t][j].x, mesh.mTextureCoords[t][j].y, mesh.mTextureCoords[t][j].z));
			}
		}
		if(mesh.HasVertexColors(0))
		{
			for(uint32 j=0; j<mesh.mNumVertices; ++j)
			{
				verts[firstVert+j].colour = (int)sub.colours.size();
				sub.colours.push(MakeVector(mesh.mColors[0][j].r, mesh.mColors[0][j].g, mesh.mColors[0][j].b, mesh.mColors[0][j].a));
			}
		}
		if(mesh.HasVertexColors(1))
		{
			for(uint32 j=0; j<mesh.mNumVertices; ++j)
			{
				verts[firstVert+j].illum = (int)sub.illumination.size();
				sub.illumination.push(MakeVector(mesh.mColors[1][j].r, mesh.mColors[1][j].g, mesh.mColors[1][j].b, mesh.mColors[1][j].a));
			}
		}

		// copy faces
		for(uint32 j=0; j<mesh.mNumFaces; ++j)
		{
			aiFace &f = mesh.mFaces[j];
			MFDebug_Assert(f.mNumIndices == 3, "Prim is not a triangle!");
			F3DTriangle &tri = matSub.triangles.push();
			tri.v[0] = firstVert+f.mIndices[0];
			tri.v[1] = firstVert+f.mIndices[1];
			tri.v[2] = firstVert+f.mIndices[2];
			MFVector &p0 = sub.positions[matSub.vertices[tri.v[0]].position];
			MFVector &p1 = sub.positions[matSub.vertices[tri.v[1]].position];
			MFVector &p2 = sub.positions[matSub.vertices[tri.v[2]].position];
			tri.normal = (p2-p0).Cross3(p1-p0).Normalise3();
		}

		// copy animation data
		matSub.numBones = mesh.mNumBones;
		for(uint32 j=0; j<mesh.mNumBones; ++j)
		{
			aiBone &bone = *mesh.mBones[j];

			bone.mName; // find or add bone by name...
			bone.mOffsetMatrix; // bone's matrix
			int id = j; // get bone id

			for(uint32 k=0; k<bone.mNumWeights; ++k)
			{
				aiVertexWeight &w = bone.mWeights[k];

				F3DVertex &v = verts[firstVert+w.mVertexId];

				const int maxWeights = sizeof(v.bone)/sizeof(v.bone[0]);
				for(int b=0; b<maxWeights; ++b)
				{
					if(v.bone[b] == -1)
					{
						v.bone[b] = id;
						v.weight[b] = w.mWeight;

						matSub.maxWeights = MFMax(matSub.maxWeights, b+1);
						break;
					}
				}
			}
		}
	}

	// parse the scene
	ParseNode(pScene->mRootNode, *_pModel);

	// special tweaks for particular formats...
	if(!MFString_CaseCmp(pExt, ".md2"))
	{
		matc.materials[0].name = f3d.name;
		matc.materials[0].maps[0] = f3d.name;
	}
}

#endif
