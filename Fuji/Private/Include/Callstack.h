#if !defined(_CALLSTACK_H)
#define _CALLSTACK_H

#include <map>
#include <vector>
#include <string>

void Callstack_InitModule();
void Callstack_DeinitModule();

#if !defined(_RETAIL)

#if defined(_CALLSTACK_MONITORING)

	class CallProfile;

	class MonitorCall
	{
	public:
		MonitorCall(bool enter, CallProfile *ptr, uint64 time)
		: start(enter), timestamp(time) { profilePtr = ptr; }

		bool start;
		CallProfile *profilePtr;
		uint64 timestamp;
	};

	struct MonitorInfo
	{
		MonitorInfo() { frameStart = frameFinish = frameEnd = overheadTime = 0; frames = 1; }

		int64 frameStart;
		int64 frameFinish;
		int64 frameEnd;
		int64 overheadTime;
		std::vector<MonitorCall> calls;
		int frames;
		double divide;
	};

#endif

	class FunctionCall
	{
	public:
		FunctionCall(char *name, int profile);
		virtual ~FunctionCall();
#if defined(_CALLSTACK_PROFILING)
		bool profiling;
#endif
	};

#if defined(_CALLSTACK_PROFILING)
	class CallProfile
	{
	public:
		CallProfile();

		int calls;

		int64 thisCall;
		int64 total;

#if defined(_CALLSTACK_MONITORING)
		uint32 colour;
#endif
	};
#endif

#if defined(_CALLSTACK_PROFILING)
	void Callstack_BeginFrame();

	#if defined(_CALLSTACK_MONITORING)
		void Callstack_EndFrame();
	#else
		#define Callstack_EndFrame()
	#endif

	extern std::map<std::string, CallProfile> FunctionRegistry;

	#define CALLSTACK(x) FunctionCall callstack(x, 0)
	#define CALLSTACKc(x) FunctionCall callstack(x, 1)

	void Callstack_DrawProfile();
#else
	#define Callstack_BeginFrame()
	#define CALLSTACK(x)
	#define CALLSTACKc(x)
	#define Callstack_DrawProfile()
	#define Callstack_EndFrame()
#endif

#else

	#define CALLSTACK(x)
	#define CALLSTACKc(x)
	#define Callstack_DrawProfile()
	#define Callstack_EndFrame()

#endif

#endif // _CALLSTACK_H
