#if !defined(_PROP_H)
#define _PROP_H

#include "SceneGraph.h"

class PropDescriptor;

// this is a base Prop node which can be used in the update graph..
class Prop : public GraphObject
{
	PropDescriptor *pDescriptor;

private:
	CommonData *pCommonData;
};


#endif
