#if !defined(_SCENEGRAPH_H)
#define _SCENEGRAPH_H

#include "PtrList.h"
#include "Matrix.h"
#include "BoundingVolume.h"

enum GraphNodeFlags
{
	GN_Enabled	= (1<<0),

	GN_ForceInt	= 0x7FFFFFFF
};

enum GraphObjectFlags
{
	GO_Enabled	= (1<<0),

	GO_ForceInt	= 0x7FFFFFFF
};

enum GraphObjectType
{

	GOT_Model			= (1<<0),
	GOT_Sprite			= (1<<1),
	GOT_ParticleSystem	= (1<<2),
	GOT_Collision		= (1<<3),

	GOT_Custom			= (1<<16),

	GOT_ForceInt		= 0x7FFFFFFF
};

class GraphNode;
class GraphObject;

class CommonData
{
public:
	Matrix localTransform;
	BoundingVolume boundingVolume; // points to bounding volume in the descriptor
	uint32 flags;

	GraphNode *pPropNode;
	GraphNode *pColNode;
	GraphNode *pVisNode;
};

class GraphNode
{
public:
	void Update(const Vector4& updateRange);
	void Draw(const Frustum& frustum);

	GraphNode *pParent;
	GraphNode *pNextSibling;
	GraphNode *pFirstChild;

	uint32 flags;
	CommonData *pCommonData; // can be NULL

	uint32 type;
	GraphObject *pData; // can be NULL (simple list node)
};

class GraphObject
{
public:
	virtual void Update();
	virtual void Draw();
};

class SceneGraph
{
public:
	void Init(int maxNodes);
	void Deinit();

protected:
	GraphNode *pRootNode;

private:
	PtrListDL<GraphNode> dynamicNodeList;
};

extern PtrList<GraphObject> gObjectSoup;

void SceneGraph_InitModule();
void SceneGraph_DeinitModule();

#endif
