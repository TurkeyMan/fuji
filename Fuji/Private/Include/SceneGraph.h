#if !defined(_SCENEGRAPH_H)
#define _SCENEGRAPH_H

#include "Matrix.h"

class GraphNode
{
public:
	inline void AddChildNode(GraphNode *pNode) { pNode->pNext = pChildren; pChildren = pNode; }
	inline void RemoveChildNode(GraphNode *pNode)
	{
		if(pNode == pChildren) pChildren = pChildren->pNext;

		GraphNode *pT;
		for(pT = pChildren; pT && pT->pNext != pNode; pT = pT->pNext) {}

		if(pT) pT->pNext = pT->pNext->pNext;
	}

	inline void Enable(bool enable = true) { enabled = enable; }
	inline void Disable() { enabled = false; }

	uint32 nodeType;
	uint32 flags;

	bool enabled;

	GraphNode *pParent;
	GraphNode *pChildren;
	GraphNode *pNext;

	Matrix localMatrix;
};

class RenderableNode : public GraphNode
{
public:
	virtual void Draw() = 0;

	inline void Show(bool show = true) { visible = show; }
	inline void Hide() { visible = false; }

	bool visible;
};

class SceneGraph : public RenderableNode
{
public:
	virtual void Draw();
};

extern SceneGraph gSceneGraph;

void SceneGraph_InitModule();
void SceneGraph_DeinitModule();

#endif
