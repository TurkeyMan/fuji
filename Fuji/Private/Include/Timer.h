#if !defined(_TIMER_H)
#define _TIMER_H

#define AVERAGE_SAMPLES 128

#define Timer_Paused	0x0001

#define UNFLAG(x, y) (x^=x&y)
#define FLAG(x, y) (x|=y)

void Timer_InitModule();
void Timer_DeinitModule(); 

uint64 GetHighResolutionTime();
uint64 GetHighResolutionFrequency();

class Timer
{
public:
	void Init();

	void Update();
	void Reset();

	void SetRate(double rate);
	double GetRate();

	double TimeDeltaD();
	float TimeDeltaF();

	float GetFPS();

	double GetSecondsD();
	float GetSecondsF();

	void Pause(bool pause);
	bool IsPaused();

	void SetFixed(int fps);
	void SetFree();

protected:
	uint64 accumulator, lastUpdate, thisCall, lastCall, freq;

	double FPS, rate;
	int current;

	float deltaF;
	double deltaD;

	double history[AVERAGE_SAMPLES];
	static int avgSamples;


	uint32 flags;

	bool fixed;
	int fixedFPS;
};

#endif //_TIMER_H
