#include "Fuji.h"
#include "F3D.h"
#include "MFMatrix.h"

#include "tinyxml/tinyxml.h"

#include <string>
#include <vector>

extern F3DFile *pModel;

MFMatrix transformMatrix;

void startElement(void *userData, const char *name, const char **atts)
{
  int i;
  int *depthPtr = (int*)userData;
  for (i = 0; i < *depthPtr; i++)
    putchar('\t');
  puts(name);
  *depthPtr += 1;
}

void endElement(void *userData, const char *name)
{
  int *depthPtr = (int*)userData;
  *depthPtr -= 1;
}

void ParseDAEAsset(TiXmlElement *pAsset)
{
	TiXmlElement *pUp = pAsset->FirstChildElement("up_axis");
	TiXmlElement *pUnit = pAsset->FirstChildElement("unit");
	TiXmlElement *pAuthor = pAsset->FirstChildElement("author");
	TiXmlElement *pCopyright = pAsset->FirstChildElement("copyright");
	TiXmlElement *pAuthoringTool = pAsset->FirstChildElement("authoring_tool");
/*
	TiXmlElement *pSourceData = pAsset->FirstChildElement("source_data");
	TiXmlElement *pLastModified = pAsset->FirstChildElement("modified");
*/

	if(pUp)
	{
		// the axiis need to be swapped about into ingame axiis
		const char *pAxis = pUp->GetText();

		MFVector x, y, z;
		x = transformMatrix.GetXAxis();
		y = transformMatrix.GetYAxis();
		z = transformMatrix.GetZAxis();

		if(!stricmp(pAxis, "Y_UP"))
		{
			transformMatrix.SetXAxis4(x);
			transformMatrix.SetYAxis4(y);
			transformMatrix.SetZAxis4(z);
		}
		else if(!stricmp(pAxis, "Z_UP"))
		{
			transformMatrix.SetXAxis4(x);
			transformMatrix.SetYAxis4(z);
			transformMatrix.SetZAxis4(y);
		}
	}

	if(pUnit)
	{
		// we need to scale the model into metres..
		const char *pMeter = pUnit->Attribute("meter");
		float scale = (float)atof(pMeter);
		transformMatrix.Scale(MakeVector(1.0f / scale));
	}

	if(pAuthor)
	{
		// get the author (for kicks)
		const char *pAuth = pAuthor->GetText();
		strcpy(pModel->author, pAuth);
	}

	if(pCopyright)
	{
		// get the author (for kicks)
		const char *pCopyrightString = pCopyright->GetText();
		strcpy(pModel->copyrightString, pCopyrightString);
	}

	if(pAuthoringTool)
	{
		// get the author (for kicks)
		const char *pAuthTool = pAuthoringTool->GetText();
		strcpy(pModel->authoringTool, pAuthTool);
	}
}

void ParseDAEMaterial(TiXmlElement *pMaterialNode)
{
	const char *pID = pMaterialNode->Attribute("id");
	const char *pName = pMaterialNode->Attribute("name");

	F3DMaterialChunk *pMatChunk = pModel->GetMaterialChunk();

	if(pMatChunk->GetMaterialIndexByName(pName) != -1)
		return;

	F3DMaterial &mat = pMatChunk->materials.push();
	strcpy(mat.name, pName);
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
	if(!stricmp(pType, "POSITION"))
	{
		return CT_Position;
	}
	else if(!stricmp(pType, "NORMAL"))
	{
		return CT_Normal;
	}
	else if(!stricmp(pType, "TEXCOORD"))
	{
		return CT_UV1;
	}

	MFDebug_Assert(false, MFStr("Unknown semantic: '%s'", pType));
	return CT_Unknown;
}

class SourceData
{
public:
	SourceData() {}
	~SourceData() {}
	std::string id;
	std::vector<std::vector<float> > data;
	int validComponents;
};

std::vector<SourceData> sourceData;

SourceData* GetSourceData(const char *pSourceData)
{
	if(*pSourceData == '#')
		++pSourceData;

	for(int a=0; a<(int)sourceData.size(); a++)
	{
		if(!stricmp(pSourceData, sourceData[a].id.c_str()))
		{
			return &sourceData[a];
		}
	}

	return NULL;
}

