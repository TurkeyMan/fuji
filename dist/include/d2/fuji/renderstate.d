module fuji.renderstate;

public import fuji.fuji;
import fuji.texture;
import fuji.matrix;
import fuji.vertex;

enum MFStateConstant_Matrix : int
{
	Unknown = -1,

	World = 0,
	Camera,
	Projection,
	ShadowMap,
	Fuji0,
	Fuji1,
	User0,
	User1,
	User2,
	User3,
	UV0,
	UV1,
	UV2,
	UV3,
	View,
	WorldView,
	ViewProjection,
	WorldViewProjection,
	InverseWorld,
	InverseViewProjection,

	Max,

	ManagedStart = World,
	NumManaged = ShadowMap - ManagedStart,

	DerivedStart = View,
	NumDerived = Max - DerivedStart,

	NumSetable = DerivedStart,

	FujiMatrixCount = User0 - Fuji0,
	UserMatrixCount = UV0 - User0,
	UVMatrixCount = View - UV0
}

template MFSCM_FujiMat(size_t i) { enum MFStateConstant_Matrix MFSCM_FujiMat = MFStateConstant_Matrix.FujiMat0 + i; }
template MFSCM_UserMat(size_t i) { enum MFStateConstant_Matrix MFSCM_UserMat = MFStateConstant_Matrix.UserMat0 + i; }
template MFSCM_UV(size_t i)      { enum MFStateConstant_Matrix MFSCM_UV = MFStateConstant_Matrix.UV0 + i; }

enum MFStateConstant_Vector : int
{
	Unknown = -1,

	Time = 0,				/**< x = game time in seconds, y = tile delta, z = 1 / x, w = 1 / y */
	FogColour,
	FogParams1,
	FogParams2,
	RenderState,			/**< x = alpha ref, ...? */
	MaterialDiffuseColour,
	DiffuseColour,
	AmbientColour,

	Fuji0, Fuji1, Fuji2, Fuji3, Fuji4, Fuji5, Fuji6,

	LightCounts,			/**< x = directional, y = omni count, z = spot count */

	User0, User1, User2, User3, User4, User5, User6, MUser7,
	User8, User9, User10, User11, User12, User13, User14, User15,

	Max,

	FujiVectorCount = LightCounts - Fuji0,
	UserectorCount = Max - User0
}

//#define MFSCV_Fuji(i) (MFStateConstant_Vector)(MFSCV_Fuji0 + i)
//#define MFSCV_User(i) (MFStateConstant_Vector)(MFSCV_User0 + i)

enum MFStateConstant_Texture : int
{
	Unknown = -1,

	Diffuse = 0,
	NormalMap,
	SpecularMap,
	DetailMap,
	OpacityMap,
	EnvironmentMap,
	SpecularPowerMap,
	EmissiveMap,
	LightMap,
	ShadowBuffer,
	Projector,
	User0,
	User1,
	User2,
	User3,
	User4,
	Vertex0,
	Vertex1,
	Vertex2,

	Max,

	FujiTextureCount = 0,
	UserTextureCount = Vertex0 - User0,
	VertexTextureCount = Max - Vertex0,
}

//#define MFSCT_Fuji(i) (MFStateConstant_Texture)(MFSCT_Max + i)
//#define MFSCT_User(i) (MFStateConstant_Texture)(MFSCT_User0 + i)
//#define MFSCT_Vertex(i) (MFStateConstant_Texture)(MFSCT_Vertex0 + i)

enum MFStateConstant_Bool
{
	Unknown = -1,

	Animated = 0,
	ZPrime,
	ShadowGeneration,
	ShadowReceiving,
	Opaque,
	AlphaTest,
	Fuji0,
	Fuji1,
	Fuji2,
	User0,
	User1,
	User2,
	User3,

	DiffuseSet,
	NormalMapSet,
	SpecularMapSet,
	DetailMapSet,
	OpacityMapSet,
	EnvironmentMapSet,
	SpecularPowerMapSet,
	EmissiveMapSet,
	LightMapSet,
	ShadowBufferSet,
	ProjectorSet,
	UserTex0Set,
	UserTex1Set,
	UserTex2Set,
	UserTex3Set,
	UserTex4Set,
	VertexTex0Set,
	VertexTex1Set,
	VertexTex2Set,

	Max,

	FujiBoolCount = User0 - Fuji0,
	UserBoolCount = DiffuseSet - User0
}

//#define MFSCB_Fuji(i) (MFStateConstant_Bool)(MFSCB_Fuji0 + i)
//#define MFSCB_User(i) (MFStateConstant_Bool)(MFSCB_User0 + i)
//#define MFSCB_TexSet(i) (MFStateConstant_Bool)(MFSCB_DiffuseSet + i)

