#pragma once
#if !defined(_HKEVENT_H)
#define _HKEVENT_H

#if defined(MF_COMPILER_VISUALC)
	#pragma warning(disable:4324)
#endif

#include "Middleware/FastDelegate.h"

struct HKEventInfo
{
	void *pSender;
	void *pUserData;
};

typedef fastdelegate::FastDelegate2<void *, const HKEventInfo *> HKEventDelegate;

class HKEventBase
{
public:
	HKEventBase();
	~HKEventBase();

	inline bool IsEmpty() { return numSubscribers == 0; }

	inline int GetNumSubscribers() { return numSubscribers; }

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
	inline void operator () (void *pSender, const HKEventInfo *pInfo)
	{
		for(int i=0; i<numSubscribers; ++i)
		{
			HKEventDelegate d; d.SetMemento(pDelegates[i]);
			d(pSender, pInfo);
		}
	}

	inline void operator += (HKEventDelegate delegateFunction) { AddDelegate(delegateFunction); }
	inline void operator -= (HKEventDelegate delegateFunction) { RemoveDelegate(delegateFunction); }

	inline void AddDelegate(HKEventDelegate delegateFunction) { Add(delegateFunction.GetMemento()); }
	inline void RemoveDelegate(HKEventDelegate delegateFunction) { Remove(delegateFunction.GetMemento()); }

	inline HKEventDelegate GetSubscriber(int index) { HKEventDelegate d; d.SetMemento(pDelegates[index]); return d; }
	inline HKEventDelegate GetDelegate() { return fastdelegate::MakeDelegate(this, &HKEvent::operator()); }
};


// **** Template definitions for various arg lengths ****

class HKEvent0 : public HKEventBase
{
public:
	typedef fastdelegate::FastDelegate0<> Delegate;

	// C# style operators
	inline void operator () ()
	{
		for(int i=0; i<numSubscribers; ++i)
		{
			Delegate d; d.SetMemento(pDelegates[i]);
			d();
		}
	}

	inline void operator += (Delegate delegateFunction) { AddDelegate(delegateFunction); }
	inline void operator -= (Delegate delegateFunction) { RemoveDelegate(delegateFunction); }

	inline void AddDelegate(Delegate delegateFunction) { Add(delegateFunction.GetMemento()); }
	inline void RemoveDelegate(Delegate delegateFunction) { Remove(delegateFunction.GetMemento()); }

	inline Delegate GetSubscriber(int index) { Delegate d; d.SetMemento(pDelegates[index]); return d; }
	inline Delegate GetDelegate() { return fastdelegate::MakeDelegate(this, &HKEvent0::operator()); }
};

template<class P1>
class HKEvent1 : public HKEventBase
{
public:
	typedef fastdelegate::FastDelegate1<P1> Delegate;

	// C# style operators
	inline void operator () (P1 p1)
	{
		for(int i=0; i<numSubscribers; ++i)
		{
			Delegate d; d.SetMemento(pDelegates[i]);
			d(p1);
		}
	}

	inline void operator += (Delegate delegateFunction) { AddDelegate(delegateFunction); }
	inline void operator -= (Delegate delegateFunction) { RemoveDelegate(delegateFunction); }

	inline void AddDelegate(Delegate delegateFunction) { Add(delegateFunction.GetMemento()); }
	inline void RemoveDelegate(Delegate delegateFunction) { Remove(delegateFunction.GetMemento()); }

	inline Delegate GetSubscriber(int index) { Delegate d; d.SetMemento(pDelegates[index]); return d; }
	inline Delegate GetDelegate() { return fastdelegate::MakeDelegate(this, &HKEvent1::operator()); }
};

template<class P1, class P2>
class HKEvent2 : public HKEventBase
{
public:
	typedef fastdelegate::FastDelegate2<P1, P2> Delegate;

	// C# style operators
	inline void operator () (P1 p1, P2 p2)
	{
		for(int i=0; i<numSubscribers; ++i)
		{
			Delegate d; d.SetMemento(pDelegates[i]);
			d(p1, p2);
		}
	}

	inline void operator += (Delegate delegateFunction) { AddDelegate(delegateFunction); }
	inline void operator -= (Delegate delegateFunction) { RemoveDelegate(delegateFunction); }

	inline void AddDelegate(Delegate delegateFunction) { Add(delegateFunction.GetMemento()); }
	inline void RemoveDelegate(Delegate delegateFunction) { Remove(delegateFunction.GetMemento()); }

	inline Delegate GetSubscriber(int index) { Delegate d; d.SetMemento(pDelegates[index]); return d; }
	inline Delegate GetDelegate() { return fastdelegate::MakeDelegate(this, &HKEvent2::operator()); }
};

template<class P1, class P2, class P3>
class HKEvent3 : public HKEventBase
{
public:
	typedef fastdelegate::FastDelegate3<P1, P2, P3> Delegate;

	// C# style operators
	inline void operator () (P1 p1, P2 p2, P3 p3)
	{
		for(int i=0; i<numSubscribers; ++i)
		{
			Delegate d; d.SetMemento(pDelegates[i]);
			d(p1, p2, p3);
		}
	}

	inline void operator += (Delegate delegateFunction) { AddDelegate(delegateFunction); }
	inline void operator -= (Delegate delegateFunction) { RemoveDelegate(delegateFunction); }

	inline void AddDelegate(Delegate delegateFunction) { Add(delegateFunction.GetMemento()); }
	inline void RemoveDelegate(Delegate delegateFunction) { Remove(delegateFunction.GetMemento()); }

	inline Delegate GetSubscriber(int index) { Delegate d; d.SetMemento(pDelegates[index]); return d; }
	inline Delegate GetDelegate() { return fastdelegate::MakeDelegate(this, &HKEvent3::operator()); }
};

