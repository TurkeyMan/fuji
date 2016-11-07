module fuji.system;

public import fuji.c.MFSystem;

alias SystemCallback = void delegate() nothrow;

SystemCallback registerSystemCallback(MFCallback callback, SystemCallback callbackFunc)
{
	void *pUserData;
	MFSystemCallbackFunction f = MFSystem_RegisterSystemCallback(callback, cast(MFSystemCallbackFunction)callbackFunc.funcptr, callbackFunc.ptr, &pUserData);
	SystemCallback cb;
	cb.funcptr = cast(typeof(cb.funcptr))f;
	cb.ptr = pUserData;
	return cb;
}

SystemCallback getSystemCallback(MFCallback callback)
{
	void *pUserData;
	MFSystemCallbackFunction f = MFSystem_GetSystemCallback(callback, &pUserData);
	SystemCallback cb;
	cb.funcptr = cast(typeof(cb.funcptr))f;
	cb.ptr = pUserData;
	return cb;
}
