/**********************************************************************
 *<
	FILE: ExporterPlugin.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 2003, All Rights Reserved.
 **********************************************************************/

#include "ExporterPlugin.h"
//#include "MAX_Mem.h"
#include <IGame.h>
#include <IConversionManager.h>
#include <IGameModifier.h>

#include "Common.h"
#include "Array.h"
#include "F3D.h"

IGameScene *pIGame;
F3DFile output;

#define ExporterPlugin_CLASS_ID	Class_ID(0x8f5d2ac4, 0x97537f49)

class ExporterPlugin : public SceneExport
{
	public:
		static HWND hParams;

		int				ExtCount();					// Number of extensions supported
		const TCHAR *	Ext(int n);					// Extension #n (i.e. "3DS")
		const TCHAR *	LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
		const TCHAR *	ShortDesc();				// Short ASCII description (i.e. "3D Studio")
		const TCHAR *	AuthorName();				// ASCII Author name
		const TCHAR *	CopyrightMessage();			// ASCII Copyright message
		const TCHAR *	OtherMessage1();			// Other message #1
		const TCHAR *	OtherMessage2();			// Other message #2
		unsigned int	Version();					// Version number * 100 (i.e. v3.01 = 301)
		void			ShowAbout(HWND hWnd);		// Show DLL's "About..." box

		BOOL	SupportsOptions(int ext, DWORD options);
		int		DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);

		//Constructor/Destructor
		ExporterPlugin();
		~ExporterPlugin();
};

class ExporterPluginClassDesc : public ClassDesc2
{
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new ExporterPlugin; }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
	Class_ID		ClassID() { return ExporterPlugin_CLASS_ID; }
	// The Skin modifier checks the category to decide whether the modifier is a Skin Gizmo.  This 
	// must not be changed
	const TCHAR* 	Category() { return GetString(IDS_PW_GIZMOCATEGORY); }

	const TCHAR*	InternalName() { return _T("ExporterPlugin"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }					// returns owning module handle

	BOOL IsManipulator() { return TRUE; }

	/********************************************************************************************************
	*
	Returns true if the class is a manipulator and it can manipulates the ReferenceTarget passed 
	to it.  In this case it simple states that it can manipulate itself.    
	*
	\********************************************************************************************************/

	BOOL CanManipulate(ReferenceTarget* hTarget)
	{
		//TODO: Return TRUE if it can manipulate the ReferenceTarget.
        return hTarget->ClassID() == ClassID() && hTarget->SuperClassID() == SuperClassID();
    }

//    Manipulator* CreateManipulator(RefTargetHandle hTarget, INode* pNode);

};

static ExporterPluginClassDesc ExporterPluginDesc;
ClassDesc2* GetExporterPluginDesc() { return &ExporterPluginDesc; }

/*===========================================================================*\
 |	Class Descriptor for the World Space Object
\*===========================================================================*/
/*
class ExporterPluginObjClassDesc : public ClassDesc2 {
	public:
	int 			IsPublic()							{ return TRUE; }
	void *			Create(BOOL loading = FALSE)		{ return new ExporterPluginObject; }
	const TCHAR *	ClassName()							{ return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID()						{ return WSM_OBJECT_CLASS_ID; }
	Class_ID		ClassID()							{ return WARPOBJ_CLASS_ID; }
	const TCHAR* 	Category()							{ return _T("");}

	// Hardwired name, used by MAX Script as unique identifier
	const TCHAR*	InternalName()						{ return _T("ExporterPluginObject"); }
	HINSTANCE		HInstance()							{ return hInstance; }
};

static ExporterPluginObjClassDesc ExporterPluginObjDesc;
ClassDesc2* GetExporterPluginObjDesc() { return &ExporterPluginObjDesc; }
*/


enum { exporterplugin_params };

enum {obj_params};

//TODO: Add enums for various parameters
enum
{ 
	pb_spin,
	pb_coords,
	pb_param1,
	pb_param2,
	pb_quality,
	pb_enable,
	pb_adapt_enable,
	pb_adapt_threshold,
	pb_diffuse, 
	pb_brightness,
	pb_gizmoparam_name,
	pb_gizmos,
	mtl_mat1,
	mtl_mat1_on,

};

// For the Space Warp Object
enum
{
	pb_spin_obj
};

