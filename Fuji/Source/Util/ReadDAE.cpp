#if defined(_MSC_VER)
	#pragma warning( disable : 4530 )
#endif

#include "Fuji.h"
#include "Util/F3D.h"
#include "MFMatrix.h"
#include "MFDocumentXML.h"

bool DAEIsWhite(int c)
{
	return c==' ' || c=='\n' || c=='\r' || c=='\t' || c==0x3000;
}

inline char* DAESkipWhite(char *pC)
{
	while(DAEIsWhite(*pC)) pC++;
	return pC;
}

inline const char* DAESkipWhite(const char *pC)
{
	while(DAEIsWhite(*pC)) pC++;
	return pC;
}

static F3DFile *pModel;

MFXMLNode* pRoot;

MFMatrix transformMatrix;
MFMatrix invTransformMatrix;
bool flipWinding = false;

bool TestID(MFXMLNode *pLib, const char *pLibName)
{
	const char *pName = pLib->Attribute("id");
	if(pName && !MFString_CaseCmp(pName, pLibName))
		return true;
	return false;
}

MFXMLNode* FindObjectInLibrary(const char *pLibName, const char *pLibrary, const char *pLibContent)
{
	if(pLibName[0] == '#')
		++pLibName;

	MFXMLNode *pLib = pRoot->FirstChild(pLibrary);

	while(pLib)
	{
		MFXMLNode *pObject = pLib->FirstChild(pLibContent);

		while(pObject)
		{
			if(TestID(pObject, pLibName))
				return pObject;

			pObject = pObject->NextSibling(pLibContent);
		}

		pLib = pLib->NextSibling(pLibrary);
	}

	return NULL;
}

MFXMLNode* GetSceneLib(const char *pLibName)
{
	return FindObjectInLibrary(pLibName, "library_visual_scenes", "visual_scene");
}

MFXMLNode* GetGeometryLib(const char *pLibName)
{
	return FindObjectInLibrary(pLibName, "library_geometries", "geometry");
}

MFXMLNode* GetMaterialLib(const char *pLibName)
{
	return FindObjectInLibrary(pLibName, "library_materials", "material");
}

void ParseDAEAsset(MFXMLNode *pAsset)
{
	MFXMLNode *pContributor = pAsset->FirstChild("contributor");

	MFXMLNode *pContrib = pContributor ? pContributor : pAsset;

	MFXMLNode *pAuthor = pContrib->FirstChild("author");
	MFXMLNode *pAuthoringTool = pContrib->FirstChild("authoring_tool");
//	MFXMLNode *pSourceData = pContrib->FirstChild("source_data");

	MFXMLNode *pUp = pAsset->FirstChild("up_axis");
	MFXMLNode *pUnit = pAsset->FirstChild("unit");
	MFXMLNode *pCopyright = pAsset->FirstChild("copyright");
//	MFXMLNode *pCreated = pAsset->FirstChild("created");
//	MFXMLNode *pLastModified = pAsset->FirstChild("modified");
//	MFXMLNode *pRevision = pAsset->FirstChild("revision");

	if(pUp)
	{
		// the axiis need to be swapped about into ingame axiis
		const char *pAxis = pUp->Value();

		MFVector x, y, z;
		x = transformMatrix.GetXAxis();
		y = transformMatrix.GetYAxis();
		z = transformMatrix.GetZAxis();

		if(!MFString_CaseCmp(pAxis, "Y_UP"))
		{
			transformMatrix.SetXAxis4(x);
			transformMatrix.SetYAxis4(y);
			transformMatrix.SetZAxis4(-z);

			flipWinding = true;
		}
		else if(!MFString_CaseCmp(pAxis, "Z_UP"))
		{
			transformMatrix.SetXAxis4(x);
			transformMatrix.SetYAxis4(z);
			transformMatrix.SetZAxis4(y);

			flipWinding = true;
		}
	}

	if(pUnit)
	{
		// we need to scale the model into metres..
		const char *pMeter = pUnit->Attribute("meter");
		float scale = (float)atof(pMeter);
		transformMatrix.Scale(MakeVector(scale));
	}

	if(pAuthor)
	{
		// get the author (for kicks)
		const char *pAuth = pAuthor->Value();
		pModel->author = pAuth;
	}

	if(pCopyright)
	{
		// get the author (for kicks)
		const char *pCopyrightString = pCopyright->Value();
		pModel->copyrightString = pCopyrightString;
	}

	if(pAuthoringTool)
	{
		// get the authoring tool (for kicks)
		const char *pAuthTool = pAuthoringTool->Value();
		pModel->authoringTool = pAuthTool;
	}

	invTransformMatrix.Inverse(transformMatrix);
}