enum MFStateConstant_RenderState
{
	Unknown= -1,

	BlendState = 0,
	DepthStencilState,
	RasteriserState,

	DiffuseSamplerState,
	NormalMapSamplerState,
	SpecularMapSamplerState,
	DetailMapSamplerState,
	OpacityMapSamplerState,
	EnvironmentMapSamplerState,
	SpecularPowerMapSamplerState,
	EmissiveMapSamplerState,
	LightMapSamplerState,
	ShadowBufferSamplerState,
	ProjectorSamplerState,
	UserTex0SamplerState,
	UserTex1SamplerState,
	UserTex2SamplerState,
	UserTex3SamplerState,
	UserTex4SamplerState,
	VertexTex0SamplerState,
	VertexTex1SamplerState,
	VertexTex2SamplerState,

	VertexDeclaration,
	VertexBuffer0,
	VertexBuffer1,
	VertexBuffer2,
	VertexBuffer3,
	VertexBuffer4,
	VertexBuffer5,
	VertexBuffer6,
	VertexBuffer7,
	IndexBuffer,

	Max,

	VertexBufferCount = VertexBuffer0 - IndexBuffer
}

//#define MFSCRS_SamplerState(tex) (MFStateConstant_RenderState)(MFSCRS_DiffuseSamplerState + tex)
//#define MFSCRS_VertexBuffer(i) (MFStateConstant_RenderState)(MFSCRS_VertexBuffer0 + i)

enum MFStateConstant_Miscellaneous
{
	AnimationMatrices,
	MatrixBatch,
	Light0,
	Light1,
	Light2,
	Light3,
	Light4,
	Light5,
	Light6,
	Light7,
	Light8,
	Light9,
	Light10,
	Light11,
	Light12,
	Light13,
	Light14,
	Light15,
	Light16,
	GPUEvent,

	Max,

	LightCount = GPUEvent - Light0
}

//#define MFSCMisc_Light(i) (MFStateConstant_Miscellaneous)(MFSCMisc_Light0 + i)

struct MFStateConstant_AnimationMatrices
{
	MFMatrix *pMatrices;
	size_t numMatrices;
}

struct MFStateConstant_MatrixBatch
{
	ushort *pIndices;
	uint numMatrices;
}

enum MFBlendOp
{
	Unknown = -1,

	Add = 0,
	Subtract,
	RevSubtract,
	Min,
	Max,

	BlendOpCount
}

enum MFBlendArg
{
	Unknown = -1,

	Zero = 0,
	One,
	SrcColour,
	InvSrcColour,
	SrcAlpha,
	InvSrcAlpha,
	DestColour,
	InvDestColour,
	DestAlpha,
	InvDestAlpha,
	SrcAlphaSaturate,

	BlendFactor,
	InvBlendFactor,
	Src1Colour,
	InvSrc1Colour,
	Src1Alpha,
	InvSrc1Alpha
}

enum MFColourWriteEnable
{
	None = 0,

	Red = 1,
	Green = 2,
	Blue = 4,
	Alpha = 8,

	All = (Red | Green | Blue | Alpha)
}

enum MFTexFilter
{
	Unknown = -1,

	None,
	Point,
	Linear,
	Anisotropic,
//	Text_1Bit
}

enum MFTexAddressMode
{
	Unknown = -1,

	Wrap = 0,
	Mirror,
	Clamp,
	Border,
	MirrorOnce
}

enum MFComparisonFunc
{
	Unknown = -1,

	Never = 0,
	Less,
	Equal,
	LessEqual,
	Greater,
	NotEqual,
	GreaterEqual,
	Always
}

enum MFDepthWriteMask
{
	Unknown = -1,

	Zero = 0,
	All = 1
}

enum MFStencilOp
{
	Unknown = -1,

	Keep = 0,
	Zero,
	Replace,
	IncrementClamp,
	DecrementClamp,
	Invert,
	Increment,
	Decrement
}

enum MFFillMode
{
	Unknown = -1,

	Solid = 0,
	Wireframe,
	Point
}

enum MFCullMode
{
	Unknown = -1,

	None = 0,
	CCW,
	CW
}

enum MFStateBlockConstantType
{
	Bool = 0,
	Vector,
	Matrix,
	Texture,
	RenderState,
	Misc,

	Unknown = 7
}

struct MFBlendState;