/************************************************************************************************
*
	The following PBAccessor allows the Skin Modifier's Gizmo list to be kept upto date when the 
	name changes in the gizmo rollout
*
\************************************************************************************************/
/*
class ExporterPluginPBAccessor : public PBAccessor
{ 
public:
	void Set(PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t)    // set from v
	{
		ExporterPlugin* p = (ExporterPlugin*)owner;

		switch (id)
		{
			//Check all name changes and pass them onto the Skin modifier
			case pb_gizmoparam_name:

					if (p->bonesMod)
						p->bonesMod->UpdateGizmoList();
				
				break;
		}
	}
};
static ExporterPluginPBAccessor gizmoJoint_accessor;

static ParamBlockDesc2 exporterplugin_param_blk ( exporterplugin_params, _T("params"),  0, &ExporterPluginDesc, 
	P_AUTO_CONSTRUCT, PBLOCK_REF,
	pb_coords,			_T("coords"),		TYPE_REFTARG,	P_OWNERS_REF,	IDS_COORDS,
		p_refno,		COORD_REF, 
		end,
	pb_param1, 		_T("param1"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_PARAM1,
		p_default,		0.3f,
		end,
	pb_param2, 		_T("param2"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_PARAM2,
		p_default,		0.6f,
		end,
	pb_quality, 		_T("quality"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_QUALITY,
		p_default,		0.5f,
		end,
	pb_enable, 			_T("enable"), 		TYPE_BOOL, 		0, 				IDS_ENABLE,
		p_default,		FALSE,
		end,
	pb_adapt_enable, 	_T("adaptive"), 	TYPE_BOOL, 		0, 				IDS_AD_ENABLE,
		p_default,		TRUE,
		end,
	pb_adapt_threshold, _T("threshold"), 	TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_AD_THRESH,
		p_default,		0.02f,
		end,
	// params
	pb_diffuse, _T("diffuse"), TYPE_RGBA, P_ANIMATABLE, IDS_MN_DIFFUSE, 
		p_default, Color(0.8f, 0.5f, 0.5f), 
		end,
	pb_brightness, _T("brightness"), TYPE_PCNT_FRAC, P_ANIMATABLE, IDS_MN_BRIGHTNESS,
		p_default,		0.2f,
		p_range,		0.0f, 1.0f,
		end,
	pb_gizmoparam_name, 	_T("name"),		TYPE_STRING, 	0,  IDS_GIZMOPARAM_NAME,
		p_ui,  TYPE_EDITBOX,  IDC_NAME,
		p_accessor,		&gizmoJoint_accessor,
		end, 		
	pb_gizmos,    _T("Gizmos"),  TYPE_INODE_TAB, 0, P_AUTO_UI,	IDS_GIZMOS,
		p_ui,	TYPE_NODELISTBOX, IDC_GIZMOLIST,	IDC_ADDGIZMO,	0,	IDC_DELGIZMO,
		end,

	mtl_mat1,			_T("mtl_mat1"),			TYPE_MTL,	P_OWNERS_REF,	IDS_MTL1,
		p_refno,		0,
		p_submtlno,		0,		
		p_ui,			TYPE_MTLBUTTON, IDC_MTL1,
		end,
	mtl_mat1_on,		_T("mtl_mat1_on"),		TYPE_BOOL,		0,				IDS_MTL1ON,
		p_default,		TRUE,
		p_ui,			TYPE_SINGLECHEKBOX, IDC_MTLON1,
		end,

	end
	);

static ParamBlockDesc2 exporterpluginobj_param_blk ( obj_params, _T("ExporterPluginObjParams"),  0, &ExporterPluginObjDesc, P_AUTO_CONSTRUCT + P_AUTO_UI, 0, 
	//rollout
	IDD_PANEL_OBJ, IDS_PARAMS, 0, 0, NULL, 
	// params
	pb_spin_obj, 			_T("spin"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_SPIN_OBJ, 
		p_default, 		0.1f, 
		p_range, 		0.0f,1000.0f, 
		p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_EDIT_OBJ,	IDC_SPIN_OBJ, 0.01f, 
		end,

	end
	);
*/



BOOL CALLBACK ExporterPluginOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	static ExporterPlugin *imp = NULL;

	switch(message) {
		case WM_INITDIALOG:
			imp = (ExporterPlugin *)lParam;
			CenterWindow(hWnd,GetParent(hWnd));
			return TRUE;

		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return TRUE;

		case WM_COMMAND:
			switch(LOWORD(IDC_CLOSEBUTTON))
			{
				case IDC_CLOSEBUTTON:
					EndDialog(hWnd, 0);
					break;
			}
			break;
	}
	return FALSE;
}