int ParseDAEMaterial(MFXMLNode *pMaterialNode)
{
	const char *pName = pMaterialNode->Attribute("name");

	if(!pName)
		pName = "Untitled collada material";

	F3DMaterialChunk *pMatChunk = pModel->GetMaterialChunk();

	int matIndex = (int)pMatChunk->materials.size();

	F3DMaterial &mat = pMatChunk->materials.push();
	mat.name = pName;

	// TODO: parse additional data from material?

	return matIndex;
}

enum ComponentType
{
	CT_Unknown = -1,

	CT_Position = 0,
	CT_Normal,
	CT_UV1,
	CT_Colour,
	CT_Weights,
	CT_Indices,
	CT_Binormal,
	CT_Tangent
};

ComponentType GetComponentType(const char *pType)
{
	if(!MFString_CaseCmp(pType, "POSITION"))
	{
		return CT_Position;
	}
	else if(!MFString_CaseCmp(pType, "NORMAL"))
	{
		return CT_Normal;
	}
	else if(!MFString_CaseCmp(pType, "TEXCOORD"))
	{
		return CT_UV1;
	}
	else if(!MFString_CaseCmp(pType, "COLOR"))
	{
		return CT_Colour;
	}

	MFDebug_Assert(false, MFStr("Unknown semantic: '%s'", pType));
	return CT_Unknown;
}

struct DataSource
{
	DataSource() {}
	DataSource(const DataSource &from)
		: first(from.first), second(from.second) {}
	DataSource(const ComponentType &first, const MFString &second)
		: first(first), second(second) {}

	ComponentType first;
	MFString second;
};

class SourceData
{
public:
	SourceData() {}
	~SourceData() {}
	MFString id;
	MFArray<MFArray<float> > data;
	int validComponents;
};

MFArray<SourceData> sourceData;

SourceData* GetSourceData(const char *pSourceData)
{
	if(*pSourceData == '#')
		++pSourceData;

	for(int a=0; a<(int)sourceData.size(); a++)
	{
		if(!MFString_CaseCmp(pSourceData, sourceData[a].id.CStr()))
			return &sourceData[a];
	}

	return NULL;
}

void ReadSourceData(MFXMLNode *pSource, SourceData &data)
{
	MFXMLNode *pTechnique = pSource->FirstChild("technique_common");

	if(pTechnique)
	{
		MFXMLNode *pAccessor = pTechnique->FirstChild("accessor");

		int count = pAccessor->IntAttribute("count");
		int stride =pAccessor->IntAttribute("stride");

		data.validComponents = stride;
		data.data.resize(count);
	}

	MFXMLNode *pArray;
	pArray = pSource->FirstChild("array");
	MFDebug_Assert(!pArray, "'array' not supported for vertex data.\n");
	pArray = pSource->FirstChild("int_array");
	MFDebug_Assert(!pArray, "'int_array' not supported for vertex data.\n");
	pArray = pSource->FirstChild("Name_array");
	MFDebug_Assert(!pArray, "'Name_array' not supported for vertex data.\n");

	pArray = pSource->FirstChild("float_array");
	MFDebug_Assert(pArray, "source has no array data.\n");

	int dataCount = pArray->IntAttribute("count");
	MFDebug_Assert((int)data.data.size() * data.validComponents == dataCount, "Not enough data in array for all data specified by the technique.\n");

	const char *pText = pArray->Value();
	pText = DAESkipWhite(pText);

	int i=0;
	int j=0;

	while(*pText)
	{
		data.data[j].push((float)atof(pText));

		while(*pText && !DAEIsWhite(*pText))
			++pText;
		pText = DAESkipWhite(pText);

		++i;
		if(i >= data.validComponents)
		{
			i=0;
			++j;
		}
	}

	MFDebug_Assert(j == (int)data.data.size(), "Inforect amount of data in array.\n");
}

