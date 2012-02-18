#pragma once
#if !defined(_HKFACTORY_H)
#define _HKFACTORY_H

#include "fastdelegate/FastDelegate.h"

template <typename T>
class HKFactory
{
public:
	struct FactoryType;
	typedef fastdelegate::FastDelegate1<FactoryType *, T*> CreateFunc;

	struct FactoryType
	{
		char typeName[64];
		CreateFunc createFunc;
		FactoryType *pParent;
		FactoryType *pNext;
	};

	HKFactory() { pTypes = NULL; }

	FactoryType *RegisterType(const char *pTypeName, CreateFunc createDelegate, FactoryType *pParent = NULL)
	{
		FactoryType *pType = new FactoryType;
		MFString_Copy(pType->typeName, pTypeName);
		pType->createFunc = createDelegate;
		pType->pParent = pParent;
		pType->pNext = pTypes;
		pTypes = pType;
		return pType;
	}

	T *Create(const char *pTypeName, FactoryType **ppType = NULL)
	{
		for(FactoryType *pType = pTypes; pType; pType = pType->pNext)
		{
			if(!MFString_CaseCmp(pTypeName, pType->typeName))
			{
				if(ppType)
					*ppType = pType;
				return (T*)pType->createFunc(pType);
			}
		}

		MFDebug_Assert(2, MFStr("Unknown factory type: '%s'", pTypeName));

		return NULL;
	}

	FactoryType *FindType(const char *pTypeName)
	{
		for(FactoryType *pType = pTypes; pType; pType = pType->pNext)
		{
			if(!MFString_CaseCmp(pTypeName, pType->typeName))
				return pType;
		}
		return NULL;
	}

protected:
	FactoryType *pTypes;
};

#endif