//--- ExporterPlugin -------------------------------------------------------
ExporterPlugin::ExporterPlugin()
{

}

ExporterPlugin::~ExporterPlugin() 
{

}

int ExporterPlugin::ExtCount()
{
	//TODO: Returns the number of file name extensions supported by the plug-in.
	return 1;
}

const TCHAR *ExporterPlugin::Ext(int n)
{		
	//TODO: Return the 'i-th' file name extension (i.e. "3DS").
	return _T("f3d");
}

const TCHAR *ExporterPlugin::LongDesc()
{
	//TODO: Return long ASCII description (i.e. "Targa 2.0 Image File")
	return _T("Fuji Geometry and Animation File");
}
	
const TCHAR *ExporterPlugin::ShortDesc() 
{			
	//TODO: Return short ASCII description (i.e. "Targa")
	return _T("Mt. Fuji 3D");
}

const TCHAR *ExporterPlugin::AuthorName()
{			
	//TODO: Return ASCII Author name
	return _T("Manu Evans");
}

const TCHAR *ExporterPlugin::CopyrightMessage() 
{	
	// Return ASCII Copyright message
	return _T("");
}

const TCHAR *ExporterPlugin::OtherMessage1() 
{		
	//TODO: Return Other message #1 if any
	return _T("");
}

const TCHAR *ExporterPlugin::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int ExporterPlugin::Version()
{				
	//TODO: Return Version number * 100 (i.e. v3.01 = 301)
	return 010;
}

void ExporterPlugin::ShowAbout(HWND hWnd)
{			
	// Optional
}

BOOL ExporterPlugin::SupportsOptions(int ext, DWORD options)
{
	// TODO Decide which options to support.  Simply return
	// true for each option supported by each Extension 
	// the exporter supports.

	return TRUE;
}

Matrix ToMatrix(const GMatrix &m)
{
	Matrix t;

	memcpy(&t, &m, sizeof(Matrix)); 

	return t;
}

Vector3 ToVector3(const Point3 &p)
{
	Vector3 t;

	t.x = p.x;
	t.y = p.y;
	t.z = p.z;

	return t;
}

Vector3 ToVector3(const Point2 &p)
{
	Vector3 t;

	t.x = p.x;
	t.y = p.y;
	t.z = 0.0f;

	return t;
}

Vector4 ToVector4(const Point4 &p)
{
	Vector4 t;

	t.x = p.x;
	t.y = p.y;
	t.z = p.z;
	t.w = p.w;

	return t;
}

Vector4 ToVector4(const Point3 &p)
{
	Vector4 t;

	t.x = p.x;
	t.y = p.y;
	t.z = p.z;
	t.w = 1.0f;

	return t;
}

Vector4 ToVector4(const Point2 &p)
{
	Vector4 t;

	t.x = p.x;
	t.y = p.y;
	t.z = 0.0f;
	t.w = 1.0f;

	return t;
}

bool IsRefPoint(char *pName)
{
	return strnicmp(pName, "r_", 2) == 0;
}

bool IsBone(char *pName)
{
	return strnicmp(pName, "z_", 2) == 0;
}

int GetBoneIndex(char *pName)
{
	for(int a=0; a<output.GetSkeletonChunk()->bones.size(); a++)
	{
		if(!stricmp(output.GetSkeletonChunk()->bones[a].name, pName)) return a;
	}

	return -1;
}

void ParseMaterial(IGameMaterial *pMat)
{
	int i;

	if(pMat->IsMultiType() || pMat->IsSubObjType())
	{
		for(i=0; i<pMat->GetSubMaterialCount(); i++)
		{
			ParseMaterial(pMat->GetSubMaterial(i));
		}

		return;
	}

	F3DMaterial &mat = output.GetMaterialChunk()->materials.push();

	char *pName = pMat->GetMaterialName();
	strcpy(mat.name, pName);

	Mtl *pMaxMat = pMat->GetMaxMaterial();

	Color col;
	float f;

	col = pMaxMat->GetDiffuse();
	mat.diffuse.x = col.r;
	mat.diffuse.y = col.g;
	mat.diffuse.z = col.b;
	mat.diffuse.w = 1.0f - pMaxMat->GetXParency();

	col = pMaxMat->GetAmbient();
	mat.ambient.x = col.r;
	mat.ambient.y = col.g;
	mat.ambient.z = col.b;
	mat.ambient.w = 1.0f;

	col = pMaxMat->GetSpecular();
	mat.specular.x = col.r;
	mat.specular.y = col.g;
	mat.specular.z = col.b;
	mat.specular.w = 1.0f;

	mat.specularLevel = pMaxMat->GetShinStr();
	mat.glossiness = pMaxMat->GetShininess();

	if(pMaxMat->GetSelfIllumColorOn())
	{
		col = pMaxMat->GetSelfIllumColor();
		mat.emissive.x = col.r;
		mat.emissive.y = col.g;
		mat.emissive.z = col.b;
		mat.emissive.w = 1.0f;
	}
	else
	{
		f = pMaxMat->GetSelfIllum();
		mat.emissive = mat.diffuse * f;
	}
}