MFArray<MFVector>* GetSemanticArray(F3DSubObject &sub, ComponentType ct)
{
	switch(ct)
	{
		case CT_Position:
			return &sub.positions;
		case CT_Normal:
			return &sub.normals;
		case CT_UV1:
			return &sub.uvs;
		case CT_Colour:
			return &sub.colours;
		case CT_Binormal:
			return &sub.biNormals;
		case CT_Tangent:
			return &sub.tangents;
		case CT_Weights:
		case CT_Indices:
			return NULL;
		default:
			MFUNREACHABLE;
	}

	return NULL;
}

void ParseDAEGeometry(MFXMLNode *pGeometryNode, const MFMatrix &worldTransform)
{
	const char *pName = pGeometryNode->Attribute("name");

	if(!pName)
	{
		pName = pGeometryNode->Attribute("id");
		MFDebug_Warn(2, MFStr("Geometry object '%s' has no name.", pName));
	}

	F3DMeshChunk *pMeshChunk = pModel->GetMeshChunk();
	F3DSubObject &subObject = pMeshChunk->subObjects.push();

	subObject.name = pName;

	MFXMLNode *pMesh = pGeometryNode->FirstChild("mesh");

	while(pMesh)
	{
		MFXMLNode *pMeshElement = pMesh->FirstChild();

		typedef MFArray<DataSource> DataSources;
		MFArray<DataSources> components;
		components.resize(1);
		sourceData.clear();

		while(pMeshElement)
		{
			const char *pValue = pMeshElement->Name();

			if(!MFString_CaseCmp(pValue, "source"))
			{
				const char *pName = pMeshElement->Attribute("id");

				SourceData x;
				sourceData.push(x);
				SourceData &data = sourceData.back();
				data.id = pName;

				ReadSourceData(pMeshElement, data);
			}
			else if(!MFString_CaseCmp(pValue, "vertices"))
			{
				MFXMLNode *pInputs = pMeshElement->FirstChild("input");

				while(pInputs)
				{
					const char *pSemantic = pInputs->Attribute("semantic");
					const char *pSource = pInputs->Attribute("source");

					components[0].push(DataSource(GetComponentType(pSemantic), pSource));

					pInputs = pInputs->NextSibling("input");
				}
			}
			else if(!MFString_CaseCmp(pValue, "polylist") || !MFString_CaseCmp(pValue, "triangles"))
			{
				MFXMLNode *pInputs = pMeshElement->FirstChild("input");

				MFDebug_Assert(pInputs, "No inputs for polygons.\n");

				const char *pSemantic = pInputs->Attribute("semantic");
//				const char *pSource = pInputs->Attribute("source");

				MFDebug_Assert(!MFString_CaseCmp(pSemantic, "VERTEX"), "First input must be VERTEX.\n");

				// get mat sub structure
				F3DMaterialSubobject &matSub = subObject.matSubobjects.push();

				const char *pMaterial = pMeshElement->Attribute("material");

				if(pMaterial)
				{
					// find material in the library
					MFXMLNode *pMat = GetMaterialLib(pMaterial);

					if(pMat)
					{
						// get material name
						const char *pMaterialName = pMat->Attribute("name");

						if(!pMaterialName)
							pMaterialName = pMat->Attribute("id");

						// get the F3D material index
						matSub.materialIndex = pModel->GetMaterialChunk()->GetMaterialIndexByName(pMaterialName);

						// if the material doesnt exist in the F3D yet
						if(matSub.materialIndex == -1)
						{
							// add material to F3D
							matSub.materialIndex = ParseDAEMaterial(pMat);
						}
					}
					else
					{
						// not found :/
						MFDebug_Assert(false, MFStr("Material '%s' not found in library", pMaterial));
						matSub.materialIndex = -1;
					}
				}

				pInputs = pInputs->NextSibling("input");

				while(pInputs)
				{
					// need to skip the first item...
					const char *pSemantic = pInputs->Attribute("semantic");
					const char *pSource = pInputs->Attribute("source");

					DataSources &t = components.push();
					t.push(DataSource(GetComponentType(pSemantic), pSource));

					pInputs = pInputs->NextSibling("input");
				}

				size_t numIndices = components.size();

				// copy the data into the arrays
				for(size_t a=0; a<numIndices; a++)
				{
					DataSources sources = components[a];

					for(size_t b=0; b<sources.size(); b++)
					{
						switch(sources[b].first)
						{
							case CT_UV1:
							{
								SourceData *pData = GetSourceData(sources[b].second.CStr());

								if(pData)
								{
									MFArray<MFVector> &dataArray = *GetSemanticArray(subObject, sources[b].first);

									for(size_t c=0; c<pData->data.size(); c++)
									{
										MFVector &v = dataArray.push();
										MFArray<float> &row = pData->data[c];

										for(int d=0; d<MFMin(pData->validComponents, 4); d++)
										{
											v[d] = d == 1 ? 1.0f - row[d] : row[d];
										}
									}
								}
								break;
							}
							case CT_Colour:
							{
								SourceData *pData = GetSourceData(sources[b].second.CStr());

								if(pData)
								{
									MFArray<MFVector> &dataArray = *GetSemanticArray(subObject, sources[b].first);

									for(size_t c=0; c<pData->data.size(); c++)
									{
										MFVector &v = dataArray.push();
										MFArray<float> &row = pData->data[c];

										for(int d=0; d<MFMin(pData->validComponents, 4); d++)
											v[d] = row[d];
										if(pData->validComponents < 4)
											v[3] = 1.0f;
									}
								}
								break;
							}
							case CT_Position:
							case CT_Normal:
							case CT_Binormal:
							case CT_Tangent:
							{
								SourceData *pData = GetSourceData(sources[b].second.CStr());

								if(pData)
								{
									MFArray<MFVector> &dataArray = *GetSemanticArray(subObject, sources[b].first);

									for(size_t c=0; c<pData->data.size(); c++)
									{
										MFVector &v = dataArray.push();
										MFArray<float> &row = pData->data[c];

										for(int d=0; d<MFMin(pData->validComponents, 4); d++)
											v[d] = row[d];
									}
								}
								break;
							}
							case CT_Weights:
							case CT_Indices:
							default:
								// do nothing
								break;
						}
					}
				}

				// get num polys
				const char *pNumPolys = pMeshElement->Attribute("count");
				MFDebug_Assert(pNumPolys, "<polylist> has no count attribute.");

				int numPolys = atoi(pNumPolys);

				// get vertcounts for each poly
				MFArray<int> vertCounts;

				if(!MFString_CaseCmp(pValue, "polylist"))
				{

					MFXMLNode *pVCount = pMeshElement->FirstChild("vcount");
					MFDebug_Assert(pVCount, "No <vcount> in <polylist>");

					const char *pVCountString = pVCount->Value();
					pVCountString = DAESkipWhite(pVCountString);

					while(*pVCountString)
					{
						int vcount = atoi(pVCountString);
						vertCounts.push(vcount);

						while(*pVCountString && !DAEIsWhite(*pVCountString))
							++pVCountString;

						pVCountString = DAESkipWhite(pVCountString);
					}
				}
				else if(!MFString_CaseCmp(pValue, "triangles"))
				{
					vertCounts.resize(numPolys);

					for(int a=0; a<numPolys; a++)
					{
						vertCounts[a] = 3;
					}
				}
				else
				{
					MFDebug_Assert(false, "Unknown primitive type..");
				}

				// read polygons
				MFXMLNode *pPolygon = pMeshElement->FirstChild("p");
				MFDebug_Assert(pPolygon, "No <p> in <polylist>");

				const char *pText = pPolygon->Value();
				pText = DAESkipWhite(pText);

				// build the vertex and face lists
				int tri = 0;
				int poly = 0;

				int vert = 0;
				int firstVert = (int)matSub.vertices.size();

				while(*pText)
				{
					if(vert > 2)
					{
						++tri;
						matSub.triangles[tri].v[0] = firstVert;
						matSub.triangles[tri].v[1] = (int)matSub.vertices.size()-1;
						matSub.triangles[tri].v[2] = (int)matSub.vertices.size();
					}
					else
					{
						matSub.triangles[tri].v[vert] = (int)matSub.vertices.size();
					}

					F3DVertex &v = matSub.vertices.push();

					for(size_t a=0; a<numIndices; a++)
					{
						int index = atoi(pText);

						DataSources sources = components[a];

						for(size_t b=0; b<sources.size(); b++)
						{
							switch(sources[b].first)
							{
								case CT_Position:
									v.position = index;
									break;
								case CT_Normal:
									v.normal = index;
									break;
								case CT_UV1:
									v.uv[0] = index;
									break;
								case CT_Colour:
									v.colour = index;
									break;
								case CT_Weights:
								{
									SourceData *pData = GetSourceData(sources[b].second.CStr());

									if(pData)
									{
										for(int c=0; c<MFMin(pData->validComponents, 4); c++)
										{
											v.weight[c] = pData->data[index][c];
										}
									}
									break;
								}
								case CT_Indices:
								{
									SourceData *pData = GetSourceData(sources[b].second.CStr());

									if(pData)
									{
										for(int c=0; c<MFMin(pData->validComponents, 4); c++)
										{
											v.bone[c] = (int)pData->data[index][c];
										}
									}
									break;
								}
								case CT_Binormal:
									v.biNormal = index;
									break;
								case CT_Tangent:
									v.tangent = index;
									break;
								default:
									// do nothing
									break;
							}
						}

						while(*pText && !DAEIsWhite(*pText))
							++pText;
						pText = DAESkipWhite(pText);
					}

					++vert;

					if(vert >= vertCounts[poly])
					{
						++poly;
						++tri;
						vert = 0;
						firstVert = (int)matSub.vertices.size();
					}
				}
			}

			pMeshElement = pMeshElement->NextSibling();
		}

		pMesh = pMesh->NextSibling("mesh");
	}

	for(size_t a=0; a<subObject.positions.size(); a++)
	{
		// apply coordinate system correction matrix
		MFVector newPos = ApplyMatrixH(subObject.positions[a], transformMatrix);

		// and transform into world space
		subObject.positions[a] = ApplyMatrixH(newPos, worldTransform);
	}

	// flip the triangle winding if coordinate systems have changed 'handedness'
	if(flipWinding)
	{
		for(size_t a=0; a<subObject.matSubobjects.size(); a++)
		{
			for(size_t b=0; b<subObject.matSubobjects[a].triangles.size(); b++)
			{
				int i = subObject.matSubobjects[a].triangles[b].v[1];
				subObject.matSubobjects[a].triangles[b].v[1] = subObject.matSubobjects[a].triangles[b].v[2];
				subObject.matSubobjects[a].triangles[b].v[2] = i;
			}
		}
	}
}

