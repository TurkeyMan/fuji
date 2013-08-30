#include "Fuji.h"

#if defined(USE_ASSIMP)

#include "Util/F3D.h"
#include "MFFileSystem.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

//#pragma comment(lib, "assimp")

void CopyMatrix(MFMatrix *a, const aiMatrix4x4 &b)
{
	a->m[0]  = b.a1;
	a->m[1]  = b.b1;
	a->m[2]  = b.c1;
	a->m[3]  = b.d1;
	a->m[4]  = b.a2;
	a->m[5]  = b.b2;
	a->m[6]  = b.c2;
	a->m[7]  = b.d2;
	a->m[8]  = b.a3;
	a->m[9]  = b.b3;
	a->m[10] = b.c3;
	a->m[11] = b.d3;
	a->m[12] = b.a4;
	a->m[13] = b.b4;
	a->m[14] = b.c4;
	a->m[15] = b.d4;
}

void ParseMesh(const aiScene *pScene, aiNode &node, F3DFile &f3d, const aiMatrix4x4 &parent, int depth = 0)
{
	aiMatrix4x4 world = parent * node.mTransformation;

	if(node.mNumMeshes)
	{
		F3DMeshChunk &mc = *f3d.GetMeshChunk();

		F3DSubObject &sub = mc.subObjects.push();
		sub.name = node.mName.C_Str();

		aiMatrix3x3 worldRotation = aiMatrix3x3(world);

		// parse node mesh
		for(uint32 i=0; i<node.mNumMeshes; ++i)
		{
			aiMesh &mesh = *pScene->mMeshes[node.mMeshes[i]];

			MFDebug_Log(0, MFStr("%*sMesh %d: %d (%s)", depth, " ", i, node.mMeshes[i], mesh.mName.C_Str()));

			F3DMaterialSubobject &matSub = sub.matSubobjects.push();
			matSub.materialIndex = mesh.mMaterialIndex;

			F3DMaterialChunk &matc = *f3d.GetMaterialChunk();
			MFDebug_Log(0, MFStr("%*s  Material: %d (%s)", depth, " ", mesh.mMaterialIndex, matc.materials[mesh.mMaterialIndex].name.CStr()));

			// copy mesh data
			matSub.vertices.resize(mesh.mNumVertices);
			F3DVertex *verts = matSub.vertices.getPointer();

			for(uint32 j=0; j<mesh.mNumVertices; ++j)
			{
				verts[j].position = (int)sub.positions.size();

				aiVector3D pos = mesh.mVertices[j];
				pos *= world;
				sub.positions.push(MakeVector(pos.x, pos.y, pos.z));
			}
			if(mesh.HasNormals())
			{
				for(uint32 j=0; j<mesh.mNumVertices; ++j)
				{
					verts[j].normal = (int)sub.normals.size();

					aiVector3D normal = mesh.mNormals[j];
					normal *= worldRotation;
					sub.normals.push(MakeVector(normal.x, normal.y, normal.z));
				}
			}
			if(mesh.HasTangentsAndBitangents())
			{
				for(uint32 j=0; j<mesh.mNumVertices; ++j)
				{
					verts[j].biNormal = (int)sub.biNormals.size();
					verts[j].tangent = (int)sub.tangents.size();

					aiVector3D biNormal = mesh.mBitangents[j];
					aiVector3D tangent = mesh.mTangents[j];
					biNormal *= worldRotation;
					tangent *= worldRotation;
					sub.biNormals.push(MakeVector(biNormal.x, biNormal.y, biNormal.z));
					sub.tangents.push(MakeVector(tangent.x, tangent.y, tangent.z));
				}
			}
			for(uint32 t=0; t<mesh.GetNumUVChannels(); ++t)
			{
				for(uint32 j=0; j<mesh.mNumVertices; ++j)
				{
					verts[j].uv[t] = (int)sub.uvs.size();
					sub.uvs.push(MakeVector(mesh.mTextureCoords[t][j].x, mesh.mTextureCoords[t][j].y, mesh.mTextureCoords[t][j].z));
				}
			}
			if(mesh.HasVertexColors(0))
			{
				for(uint32 j=0; j<mesh.mNumVertices; ++j)
				{
					verts[j].colour = (int)sub.colours.size();
					sub.colours.push(MakeVector(mesh.mColors[0][j].r, mesh.mColors[0][j].g, mesh.mColors[0][j].b, mesh.mColors[0][j].a));
				}
			}
			if(mesh.HasVertexColors(1))
			{
				for(uint32 j=0; j<mesh.mNumVertices; ++j)
				{
					verts[j].illum = (int)sub.illumination.size();
					sub.illumination.push(MakeVector(mesh.mColors[1][j].r, mesh.mColors[1][j].g, mesh.mColors[1][j].b, mesh.mColors[1][j].a));
				}
			}

			// copy faces
			for(uint32 j=0; j<mesh.mNumFaces; ++j)
			{
				aiFace &f = mesh.mFaces[j];
				MFDebug_Assert(f.mNumIndices == 3, "Prim is not a triangle!");
				F3DTriangle &tri = matSub.triangles.push();
				tri.v[0] = f.mIndices[0];
				tri.v[1] = f.mIndices[1];
				tri.v[2] = f.mIndices[2];
				MFVector &p0 = sub.positions[matSub.vertices[tri.v[0]].position];
				MFVector &p1 = sub.positions[matSub.vertices[tri.v[1]].position];
				MFVector &p2 = sub.positions[matSub.vertices[tri.v[2]].position];
				tri.normal = (p2-p0).Cross3(p1-p0).Normalise3();
			}

			F3DSkeletonChunk &sc = *f3d.GetSkeletonChunk();

			// copy animation data
			matSub.numBones = mesh.mNumBones;
			for(uint32 j=0; j<mesh.mNumBones; ++j)
			{
				aiBone &bone = *mesh.mBones[j];

				int boneIndex = sc.FindBone(bone.mName.C_Str());
				sc.bones[boneIndex].bIsSkinned = true;

				MFDebug_Log(0, MFStr("%*s  Bone %d: %d (%s)", depth, " ", j, boneIndex, bone.mName.C_Str()));

//				bone.mOffsetMatrix; // bone's matrix

				for(uint32 k=0; k<bone.mNumWeights; ++k)
				{
					aiVertexWeight &w = bone.mWeights[k];

					F3DVertex &v = verts[w.mVertexId];

					const int maxWeights = sizeof(v.bone)/sizeof(v.bone[0]);
					for(int b=0; b<maxWeights; ++b)
					{
						if(v.bone[b] == -1)
						{
							v.bone[b] = boneIndex;
							v.weight[b] = w.mWeight;

							matSub.maxWeights = MFMax(matSub.maxWeights, b+1);
							break;
						}
					}
				}
			}
		}
	}

	for(uint32 i=0; i<node.mNumChildren; ++i)
		ParseMesh(pScene, *node.mChildren[i], f3d, world, depth + 2);
}