void ParseBone(IGameNode *pNode)
{
	char *pName = pNode->GetName();

	if(IsBone(pName))
	{
		//IGameObject *pObject = pNode->GetIGameObject();

		F3DBone &bone = output.GetSkeletonChunk()->bones.push();

		strcpy(bone.name, pName);
		bone.localMatrix = ToMatrix(pNode->GetLocalTM(0));
		bone.worldMatrix = ToMatrix(pNode->GetWorldTM(0));

		IGameNode *pParent = pNode->GetNodeParent();
		strcpy(bone.parentName, pParent ? pParent->GetName() : "" );
		strcpy(bone.options, "");
	}

	// recurse children
	for(int a=0; a<pNode->GetChildCount(); a++)
	{
		ParseBone(pNode->GetNodeChild(a));
	}
}

void ParseMesh(IGameNode *pNode)
{
	int a;
//	int b;

	char *pName = pNode->GetName();

	if(!IsBone(pName) && !IsRefPoint(pName))
	{
		IGameObject *pObject = pNode->GetIGameObject();

		IGameObject::ObjectTypes type = pObject->GetIGameType();

		if(type == IGameObject::ObjectTypes::IGAME_MESH)
		{
			IGameMesh *pMesh = static_cast<IGameMesh*>(pObject);

			if(pMesh->InitializeData())
			{
				F3DSubObject *pSub = &output.GetMeshChunk()->subObjects.push();

				IGameSkin *pSkin = NULL;

				if(pMesh->IsObjectSkinned())
				{
					pSkin = pMesh->GetIGameSkin();

					DBGASSERT(pSkin->GetNumOfSkinnedVerts() == pMesh->GetNumberOfVerts(), "Not all vertices are skinned.");
				}

				for(a=0; a<pMesh->GetNumberOfVerts(); a++)
				{
					pSub->positions[a] = ToVector3(pMesh->GetVertex(a));
				}

				for(a=0; a<pMesh->GetNumberOfTexVerts(); a++)
				{
					pSub->uvs[a] = ToVector3(pMesh->GetTexVertex(a));
				}

				for(a=0; a<pMesh->GetNumberOfIllumVerts(); a++)
				{
					pSub->illumination[a].x = pMesh->GetIllumVertex(a);
					pSub->illumination[a].y = pMesh->GetIllumVertex(a);
					pSub->illumination[a].z = pMesh->GetIllumVertex(a);
				}
//				pSub->illumination[a] = Vector3::zero;

				DBGASSERT(pMesh->GetNumberOfAlphaVerts() == pMesh->GetNumberOfColorVerts(), "Incorrect number of alpha verts..");

				for(a=0; a<pMesh->GetNumberOfColorVerts(); a++)
				{
					pSub->colours[a] = ToVector4(pMesh->GetColorVertex(a));
					pSub->colours[a].w = pMesh->GetAlphaVertex(a);
				}

				Tab<int> mid = pMesh->GetActiveMatIDs();

				for(a=0; a<mid.Count(); a++)
				{
					F3DMaterialSubobject *pMatSub = &pSub->matSubobjects[a];

					pMatSub->materialIndex = mid[a];

					Tab<FaceEx*> faces = pMesh->GetFacesFromMatID(mid[a]);

					char *pName = pMesh->GetMaterialFromFace(faces[0])->GetMaterialName();

					strcpy(pSub->name, pName ? pName : "");  

					for(int b=0; b<faces.Count(); b++)
					{
						for(int v=0; v<3; v++)
						{
							pMatSub->triangles[b].v[v] = pMatSub->vertices.size();

							int vert = faces[b]->vert[v];
							F3DVertex *pVert = &pMatSub->vertices.push();

							pVert->position = vert;
							pVert->normal = faces[b]->norm[v];
							pVert->uv1 = faces[b]->texCoord[v];
							pVert->colour = faces[b]->color[v];
							pVert->illum = faces[b]->illum[v];

							pVert->biNormal = 0;
							pVert->tangent = 0;
							pVert->uv2 = 0;
							pVert->uv3 = 0;
							pVert->uv4 = 0;
							pVert->uv5 = 0;
							pVert->uv6 = 0;
							pVert->uv7 = 0;
							pVert->uv8 = 0;

							int w = 0;

							if(pSkin)
							{
								int bones = pSkin->GetNumberOfBones(vert);

								for(int c=0; c<bones && w<4; c++)
								{
									float weight = pSkin->GetWeight(vert, c);

									if(weight > 0.0f)
									{
										pVert->weight[w] = weight;
										pVert->bone[w] = GetBoneIndex(pSkin->GetIGameBone(vert, c)->GetName());
										w++;
									}
								}
							}

							while(w<4)
							{
								pVert->weight[w] = 0.0f;
								pVert->bone[w] = -1;
								w++;
							}
						}
					}
				}
/*
				Array<int> faceID(128);
				for(a=0; a<128; a++) faceID[a] = -1;

				int matSubCount = 0;

				for(a=0; a<pMesh->GetNumberOfFaces(); a++)
				{
					FaceEx *pFace = pMesh->GetFace(a);

					if(faceID[pFace->matID] == -1)
					{
						faceID[pFace->matID] = matSubCount;
						matSubCount++;
					}

					int matSub = faceID[pFace->matID];

					F3DTriangle &tri = pChunk->matSubs[matSub].triangles.push();

					for(b=0; b<3; b++)
					{
						int vert = pFace->vert[b];

						tri.v[b].position = vert;
						tri.v[b].uv1 = pFace->texCoord[b];
						tri.v[b].colour = pFace->color[b];
						tri.v[b].illum = pFace->illum[b];

						tri.v[b].normal = pChunk->normals.size();
						pChunk->normals.push(ToVector3(pMesh->GetNormal(pFace, b)));

						if(pSkin)
						{
							int bones = pSkin->GetNumberOfBones(vert);

							int w = 0;

							for(int c=0; c<bones && w<4; c++)
							{
								float weight = pSkin->GetWeight(vert, c);

								if(weight > 0.0f)
								{
									tri.v[b].weight[w] = weight;
									tri.v[b].bone[w] = GetBoneIndex(pSkin->GetIGameBone(vert, c)->GetName());
									w++;
								}
							}

							while(w<4)
							{
								tri.v[b].weight[w] = 0.0f;
								tri.v[b].bone[w] = -1;
								w++;
							}
						}
					}
				}
*/
			}
		}
	}

	// recurse children
	for(a=0; a<pNode->GetChildCount(); a++)
	{
		ParseMesh(pNode->GetNodeChild(a));
	}
}