template<class P1, class P2, class P3, class P4>
class HKEvent4 : public HKEventBase
{
public:
	typedef fastdelegate::FastDelegate4<P1, P2, P3, P4> Delegate;

	// C# style operators
	inline void operator () (P1 p1, P2 p2, P3 p3, P4 p4)
	{
		for(int i=0; i<numSubscribers; ++i)
		{
			Delegate d; d.SetMemento(pDelegates[i]);
			d(p1, p2, p3, p4);
		}
	}

	inline void operator += (Delegate delegateFunction) { AddDelegate(delegateFunction); }
	inline void operator -= (Delegate delegateFunction) { RemoveDelegate(delegateFunction); }

	inline void AddDelegate(Delegate delegateFunction) { Add(delegateFunction.GetMemento()); }
	inline void RemoveDelegate(Delegate delegateFunction) { Remove(delegateFunction.GetMemento()); }

	inline Delegate GetSubscriber(int index) { Delegate d; d.SetMemento(pDelegates[index]); return d; }
	inline Delegate GetDelegate() { return fastdelegate::MakeDelegate(this, &HKEvent4::operator()); }
};

template<class P1, class P2, class P3, class P4, class P5>
class HKEvent5 : public HKEventBase
{
public:
	typedef fastdelegate::FastDelegate5<P1, P2, P3, P4, P5> Delegate;

	// C# style operators
	inline void operator () (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
	{
		for(int i=0; i<numSubscribers; ++i)
		{
			Delegate d; d.SetMemento(pDelegates[i]);
			d(p1, p2, p3, p4, p5);
		}
	}

	inline void operator += (Delegate delegateFunction) { AddDelegate(delegateFunction); }
	inline void operator -= (Delegate delegateFunction) { RemoveDelegate(delegateFunction); }

	inline void AddDelegate(Delegate delegateFunction) { Add(delegateFunction.GetMemento()); }
	inline void RemoveDelegate(Delegate delegateFunction) { Remove(delegateFunction.GetMemento()); }

	inline Delegate GetSubscriber(int index) { Delegate d; d.SetMemento(pDelegates[index]); return d; }
	inline Delegate GetDelegate() { return fastdelegate::MakeDelegate(this, &HKEvent5::operator()); }
};

template<class P1, class P2, class P3, class P4, class P5, class P6>
class HKEvent6 : public HKEventBase
{
public:
	typedef fastdelegate::FastDelegate6<P1, P2, P3, P4, P5, P6> Delegate;

	// C# style operators
	inline void operator () (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
	{
		for(int i=0; i<numSubscribers; ++i)
		{
			Delegate d; d.SetMemento(pDelegates[i]);
			d(p1, p2, p3, p4, p5, p6);
		}
	}

	inline void operator += (Delegate delegateFunction) { AddDelegate(delegateFunction); }
	inline void operator -= (Delegate delegateFunction) { RemoveDelegate(delegateFunction); }

	inline void AddDelegate(Delegate delegateFunction) { Add(delegateFunction.GetMemento()); }
	inline void RemoveDelegate(Delegate delegateFunction) { Remove(delegateFunction.GetMemento()); }

	inline Delegate GetSubscriber(int index) { Delegate d; d.SetMemento(pDelegates[index]); return d; }
	inline Delegate GetDelegate() { return fastdelegate::MakeDelegate(this, &HKEvent6::operator()); }
};

template<class P1, class P2, class P3, class P4, class P5, class P6, class P7>
class HKEvent7 : public HKEventBase
{
public:
	typedef fastdelegate::FastDelegate7<P1, P2, P3, P4, P5, P6, P7> Delegate;

	// C# style operators
	inline void operator () (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
	{
		for(int i=0; i<numSubscribers; ++i)
		{
			Delegate d; d.SetMemento(pDelegates[i]);
			d(p1, p2, p3, p4, p5, p6, p7);
		}
	}

	inline void operator += (Delegate delegateFunction) { AddDelegate(delegateFunction); }
	inline void operator -= (Delegate delegateFunction) { RemoveDelegate(delegateFunction); }

	inline void AddDelegate(Delegate delegateFunction) { Add(delegateFunction.GetMemento()); }
	inline void RemoveDelegate(Delegate delegateFunction) { Remove(delegateFunction.GetMemento()); }

	inline Delegate GetSubscriber(int index) { Delegate d; d.SetMemento(pDelegates[index]); return d; }
	inline Delegate GetDelegate() { return fastdelegate::MakeDelegate(this, &HKEvent7::operator()); }
};

template<class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8>
class HKEvent8 : public HKEventBase
{
public:
	typedef fastdelegate::FastDelegate8<P1, P2, P3, P4, P5, P6, P7, P8> Delegate;

	// C# style operators
	inline void operator () (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8)
	{
		for(int i=0; i<numSubscribers; ++i)
		{
			Delegate d; d.SetMemento(pDelegates[i]);
			d(p1, p2, p3, p4, p5, p6, p7, p8);
		}
	}

	inline void operator += (Delegate delegateFunction) { AddDelegate(delegateFunction); }
	inline void operator -= (Delegate delegateFunction) { RemoveDelegate(delegateFunction); }

	inline void AddDelegate(Delegate delegateFunction) { Add(delegateFunction.GetMemento()); }
	inline void RemoveDelegate(Delegate delegateFunction) { Remove(delegateFunction.GetMemento()); }

	inline Delegate GetSubscriber(int index) { Delegate d; d.SetMemento(pDelegates[index]); return d; }
	inline Delegate GetDelegate() { return fastdelegate::MakeDelegate(this, &HKEvent8::operator()); }
};

#endif