void ParseNode(const aiScene *pScene, aiNode &node, F3DFile &f3d, const aiMatrix4x4 &parent, int depth = 0)
{
	MFDebug_Log(0, MFStr("%*sNode: %s", depth, " ", node.mName.C_Str()));

	aiMatrix4x4 &local = node.mTransformation;
	aiMatrix4x4 world = parent * local;

	MFDebug_Log(0, MFStr("%*s  Local Position: %.2f,%.2f,%.2f", depth, " ", local.a4, local.b4, local.c4));
	MFDebug_Log(0, MFStr("%*s  Local Orientation: [%.1f,%.1f,%.1f],[%.1f,%.1f,%.1f],[%.1f,%.1f,%.1f]", depth, " ", local.a1, local.b1, local.c1, local.a2, local.b2, local.c2, local.a3, local.b3, local.c3));
	MFDebug_Log(0, MFStr("%*s  World Position: %.2f,%.2f,%.2f", depth, " ", world.a4, world.b4, world.c4));
	MFDebug_Log(0, MFStr("%*s  World Orientation: [%.1f,%.1f,%.1f],[%.1f,%.1f,%.1f],[%.1f,%.1f,%.1f]", depth, " ", world.a1, world.b1, world.c1, world.a2, world.b2, world.c2, world.a3, world.b3, world.c3));

	// create bone from node
	F3DBone &bone = f3d.GetSkeletonChunk()->bones.push();
	bone.name = node.mName.C_Str();
	bone.parentName = node.mParent ? node.mParent->mName.C_Str() : NULL;
	CopyMatrix(&bone.boneMatrix, local);
	CopyMatrix(&bone.worldMatrix, world);

	for(uint32 i=0; i<node.mNumChildren; ++i)
		ParseNode(pScene, *node.mChildren[i], f3d, world, depth + 2);
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

		MFDebug_Log(0, MFStr("Material %d: %s", i, name.C_Str()));

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

		MFDebug_Log(0, MFStr("  Texture: %s", diffuse.C_Str()));
	}

	// parse the scene
	if(pScene->mRootNode)
	{
		aiMatrix4x4 world;
		ParseNode(pScene, *pScene->mRootNode, *_pModel, world);
		ParseMesh(pScene, *pScene->mRootNode, *_pModel, world);
	}

	// read animation
	F3DSkeletonChunk &skeletonc = *f3d.GetSkeletonChunk();
	F3DAnimationChunk &animc = *f3d.GetAnimationChunk();
	for(uint32 i=0; i<pScene->mNumAnimations; ++i)
	{
		aiAnimation &animation = *pScene->mAnimations[i];

		MFDebug_Log(0, MFStr("Animation %d: %s", i, animation.mName.C_Str()));

		for(uint32 j=0; j<animation.mNumChannels; ++j)
		{
			aiNodeAnim &channel = *animation.mChannels[j];

			int id = skeletonc.FindBone(channel.mNodeName.C_Str());
			MFDebug_Log(0, MFStr("  Node %d: %s", id, channel.mNodeName.C_Str()));

			if(id < 0)
				continue;

			F3DAnimation &anim = animc.anims.push();
			anim.boneID = id;
			anim.minTime = 0.f;
			anim.maxTime = (float)animation.mDuration;

			for(uint32 p=0,r=0,s=0; p<channel.mNumPositionKeys || r<channel.mNumRotationKeys || s<channel.mNumScalingKeys; )
			{
				F3DKeyFrame &frame = anim.keyframes.push();

				aiVectorKey &pk = channel.mPositionKeys[p];
				aiQuatKey &rk = channel.mRotationKeys[r];
				aiVectorKey &sk = channel.mScalingKeys[s];

				bool bKeyPos = p < channel.mNumPositionKeys && (r >= channel.mNumRotationKeys || pk.mTime <= rk.mTime) && (s >= channel.mNumScalingKeys || pk.mTime <= sk.mTime);
				bool bKeyRot = r < channel.mNumRotationKeys && (p >= channel.mNumPositionKeys || rk.mTime <= pk.mTime) && (s >= channel.mNumScalingKeys || rk.mTime <= sk.mTime);
				bool bKeyScale = s < channel.mNumScalingKeys && (p >= channel.mNumPositionKeys || sk.mTime <= pk.mTime) && (r >= channel.mNumRotationKeys || sk.mTime <= rk.mTime);

				aiMatrix4x4 mat;
				if(bKeyRot)
				{
					mat = aiMatrix4x4(rk.mValue.GetMatrix());
					frame.time = (float)rk.mTime;
					++r;
				}
				if(bKeyScale)
				{
					mat.a1 *= sk.mValue.x;
					mat.b1 *= sk.mValue.x;
					mat.c1 *= sk.mValue.x;
					mat.a2 *= sk.mValue.y;
					mat.b2 *= sk.mValue.y;
					mat.c2 *= sk.mValue.y;
					mat.a3 *= sk.mValue.z;
					mat.b3 *= sk.mValue.z;
					mat.c3 *= sk.mValue.z;
					frame.time = (float)sk.mTime;
					++s;
				}
				if(bKeyPos)
				{
					mat.a4 = pk.mValue.x;
					mat.b4 = pk.mValue.y;
					mat.c4 = pk.mValue.z;
					frame.time = (float)pk.mTime;
					++p;
				}

				CopyMatrix(&frame.key, mat);
			}
		}
	}

	// special tweaks for particular formats...
	if(!MFString_CaseCmp(pExt, ".md2"))
	{
		matc.materials[0].name = f3d.name;
		matc.materials[0].maps[0] = f3d.name;
	}
}

#endif
