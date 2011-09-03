#if !defined(_HKEVENT_H)
#define _HKEVENT_H

#pragma warning(disable:4324)

#include "fastdelegate/FastDelegate.h"

struct HKEventInfo
{
	void *pSender;
	void *pUserData;
};

typedef fastdelegate::FastDelegate2<void *, HKEventInfo *> HKEventDelegate;

class HKEventBase
{
public:
	HKEventBase();
	~HKEventBase();

	bool IsEmpty() { return numSubscribers == 0; }

	int GetNumSubscribers() { return numSubscribers; }

protected:
	void Add(fastdelegate::DelegateMemento memento);
	void Remove(fastdelegate::DelegateMemento memento);

	fastdelegate::DelegateMemento *pDelegates;
	uint16 numSubscribers;
	uint16 numAllocated;
};


// **** Default HKEvent using HKEventDelegate ****

class HKEvent : public HKEventBase
{
public:
	// C# style operators
	void operator () (void *pSender, HKEventInfo *pInfo)
	{
		for(int i=0; i<numSubscribers; ++i)
		{
			HKEventDelegate d; d.SetMemento(pDelegates[i]);
			d(pSender, pInfo);
		}
	}

	void operator += (HKEventDelegate delegateFunction) { AddDelegate(delegateFunction); }
	void operator -= (HKEventDelegate delegateFunction) { RemoveDelegate(delegateFunction); }

	void AddDelegate(HKEventDelegate delegateFunction) { Add(delegateFunction.GetMemento()); }
	void RemoveDelegate(HKEventDelegate delegateFunction) { Remove(delegateFunction.GetMemento()); }

	HKEventDelegate GetSubscriber(int index) { HKEventDelegate d; d.SetMemento(pDelegates[index]); return d; }
};


// **** Template definitions for various arg lengths ****

class HKEvent0 : public HKEventBase
{
public:
	typedef fastdelegate::FastDelegate0<> DelegateType;

	// C# style operators
	void operator () ()
	{
		for(int i=0; i<numSubscribers; ++i)
		{
			DelegateType d; d.SetMemento(pDelegates[i]);
			d();
		}
	}

	void operator += (DelegateType delegateFunction) { AddDelegate(delegateFunction); }
	void operator -= (DelegateType delegateFunction) { RemoveDelegate(delegateFunction); }

	void AddDelegate(DelegateType delegateFunction) { Add(delegateFunction.GetMemento()); }
	void RemoveDelegate(DelegateType delegateFunction) { Remove(delegateFunction.GetMemento()); }

	DelegateType GetSubscriber(int index) { DelegateType d; d.SetMemento(pDelegates[index]); return d; }
};

template<class P1>
class HKEvent1 : public HKEventBase
{
public:
	typedef fastdelegate::FastDelegate1<P1> DelegateType;

	// C# style operators
	void operator () (P1 p1)
	{
		for(int i=0; i<numSubscribers; ++i)
		{
			DelegateType d; d.SetMemento(pDelegates[i]);
			d(p1);
		}
	}

	void operator += (DelegateType delegateFunction) { AddDelegate(delegateFunction); }
	void operator -= (DelegateType delegateFunction) { RemoveDelegate(delegateFunction); }

	void AddDelegate(DelegateType delegateFunction) { Add(delegateFunction.GetMemento()); }
	void RemoveDelegate(DelegateType delegateFunction) { Remove(delegateFunction.GetMemento()); }

	DelegateType GetSubscriber(int index) { DelegateType d; d.SetMemento(pDelegates[index]); return d; }
};

template<class P1, class P2>
class HKEvent2 : public HKEventBase
{
public:
	typedef fastdelegate::FastDelegate2<P1, P2> DelegateType;

	// C# style operators
	void operator () (P1 p1, P2 p2)
	{
		for(int i=0; i<numSubscribers; ++i)
		{
			DelegateType d; d.SetMemento(pDelegates[i]);
			d(p1, p2);
		}
	}

	void operator += (DelegateType delegateFunction) { AddDelegate(delegateFunction); }
	void operator -= (DelegateType delegateFunction) { RemoveDelegate(delegateFunction); }

	void AddDelegate(DelegateType delegateFunction) { Add(delegateFunction.GetMemento()); }
	void RemoveDelegate(DelegateType delegateFunction) { Remove(delegateFunction.GetMemento()); }

	DelegateType GetSubscriber(int index) { DelegateType d; d.SetMemento(pDelegates[index]); return d; }
};

template<class P1, class P2, class P3>
class HKEvent3 : public HKEventBase
{
public:
	typedef fastdelegate::FastDelegate3<P1, P2, P3> DelegateType;

	// C# style operators
	void operator () (P1 p1, P2 p2, P3 p3)
	{
		for(int i=0; i<numSubscribers; ++i)
		{
			DelegateType d; d.SetMemento(pDelegates[i]);
			d(p1, p2, p3);
		}
	}

	void operator += (DelegateType delegateFunction) { AddDelegate(delegateFunction); }
	void operator -= (DelegateType delegateFunction) { RemoveDelegate(delegateFunction); }

	void AddDelegate(DelegateType delegateFunction) { Add(delegateFunction.GetMemento()); }
	void RemoveDelegate(DelegateType delegateFunction) { Remove(delegateFunction.GetMemento()); }

