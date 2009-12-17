#if !defined(_VIDEO_H)
#define _VIDEO_H

class Video
{
public:
	void Update();
	void Draw();

	void Release();

	void SetDrawingRect(const MFVector& topLeft, const MFVector& bottomRight); // set the drawing rect in ortho space

	float GetTotalTime(); // return total playback time in seconds
	float GetTimeRemaining();
	float GetCurrentTime();

	void SetTime(float time); // time in seconds
	void SetPosition(float position) // 0.0 - 1.0 (start - finish)

	void Play(); // continues playback (from current offset)
	void Stop(); // freezes stream (dows not return to start)

	void Reset(); // reset to start

	MFVector drawTopLeft, drawBrottomRight;
};

Video* Video_CreateStreaming(const char *pFilename);

#endif
