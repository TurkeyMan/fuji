#include "Fuji.h"

#if defined(USE_ASSIMP)

#include "Util/F3D.h"
#include "MFFileSystem.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#pragma comment(lib, "assimp")

void ParseNode(aiNode *pNode)
{

}

void ParseAssimpMesh(char *pFile, size_t size, const char *pExt, F3DFile *_pModel)
{
	Assimp::Importer importer;

	unsigned int options = aiProcess_ConvertToLeftHanded	|
							aiProcess_CalcTangentSpace		| 
							aiProcess_Triangulate			|
							aiProcess_GenUVCoords			|
							aiProcess_JoinIdenticalVertices	|
							aiProcess_FindDegenerates		|
							aiProcess_SortByPType			|
							aiProcess_RemoveRedundantMaterials	|
							aiProcess_ImproveCacheLocality	| // this needs to be configured for each target...
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

	// parse the scene
	ParseNode(pScene->mRootNode);
}

#endif
