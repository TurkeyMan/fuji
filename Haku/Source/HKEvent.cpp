#include "Haku.h"
#include "HKEvent.h"

#include "MFHeap.h"

HKEventBase::HKEventBase()
{
	numSubscribers = numAllocated = 0;
	pDelegates = NULL;
}

HKEventBase::~HKEventBase()
{
	if(numAllocated)
		MFHeap_Free(pDelegates);
}

void HKEventBase::Add(fastdelegate::DelegateMemento memento)
{
	if(numAllocated <= numSubscribers)
	{
		int allocCount = numSubscribers == 0 ? 4 : numSubscribers * 2;
		pDelegates = (fastdelegate::DelegateMemento*)MFHeap_Realloc(pDelegates, sizeof(fastdelegate::DelegateMemento)*allocCount);
	}

	pDelegates[numSubscribers++] = memento;
}

void HKEventBase::Remove(fastdelegate::DelegateMemento memento)
{
	for(int a=0; a<numSubscribers; ++a)
	{
		if(pDelegates[a].IsEqual(memento))
		{
			--numSubscribers;
			for(; a<numSubscribers; ++a)
				pDelegates[a] = pDelegates[a+1];
			break;
		}
	}
}
