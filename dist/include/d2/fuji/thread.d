module fuji.thread;

public import fuji.c.MFThread;
import fuji.string;
import fuji.heap;

import core.thread;

extern (C) void rt_moduleTlsCtor();
extern (C) void rt_moduleTlsDtor();


alias MFThreadEntryFunction = int function(void* pUserData);
alias MFThreadEntryDelegate = int delegate();

nothrow:

void MFThread_ExitThread(int exitCode)
{
	thread_detachThis();
	try
	{
		rt_moduleTlsDtor();
	}
	catch(Exception e)
	{
	}
	fuji.c.MFThread.MFThread_ExitThread(exitCode);
}

@nogc:

MFThread MFThread_CreateThread(const(char)[] name, MFThreadEntryFunction entryPoint, void* pUserData = null, int priority = MFThreadPriority.Normal, uint flags = 0, uint stackSize = 0)
{
	struct Func
	{
		MFThreadEntryFunction f;
		void* d;
	}

	static int EnterThread(void* pData) nothrow
	{
		Func f = *cast(Func*)pData;
		MFHeap_Free(pData);
		try
		{
			thread_attachThis();
			rt_moduleTlsCtor();
			return f.f(f.d);
		}
		catch(Exception e)
		{
			return -1;
		}
	}

	Func* f = MFHeap_Alloc!Func();
	f.f = entryPoint;
	f.d = pUserData;

	auto s = Stringz!32(name);
	return fuji.c.MFThread.MFThread_CreateThread(s, cast(MFThreadEntryPoint)&EnterThread, f, priority, flags, stackSize);
}

MFThread MFThread_CreateThread(const(char)[] name, MFThreadEntryDelegate entryPoint, int priority = MFThreadPriority.Normal, uint flags = 0, uint stackSize = 0)
{
	struct Func
	{
		MFThreadEntryDelegate d;
	}

	static int EnterThread(void* pData) nothrow
	{
		Func f = *cast(Func*)pData;
		MFHeap_Free(pData);
		try
		{
			thread_attachThis();
			rt_moduleTlsCtor();
			return f.d();
		}
		catch(Exception e)
		{
			return -1;
		}
	}

	Func* f = MFHeap_Alloc!Func();
	f.d = entryPoint;

	auto s = Stringz!32(name);
	return fuji.c.MFThread.MFThread_CreateThread(s, cast(MFThreadEntryPoint)&EnterThread, f, priority, flags, stackSize);
}

MFMutex MFThread_CreateMutex(const(char)[] name)
{
	auto s = Stringz!32(name);
	return fuji.c.MFThread.MFThread_CreateMutex(s);
}

MFSemaphore MFThread_CreateSemaphore(const(char)[] name, int maxCount, int startCount)
{
	auto s = Stringz!32(name);
	return fuji.c.MFThread.MFThread_CreateSemaphore(s, maxCount, startCount);
}
