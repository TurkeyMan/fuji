#if !defined(_SYNTH_H)
#define _SYNTH_H

#define SYNTH_SAMPLERATE 44100
#define SYNTH_SAMPLEPERIOD (1.0f/44100.0f)

// enums
enum WaveType
{
	WT_Sine,
	WT_CoSine,
	WT_Triangle,
	WT_Square,
	WT_UpSaw,
	WT_DownSaw,
};

// functions
float GetWave(WaveType waveType, float phase);
float GetNoteFrequency(float noteOffset, float baseFrequency = 261.62556530059863467f);

// classes
class Filter
{
public:
	virtual void Reset() = 0;
	virtual void Draw();

	virtual void GetSamples(float *pOutputBuffer, int numSamples);
	virtual void SetBufferSize(int numSamples);
};

class Parameter
{
public:
	Parameter() { time = 0; }

	virtual float GetValue() = 0;
	virtual void SetTime(uint32 _time) { time = _time; }

protected:
	uint32 time;
};

class ConstantParameter : public Parameter
{
public:
	ConstantParameter(float _value = 0.0f) { value = _value; }
	void SetValue(float _value) { value = _value; }
	virtual float GetValue() { return value; }

protected:
	float value;
};

class NoteFrequency : public Parameter
{
public:
	NoteFrequency(float _note = 0.0f) { note = _note; }
	virtual float GetValue() { return GetNoteFrequency(floorf((pNoteParameter ? pNoteParameter->GetValue() : note)+0.5f)); }
	virtual void SetTime(uint32 _time) { Parameter::SetTime(_time); if(pNoteParameter) { pNoteParameter->SetTime(_time); } }

	void SetNote(float _note) { note = _note; }
	void SetNote(Parameter *pParameter = NULL) { pNoteParameter = pParameter; }

protected:
	float note;
	Parameter *pNoteParameter;
};

class TranslateParameter: public Parameter
{
public:
	TranslateParameter(float _translation = 0.0f) { translation = _translation; }
	virtual float GetValue() { return (pParameter ? pParameter->GetValue() : 0.0f) + translation; }
	virtual void SetTime(uint32 _time) { Parameter::SetTime(_time); if(pParameter) { pParameter->SetTime(_time); } }

	void SetTranslation(float _translation) { translation = _translation; }
	void SetParameter(Parameter *pParameter = NULL) { pParameter = pParameter; }

protected:
	float translation;
	Parameter *pParameter;
};

class RealTimeParameter : public Parameter
{
public:
	RealTimeParameter() {}

	virtual float GetValue() { return gSystemTimer.GetSecondsF(); }
};

class TimeParameter : public Parameter
{
public:
	TimeParameter() {}

	virtual float GetValue() { return (float)time * SYNTH_SAMPLEPERIOD; }
};

#endif