void ParseDAEAnimation(MFXMLNode *pSceneNode)
{

}

void ParseDAELight(MFXMLNode *pLightNode)
{

}

void ParseDAECamera(MFXMLNode *pCameraNode)
{

}

void FindAndAddGeometryToScene(MFXMLNode *pInstanceNode, MFXMLNode *pParentNode, const char *pObjectName, const MFMatrix &matrix)
{
	// scan library for object
	MFXMLNode *pObject = GetGeometryLib(pObjectName);

	if(pObject)
	{
		ParseDAEGeometry(pObject, matrix);
	}
	else
	{
		MFDebug_Warn(1, MFStr("Object '%s' not found in library...\n", pObjectName));
	}
}

void ParseSceneNode(MFXMLNode *pSceneNode, const MFMatrix &parentMatrix, const char *pParentName)
{
	MFXMLNode *pTransform;
	MFMatrix localMat = MFMatrix::identity;

	// build the transform
	pTransform = pSceneNode->FirstChild();

	while(pTransform)
	{
		const char *pTransformValue = pTransform->Name();

		if(!MFString_CaseCmp(pTransformValue, "matrix"))
		{
			// apply this node's transformation matrix to the current transform
			const char *pMat = pTransform->Value();

			int lookup[16] = { 0,4,8,12, 1,5,9,13, 2,6,10,14, 3,7,11,15 };
			for(int c=0; *pMat && c<16; c++)
			{
				localMat.m[lookup[c]] = (float)atof(pMat);

				while(*pMat && !DAEIsWhite(*pMat))
					pMat++;
				while(DAEIsWhite(*pMat))
					pMat++;
			}
		}
		else if(!MFString_CaseCmp(pTransformValue, "translate"))
		{
			// just translate the current transform
			const char *pTrans = pTransform->Value();
			MFVector translation = MFVector::identity;

			int c = 0;
			while(*pTrans && c<4)
			{
				translation[c] = (float)atof(pTrans);
				++c;

				while(*pTrans && !DAEIsWhite(*pTrans))
					pTrans++;
				while(DAEIsWhite(*pTrans))
					pTrans++;
			}

			localMat.Translate(translation);
		}
		else if(!MFString_CaseCmp(pTransformValue, "scale"))
		{
			// scale the current transform
			const char *pScale = pTransform->Value();
			MFVector scale = MFVector::identity;

			int c = 0;
			while(*pScale && c<4)
			{
				scale[c] = (float)atof(pScale);
				++c;

				while(*pScale && !DAEIsWhite(*pScale))
					pScale++;
				while(DAEIsWhite(*pScale))
					pScale++;
			}

			localMat.Scale(scale);
		}
		else if(!MFString_CaseCmp(pTransformValue, "rotate"))
		{
			// rotate the current transform
			const char *pRot = pTransform->Value();
			MFVector rot = MFVector::zero;

			int c = 0;
			while(*pRot && c<4)
			{
				rot[c] = (float)atof(pRot);
				++c;

				while(*pRot && !DAEIsWhite(*pRot))
					pRot++;
				while(DAEIsWhite(*pRot))
					pRot++;
			}

			localMat.Rotate(rot, MFDEGREES(rot.w));
		}

		pTransform = pTransform->NextSibling();
	}

	localMat.Multiply4x4(invTransformMatrix, localMat).Multiply4x4(transformMatrix);

	// TODO: this may need to be the other way around :)
	MFMatrix worldMat;
	worldMat.Multiply4x4(localMat, parentMatrix);

	// check the node name for any naming conventions (bones/ref points/etc)
	const char *pNodeName = pSceneNode->Attribute("name");

	if(!MFString_CaseCmpN(pNodeName, "r_", 2))
	{
		F3DRefPoint &ref = pModel->GetRefPointChunk()->refPoints.push();

		ref.name = pNodeName;
		ref.worldMatrix = worldMat;
		ref.localMatrix = localMat;
		ref.bone[0] = (uint16)-1;
	}
	else if(!MFString_CaseCmpN(pNodeName, "z_", 2) || !MFString_CaseCmpN(pNodeName, "bn_", 3))
	{
		F3DBone &bone = pModel->GetSkeletonChunk()->bones.push();

		bone.name = pNodeName;
		bone.parentName = pParentName;
		bone.boneMatrix = localMat;
		bone.worldMatrix = worldMat;
	}
	else
	{
		// check for a geometry node
		MFXMLNode *pGeometry = pSceneNode->FirstChild("instance_geometry");
		if(pGeometry)
		{
			// get object name
			const char *pObjectName = pGeometry->Attribute("url");

			if(pObjectName[0] != '#')
			{
				const char *pHash = MFString_Chr(pObjectName, '#');
				const char *pFilename = MFStr_GetFileNameWithoutExtension(pHash ? MFStrN(pObjectName, pHash - pObjectName) : pObjectName);

				// this is instancing an external mesh..
				F3DRefMesh &ref = pModel->GetRefMeshChunk()->refMeshes.push();

				ref.name = pNodeName;
				ref.target = pFilename;
				ref.worldMatrix = worldMat;
				ref.localMatrix = localMat;
			}
			else
			{
				// node is an instance node
				// find and add instance to scene
				FindAndAddGeometryToScene(pGeometry, pSceneNode, pObjectName, worldMat);
			}
		}

		// all the other stuff
//		MFXMLNode *pInstance = pSceneNode->FirstChild("instance_camera");
	}

	// recurse child nodes
	MFXMLNode *pNode = pSceneNode->FirstChild("node");
	while(pNode)
	{
		// and recurse for child nodes
		ParseSceneNode(pNode, worldMat, pNodeName);

		pNode = pNode->NextSibling("node");
	}
}