struct MFBlendStateDesc
{
	struct RenderTargetBlendDesc
	{
		MFBlendOp blendOp = MFBlendOp.Add;				/**< This blend operation defines how to combine the RGB data sources */
		MFBlendArg srcBlend = MFBlendArg.SrcAlpha;		/**< This blend option specifies the first RGB data source and includes an optional pre-blend operation */
		MFBlendArg destBlend = MFBlendArg.InvSrcAlpha;	/**< This blend option specifies the second RGB data source and includes an optional pre-blend operation */
		MFBlendOp blendOpAlpha = MFBlendOp.Add;			/**< This blend operation defines how to combine the alpha data sources */
		MFBlendArg srcBlendAlpha = MFBlendArg.One;		/**< This blend option specifies the first alpha data source and includes an optional pre-blend operation. Blend options that end in _COLOR are not allowed */
		MFBlendArg destBlendAlpha = MFBlendArg.Zero;	/**< This blend option specifies the second alpha data source and includes an optional pre-blend operation. Blend options that end in _COLOR are not allowed */
		ubyte writeMask = MFColourWriteEnable.All;		/**< A write mask */
		bool bEnable = false;					    	/**< Enable (or disable) blending */
	}

	bool bAlphaToCoverageEnable = false;    			/**< Use alpha-to-coverage as a multisampling technique when setting a pixel to a rendertarget. */
	bool bIndependentBlendEnable = false;	    		/**< Enables independent blending to simultaneous render targets. If false, only the renderTarget[0] members are used. RenderTarget[1..7] are ignored. */
	RenderTargetBlendDesc renderTarget[8];	            /**< Array of RenderTargetBlendDesc; corresponding to the eight rendertargets that can be set to the output-merger stage at one time. */
}

struct MFSamplerState;

struct MFSamplerStateDesc
{
	MFTexFilter minFilter = MFTexFilter.Linear;
	MFTexFilter magFilter = MFTexFilter.Linear;
	MFTexFilter mipFilter = MFTexFilter.Linear;
	MFTexAddressMode addressU = MFTexAddressMode.Wrap;
	MFTexAddressMode addressV = MFTexAddressMode.Wrap;
	MFTexAddressMode addressW = MFTexAddressMode.Wrap;
	bool bEnableComparison = false;
	MFComparisonFunc comparisonFunc = MFComparisonFunc.Never;
	uint maxAnisotropy = 16;
	float mipLODBias = 0.0f;
	float minLOD = -float.max;
	float maxLOD = float.max;
	MFVector borderColour = MFVector.zero;
}

struct MFDepthStencilState;

struct MFDepthStencilStateDesc
{
	struct StencilOpDesc
	{
		MFStencilOp stencilFailOp = MFStencilOp.Keep;
		MFStencilOp stencilDepthFailOp = MFStencilOp.Keep;
		MFStencilOp stencilPassOp = MFStencilOp.Keep;
		MFComparisonFunc stencilFunc = MFComparisonFunc.Always;
	}

	bool bDepthEnable = true;
	MFComparisonFunc depthFunc = MFComparisonFunc.Less;
	MFDepthWriteMask depthWriteMask = MFDepthWriteMask.All;
	bool bStencilEnable = false;
	ubyte stencilReadMask = 0xFF;
	ubyte stencilWriteMask = 0xFF;
	StencilOpDesc frontFace;
	StencilOpDesc backFace = StencilOpDesc(MFStencilOp.Keep, MFStencilOp.Keep, MFStencilOp.Keep, MFComparisonFunc.Never);
}

struct MFRasteriserState;

struct MFRasteriserStateDesc
{
	MFFillMode fillMode = MFFillMode.Solid;
	MFCullMode cullMode = MFCullMode.CCW;
	int depthBias = 0;
	float depthBiasClamp = 0;
	float slopeScaledDepthBias = 0;
	bool bDepthClipEnable = true;
	bool bScissorEnable = false;
	bool bMultisampleEnable = false;
	bool bAntialiasedLineEnable = false;
}

struct MFStateBlock;

extern (C) MFBlendState* MFBlendState_Create(MFBlendStateDesc *pDesc);
extern (C) int MFBlendState_Release(MFBlendState *pBlendState);

extern (C) MFSamplerState* MFSamplerState_Create(MFSamplerStateDesc *pDesc);
extern (C) int MFSamplerState_Release(MFSamplerState *pSamplerState);

extern (C) MFDepthStencilState* MFDepthStencilState_Create(MFDepthStencilStateDesc *pDesc);
extern (C) int MFDepthStencilState_Release(MFDepthStencilState *pDepthStencilState);

extern (C) MFRasteriserState* MFRasteriserState_Create(MFRasteriserStateDesc *pDesc);
extern (C) int MFRasteriserState_Release(MFRasteriserState *pRasteriserState);