void ReadSourceData(TiXmlElement *pSource, SourceData &data)
{
	TiXmlElement *pTechnique = pSource->FirstChildElement("technique");

	while(pTechnique)
	{
		const char *pProfile = pTechnique->Attribute("profile");

		if(!stricmp(pProfile, "COMMON"))
		{
			TiXmlElement *pAccessor = pTechnique->FirstChildElement("accessor");

			int count, stride;
			pAccessor->Attribute("count", &count);
			pAccessor->Attribute("stride", &stride);

			data.validComponents = stride;
			data.data.resize(count);
			break;
		}

		pTechnique = pTechnique->NextSiblingElement("technique");
	}

	TiXmlElement *pArray;
	pArray = pSource->FirstChildElement("array");
	MFDebug_Assert(!pArray, "'array' not supported for vertex data.\n");
	pArray = pSource->FirstChildElement("int_array");
	MFDebug_Assert(!pArray, "'int_array' not supported for vertex data.\n");
	pArray = pSource->FirstChildElement("Name_array");
	MFDebug_Assert(!pArray, "'Name_array' not supported for vertex data.\n");

	pArray = pSource->FirstChildElement("float_array");
	MFDebug_Assert(pArray, "source has no array data.\n");

	int dataCount;
	pArray->Attribute("count", &dataCount);
	MFDebug_Assert(data.data.size() * data.validComponents == dataCount, "Not enough data in array for all data specified by the technique.\n");

	const char *pText = pArray->GetText();

	int i=0;
	int j=0;

	while(*pText)
	{
		data.data[j].push_back((float)atof(pText));

		while(*pText && !MFIsWhite(*pText))
			++pText;
		pText = MFSkipWhite(pText);

		++i;
		if(i >= data.validComponents)
		{
			i=0;
			++j;
		}
	}

	MFDebug_Assert(j == data.data.size(), "Inforect amount of data in array.\n");
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
	}

	return NULL;
}