void ParseDAEScene(MFXMLNode *pSceneNode)
{
	MFXMLNode *pScene;
	pScene = pSceneNode->FirstChild();

	while(pScene)
	{
		const char *pValue = pScene->Name();

		if(!MFString_CaseCmp(pValue, "instance_visual_scene"))
		{
			const char *pURL = pScene->Attribute("url");

			MFXMLNode *pSceneRoot = GetSceneLib(pURL);

			if(pSceneRoot)
			{
				const char *pName = pSceneRoot->Attribute("name");
				pModel->name = pName;

				ParseSceneNode(pSceneRoot, MFMatrix::identity, "");
			}
			else
			{
				// scene not found..
				MFDebug_Warn(1, MFStr("Scene '%s' not found!\n", pURL));
			}
		}

		pScene = pScene->NextSibling();
	}
}

void ParseDAERoot(MFXMLNode *pRoot)
{
	MFXMLNode *pElement = pRoot->FirstChild();

	while(pElement)
	{
		const char *pValue = pElement->Name();

		if(!MFString_CaseCmp(pValue, "asset"))
		{
			ParseDAEAsset(pElement);
		}
		// i should probably build the library here
/*
		else if(!MFString_CaseCmp(pValue, "library"))
		{
			ParseDAELibrary(pElement);
		}
*/
		// but for the time being i'll just parse the scene graph and refer to the library the slow way (manually)
		else if(!MFString_CaseCmp(pValue, "scene"))
		{
			ParseDAEScene(pElement);
		}


		pElement = pElement->NextSibling();
	}
}

