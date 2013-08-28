#if !defined(_MFINTMODEL_H)
#define _MFINTMODEL_H

enum MFIntModelFormat
{
	MFIMF_Unknown = -1,

	MFIMF_F3D = 0,	// Fuji 3D
	MFIMF_DAE,		// Collada
	MFIMF_X,		// DirectX
	MFIMF_ASE,		// 3DSMax Ascii
	MFIMF_OBJ,		// Wavefront Object
	MFIMF_MD2,		// Quake II
	MFIMF_MD3,		// Quake III
	MFIMF_MEMD2,	// ManuEngine Model 2

	// via assimp
	MFIMF_BLEND,	// Blender
	MFIMF_3DS,		// 3DSMax
	MFIMF_DXF,		// AutoCAD DXF
	MFIMF_LWO,		// LightWave
	MFIMF_LWS,		// LightWave Scene
	MFIMF_MS3D,		// Milkshape 3D
	MFIMF_MDL,		// Quake I
	MFIMF_PK3,		// Quake III Map/BSP
	MFIMF_MDC,		// Return to Castle Wolfenstein
	MFIMF_MD5,		// Doom 3
	MFIMF_SMD,		// Valve Model ( .smd )
	MFIMF_VTA,		// Valve Model ( .vta )
	MFIMF_M3,		// Starcraft II M3
	MFIMF_3D,		// Unreal
/*
	other assimp formats:

	COMMON INTERCHANGE FORMATS
		Industry Foundation Classes (IFC/Step) ( .ifc )
		XGL ( .xgl,.zgl )
		Stanford Polygon Library ( .ply )
		Modo ( .lxo )
		Stereolithography ( .stl )
		AC3D ( .ac )
		* TrueSpace ( .cob,.scn )
	MOTION CAPTURE FORMATS
		Biovision BVH ( .bvh )
		* CharacterStudio Motion ( .csm )
	GRAPHICS ENGINE FORMATS
		Ogre XML ( .xml )
		Irrlicht Mesh ( .irrmesh )
		* Irrlicht Scene ( .irr )
	OTHER FILE FORMATS
		BlitzBasic 3D ( .b3d )
		Quick3D ( .q3d,.q3s )
		Neutral File Format ( .nff )
		Sense8 WorldToolKit ( .nff )
		Object File Format ( .off )
		PovRAY Raw ( .raw )
		Terragen Terrain ( .ter )
		3D GameStudio (3DGS) ( .mdl )
		3D GameStudio (3DGS) Terrain ( .hmp )
		Izware Nendo ( .ndo )
*/

	MFIMF_Max,
	MFIMF_ForceInt = 0x7FFFFFFF
};

struct MFIntModel;

MF_API MFIntModel *MFIntModel_CreateFromFile(const char *pFilename);
MF_API MFIntModel *MFIntModel_CreateFromFileInMemory(const void *pMemory, size_t size, MFIntModelFormat format, const char *pName);

MF_API void MFIntModel_Optimise(MFIntModel *pModel);

MF_API void MFIntModel_CreateRuntimeData(MFIntModel *pModel, void **ppOutput, size_t *pSize, MFPlatform platform, size_t extraBytes = 0);
MF_API void MFIntModel_CreateAnimationData(MFIntModel *pModel, void **ppOutput, size_t *pSize, MFPlatform platform, size_t extraBytes = 0);

MF_API void MFIntModel_Destroy(MFIntModel *pModel);

#endif