void ParseRefPoint(IGameNode *pNode)
{
	char *pName = pNode->GetName();

	if(IsRefPoint(pName))
	{
		IGameObject *pObject = pNode->GetIGameObject();
	}

	// recurse children
	for(int a=0; a<pNode->GetChildCount(); a++)
	{
		ParseRefPoint(pNode->GetNodeChild(a));
	}
}

int	ExporterPlugin::DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options)
{
	int a;

	if(!suppressPrompts)
		DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_PANEL), GetActiveWindow(), ExporterPluginOptionsDlgProc, (LPARAM)this);

	pIGame = GetIGameInterface();

	pIGame->InitialiseIGame(true);

	GetConversionManager()->SetCoordSystem(IGameConversionManager::CoordSystem::IGAME_D3D);

	// scan material list
	for(a=0; a<pIGame->GetRootMaterialCount(); a++)
	{
		ParseMaterial(pIGame->GetRootMaterial(a));
	}

	// parse bones
	for(a=0; a<pIGame->GetTopLevelNodeCount(); a++)
	{
		ParseBone(pIGame->GetTopLevelNode(a));
	}

	// parse geometry
	for(a=0; a<pIGame->GetTopLevelNodeCount(); a++)
	{
		ParseMesh(pIGame->GetTopLevelNode(a));
	}

	// parse ref points
	for(a=0; a<pIGame->GetTopLevelNodeCount(); a++)
	{
		ParseRefPoint(pIGame->GetTopLevelNode(a));
	}

	output.WriteToDisk((char*)name);

	pIGame->ReleaseIGame();

	// return TRUE If the file is exported properly
	return FALSE;
}