int F3DFile::ReadDAE(const char *pFilename)
{
	pModel = this;

	transformMatrix = MFMatrix::identity;

	// attempt to load the xml document
	MFDocumentXML *pDoc = MFParseXML_ParseFile(pFilename);
	if(!pDoc)
	{
		MFDebug_Warn(2, MFStr("Failed to load collada file '%s' for conversion..\n", pFilename));
		return -1;
	}

	pRoot = MFParseXML_RootNode(pDoc, "COLLADA");
	if(!pRoot)
	{
		MFDebug_Warn(2, "Document has no root node..\n");
		MFParseXML_DestroyDocument(pDoc);
		return 1;
	}

	pModel->name = "Untitled collada file";

	ParseDAERoot(pRoot);

	MFParseXML_DestroyDocument(pDoc);
	return 0;
}

void ParseDAEFileFromMemory(char *pFile, size_t size, F3DFile *_pModel)
{
	pModel = _pModel;

	transformMatrix = MFMatrix::identity;

	// attempt to load the xml document
	MFDocumentXML *pDoc = MFParseXML_Parse(pFile);
	if(!pDoc)
	{
		MFDebug_Warn(2, "Failed to load collada file for conversion..\n");
		return;
	}

	pRoot = MFParseXML_RootNode(pDoc, "COLLADA");
	if(!pRoot)
	{
		MFDebug_Warn(2, "Document has no root node..\n");
		MFParseXML_DestroyDocument(pDoc);
		return;
	}

	pModel->name = "Untitled collada file";

	ParseDAERoot(pRoot);

	MFParseXML_DestroyDocument(pDoc);
}