	DelegateType GetSubscriber(int index) { DelegateType d; d.SetMemento(pDelegates[index]); return d; }
};

template<class P1, class P2, class P3, class P4>
class HKEvent4 : public HKEventBase
{
public:
	typedef fastdelegate::FastDelegate4<P1, P2, P3, P4> DelegateType;

	// C# style operators
	void operator () (P1 p1, P2 p2, P3 p3, P4 p4)
	{
		for(int i=0; i<numSubscribers; ++i)
		{
			DelegateType d; d.SetMemento(pDelegates[i]);
			d(p1, p2, p3, p4);
		}
	}

	void operator += (DelegateType delegateFunction) { AddDelegate(delegateFunction); }
	void operator -= (DelegateType delegateFunction) { RemoveDelegate(delegateFunction); }

	void AddDelegate(DelegateType delegateFunction) { Add(delegateFunction.GetMemento()); }
	void RemoveDelegate(DelegateType delegateFunction) { Remove(delegateFunction.GetMemento()); }

	DelegateType GetSubscriber(int index) { DelegateType d; d.SetMemento(pDelegates[index]); return d; }
};

template<class P1, class P2, class P3, class P4, class P5>
class HKEvent5 : public HKEventBase
{
public:
	typedef fastdelegate::FastDelegate5<P1, P2, P3, P4, P5> DelegateType;

	// C# style operators
	void operator () (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
	{
		for(int i=0; i<numSubscribers; ++i)
		{
			DelegateType d; d.SetMemento(pDelegates[i]);
			d(p1, p2, p3, p4, p5);
		}
	}

	void operator += (DelegateType delegateFunction) { AddDelegate(delegateFunction); }
	void operator -= (DelegateType delegateFunction) { RemoveDelegate(delegateFunction); }

	void AddDelegate(DelegateType delegateFunction) { Add(delegateFunction.GetMemento()); }
	void RemoveDelegate(DelegateType delegateFunction) { Remove(delegateFunction.GetMemento()); }

	DelegateType GetSubscriber(int index) { DelegateType d; d.SetMemento(pDelegates[index]); return d; }
};

template<class P1, class P2, class P3, class P4, class P5, class P6>
class HKEvent6 : public HKEventBase
{
public:
	typedef fastdelegate::FastDelegate6<P1, P2, P3, P4, P5, P6> DelegateType;

	// C# style operators
	void operator () (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
	{
		for(int i=0; i<numSubscribers; ++i)
		{
			DelegateType d; d.SetMemento(pDelegates[i]);
			d(p1, p2, p3, p4, p5, p6);
		}
	}

	void operator += (DelegateType delegateFunction) { AddDelegate(delegateFunction); }
	void operator -= (DelegateType delegateFunction) { RemoveDelegate(delegateFunction); }

	void AddDelegate(DelegateType delegateFunction) { Add(delegateFunction.GetMemento()); }
	void RemoveDelegate(DelegateType delegateFunction) { Remove(delegateFunction.GetMemento()); }

	DelegateType GetSubscriber(int index) { DelegateType d; d.SetMemento(pDelegates[index]); return d; }
};

template<class P1, class P2, class P3, class P4, class P5, class P6, class P7>
class HKEvent7 : public HKEventBase
{
public:
	typedef fastdelegate::FastDelegate7<P1, P2, P3, P4, P5, P6, P7> DelegateType;

	// C# style operators
	void operator () (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
	{
		for(int i=0; i<numSubscribers; ++i)
		{
			DelegateType d; d.SetMemento(pDelegates[i]);
			d(p1, p2, p3, p4, p5, p6, p7);
		}
	}

	void operator += (DelegateType delegateFunction) { AddDelegate(delegateFunction); }
	void operator -= (DelegateType delegateFunction) { RemoveDelegate(delegateFunction); }

	void AddDelegate(DelegateType delegateFunction) { Add(delegateFunction.GetMemento()); }
	void RemoveDelegate(DelegateType delegateFunction) { Remove(delegateFunction.GetMemento()); }

	DelegateType GetSubscriber(int index) { DelegateType d; d.SetMemento(pDelegates[index]); return d; }
};

template<class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8>
class HKEvent8 : public HKEventBase
{
public:
	typedef fastdelegate::FastDelegate8<P1, P2, P3, P4, P5, P6, P7, P8> DelegateType;

	// C# style operators
	void operator () (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8)
	{
		for(int i=0; i<numSubscribers; ++i)
		{
			DelegateType d; d.SetMemento(pDelegates[i]);
			d(p1, p2, p3, p4, p5, p6, p7, p8);
		}
	}

	void operator += (DelegateType delegateFunction) { AddDelegate(delegateFunction); }
	void operator -= (DelegateType delegateFunction) { RemoveDelegate(delegateFunction); }

	void AddDelegate(DelegateType delegateFunction) { Add(delegateFunction.GetMemento()); }
	void RemoveDelegate(DelegateType delegateFunction) { Remove(delegateFunction.GetMemento()); }

	DelegateType GetSubscriber(int index) { DelegateType d; d.SetMemento(pDelegates[index]); return d; }
};

#endif