void ParseDAEGeometry(TiXmlElement *pGeometryNode)
{
	const char *pID = pGeometryNode->Attribute("id");
	const char *pName = pGeometryNode->Attribute("name");

	F3DMeshChunk *pMeshChunk = pModel->GetMeshChunk();
	F3DSubObject &subObject = pMeshChunk->subObjects.push();

	strcpy(subObject.name, pName);

	TiXmlElement *pMesh = pGeometryNode->FirstChildElement("mesh");

	while(pMesh)
	{
		TiXmlElement *pMeshElement = pMesh->FirstChildElement();

		typedef std::pair<ComponentType, std::string> DataSource;
		typedef std::vector<DataSource> DataSources;
		std::vector<DataSources> components;
		components.resize(1);
		sourceData.clear();

		while(pMeshElement)
		{
			const char *pValue = pMeshElement->Value();

			if(!stricmp(pValue, "source"))
			{
				const char *pName = pMeshElement->Attribute("id");

				SourceData x;
				sourceData.push_back(x);
				SourceData &data = sourceData.back();
				data.id = pName;

				ReadSourceData(pMeshElement, data);
			}
			else if(!stricmp(pValue, "vertices"))
			{
				TiXmlElement *pInputs = pMeshElement->FirstChildElement("input");

				while(pInputs)
				{
					const char *pSemantic = pInputs->Attribute("semantic");
					const char *pSource = pInputs->Attribute("source");

					components[0].push_back(DataSource(GetComponentType(pSemantic), pSource));

					pInputs = pInputs->NextSiblingElement("input");
				}
			}
			else if(!stricmp(pValue, "polygons"))
			{

				TiXmlElement *pInputs = pMeshElement->FirstChildElement("input");

				MFDebug_Assert(pInputs, "No inputs for polygons.\n");

				const char *pSemantic = pInputs->Attribute("semantic");
				const char *pSource = pInputs->Attribute("source");

				MFDebug_Assert(!stricmp(pSemantic, "VERTEX"), "First input must be VERTEX.\n");

				// get mat sub structure
				F3DMaterialSubobject &matSub = subObject.matSubobjects.push();

				const char *pMaterial = pMeshElement->Attribute("material");

				if(pMaterial)
				{
					if(*pMaterial == '#')
						++pMaterial;

					matSub.materialIndex = pModel->GetMaterialChunk()->GetMaterialIndexByName(pMaterial);
				}

				pInputs = pInputs->NextSiblingElement("input");

				while(pInputs)
				{
					// need to skip the first item...
					const char *pSemantic = pInputs->Attribute("semantic");
					const char *pSource = pInputs->Attribute("source");

					DataSources t;
					t.push_back(DataSource(GetComponentType(pSemantic), pSource));
					components.push_back(t);

					pInputs = pInputs->NextSiblingElement("input");
				}

				TiXmlElement *pPolygon = pMeshElement->FirstChildElement("p");

				int numIndices = (int)components.size();
				int a,b,c,d;

				// copy the data into the arrays
				for(a=0; a<numIndices; a++)
				{
					DataSources sources = components[a];

					for(b=0; b<(int)sources.size(); b++)
					{
						switch(sources[b].first)
						{
							case CT_Position:
							case CT_Normal:
							case CT_UV1:
							case CT_Colour:
							case CT_Binormal:
							case CT_Tangent:
							{
								SourceData *pData = GetSourceData(sources[b].second.c_str());

								if(pData)
								{
									MFArray<MFVector> *pDataArray = GetSemanticArray(subObject, sources[b].first);

									for(c=0; c<(int)pData->data.size(); c++)
									{
										MFVector &v = pDataArray->push();

										for(d=0; d<MFMin(pData->validComponents, 4); d++)
										{
											v[d] = pData->data[c][d];
										}
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

				// build the vertex and face lists
				int tri=0;

				while(pPolygon)
				{
					const char *pText = pPolygon->GetText();

					int point=0;
					int firstVert = matSub.vertices.size();

					while(*pText)
					{
						if(point > 2)
						{
							++tri;
							matSub.triangles[tri].v[0] = firstVert;
							matSub.triangles[tri].v[1] = matSub.vertices.size()-1;
							matSub.triangles[tri].v[2] = matSub.vertices.size();
						}
						else
						{
							matSub.triangles[tri].v[point] = matSub.vertices.size();
						}

						F3DVertex &v = matSub.vertices.push();

						for(a=0; a<numIndices; a++)
						{
							int index = atoi(pText);

							DataSources sources = components[a];

							for(b=0; b<(int)sources.size(); b++)
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
										v.uv1 = index;
										break;
									case CT_Colour:
										v.colour = index;
										break;
									case CT_Weights:
									{
										SourceData *pData = GetSourceData(sources[b].second.c_str());

										if(pData)
										{
											for(c=0; c<MFMin(pData->validComponents, 4); c++)
											{
												v.weight[c] = pData->data[index][c];
											}
										}
										break;
									}
									case CT_Indices:
									{
										SourceData *pData = GetSourceData(sources[b].second.c_str());

										if(pData)
										{
											for(c=0; c<MFMin(pData->validComponents, 4); c++)
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

							while(*pText && !MFIsWhite(*pText))
								++pText;
							pText = MFSkipWhite(pText);
						}

						++point;
					}

					++tri;

					pPolygon = pPolygon->NextSiblingElement("p");
				}
			}

			pMeshElement = pMeshElement->NextSiblingElement();
		}

		pMesh = pMesh->NextSiblingElement("mesh");
	}
}

void ParseDAEAnimation(TiXmlElement *pSceneNode)
{
	
}

void ParseDAELight(TiXmlElement *pLightNode)
{

}

void ParseDAECamera(TiXmlElement *pCameraNode)
{

}

void ParseDAEScene(TiXmlElement *pSceneNode)
{
	
}

void ParseDAERootElement(TiXmlElement *pRoot)
{
	TiXmlElement *pElement = pRoot->FirstChildElement();

	while(pElement)
	{
		const char *pValue = pElement->Value();

		if(!stricmp(pValue, "asset"))
		{
			ParseDAEAsset(pElement);
		}
		else if(!stricmp(pValue, "library"))
		{
//			const char *pType = pValue->Attribute("type");
			ParseDAERootElement(pElement);
		}
		else if(!stricmp(pValue, "material"))
		{
			ParseDAEMaterial(pElement);
		}
		else if(!stricmp(pValue, "geometry"))
		{
			ParseDAEGeometry(pElement);
		}
		else if(!stricmp(pValue, "animation"))
		{
			ParseDAEAnimation(pElement);
		}
		else if(!stricmp(pValue, "camera"))
		{
			ParseDAECamera(pElement);
		}
		else if(!stricmp(pValue, "image"))
		{
//			MFDebug_Assert(false, "Not written...");
		}
		else if(!stricmp(pValue, "light"))
		{
			ParseDAELight(pElement);
		}
		else if(!stricmp(pValue, "texture"))
		{
//			MFDebug_Assert(false, "Not written...");
		}
		else if(!stricmp(pValue, "scene"))
		{
			ParseDAEScene(pElement);
		}

		pElement = pElement->NextSiblingElement();
	}
}

int F3DFile::ReadDAE(char *pFilename)
{
	transformMatrix = MFMatrix::identity;

	TiXmlDocument doc(pFilename);
	bool loadOkay = doc.LoadFile();

	if(!loadOkay)
	{
		printf("Failed to load collada file '%s' for conversion..\n", pFilename);
		return -1;
	}

	TiXmlElement* pRoot = doc.FirstChildElement("COLLADA");

	if(!pRoot)
	{
		printf("Document has no root node..\n");
		return 1;
	}

	strcpy(pModel->name, "collada file");

	ParseDAERootElement(pRoot);

	return 0;
}