extern (C) MFStateBlock* MFStateBlock_Create(uint size);
extern (C) MFStateBlock* MFStateBlock_CreateTemporary(uint size);
extern (C) void MFStateBlock_Destroy(MFStateBlock *pStateBlock);
extern (C) MFStateBlock* MFStateBlock_Clone(MFStateBlock *pSource);
extern (C) void MFStateBlock_Copy(MFStateBlock *pSource, MFStateBlock *pDest);
extern (C) MFStateBlock* MFStateBlock_Merge(MFStateBlock *pSource1, MFStateBlock *pSource2);
extern (C) void MFStateBlock_Clear(MFStateBlock *pStateBlock);

extern (C) size_t MFStateBlock_GetAllocatedBytes(MFStateBlock *pStateBlock);
extern (C) size_t MFStateBlock_GetUsedBytes(MFStateBlock *pStateBlock);
extern (C) size_t MFStateBlock_GetFreeBytes(MFStateBlock *pStateBlock);

extern (C) const char* MFStateBlock_GetStateName(MFStateBlockConstantType ct, int constant);

extern (C) bool MFStateBlock_SetBool(MFStateBlock *pStateBlock, MFStateConstant_Bool constant, bool state);
extern (C) bool MFStateBlock_SetVector(MFStateBlock *pStateBlock, MFStateConstant_Vector constant, ref const MFVector state);
extern (C) bool MFStateBlock_SetMatrix(MFStateBlock *pStateBlock, MFStateConstant_Matrix constant, ref const MFMatrix state);
extern (C) bool MFStateBlock_SetTexture(MFStateBlock *pStateBlock, MFStateConstant_Texture constant, MFTexture *pTexture);
extern (C) bool MFStateBlock_SetRenderState(MFStateBlock *pStateBlock, MFStateConstant_RenderState renderState, void *pState);
extern (C) bool MFStateBlock_SetMiscState(MFStateBlock *pStateBlock, MFStateConstant_Miscellaneous miscState, const void *pStateData, size_t dataSize);

bool MFStateBlock_SetAnimMatrices(MFStateBlock *pStateBlock, ref const MFStateConstant_AnimationMatrices matrices)
{
	return MFStateBlock_SetMiscState(pStateBlock, MFStateConstant_Miscellaneous.AnimationMatrices, &matrices, matrices.sizeof);
}

bool MFStateBlock_SetMatrixBatch(MFStateBlock *pStateBlock, ref const MFStateConstant_MatrixBatch batch)
{
	return MFStateBlock_SetMiscState(pStateBlock, MFStateConstant_Miscellaneous.MatrixBatch, &batch, batch.sizeof);
}

//extern (C) void MFStateBlock_SetLight(MFStateBlock *pStateBlock, MFStateConstant_Miscellaneous light, const MFLight *pLight);

extern (C) bool MFStateBlock_GetBool(MFStateBlock *pStateBlock, MFStateConstant_Bool constant, bool *pState);
extern (C) bool MFStateBlock_GetVector(MFStateBlock *pStateBlock, MFStateConstant_Vector constant, MFVector *pState);
extern (C) bool MFStateBlock_GetMatrix(MFStateBlock *pStateBlock, MFStateConstant_Matrix constant, MFMatrix *pState);
extern (C) bool MFStateBlock_GetTexture(MFStateBlock *pStateBlock, MFStateConstant_Texture constant, MFTexture **ppTexture);
extern (C) bool MFStateBlock_GetRenderState(MFStateBlock *pStateBlock, MFStateConstant_RenderState renderState, void **ppState);
extern (C) bool MFStateBlock_GetMiscState(MFStateBlock *pStateBlock, MFStateConstant_Miscellaneous miscState, void **ppStateData);
//extern (C) void MFStateBlock_GetLight(MFStateBlock *pStateBlock, MFStateConstant_Miscellaneous light, MFLight **ppLight);

//extern (C) void MFStateBlock_GetLightCounts(MFStateBlock *pStateBlock, int *pOmniLightCount, int *pSpotLightCount, int *pDirectionalLightCount);

extern (C) void MFStateBlock_ClearBool(MFStateBlock *pStateBlock, MFStateConstant_Bool constant);
extern (C) void MFStateBlock_ClearVector(MFStateBlock *pStateBlock, MFStateConstant_Vector constant);
extern (C) void MFStateBlock_ClearMatrix(MFStateBlock *pStateBlock, MFStateConstant_Matrix constant);
extern (C) void MFStateBlock_ClearTexture(MFStateBlock *pStateBlock, MFStateConstant_Texture constant);
extern (C) void MFStateBlock_ClearRenderState(MFStateBlock *pStateBlock, MFStateConstant_RenderState renderState);
extern (C) void MFStateBlock_ClearMiscState(MFStateBlock *pStateBlock, MFStateConstant_Miscellaneous miscState);
