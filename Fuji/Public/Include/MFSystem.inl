
extern uint32 gFrameCount;
extern float gSystemTimeDelta;

inline float MFSystem_TimeDelta()
{
	return gSystemTimeDelta;
}

inline uint32 MFSystem_FrameCounter()
{
	return gFrameCount;
}
