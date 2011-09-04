#if !defined(_HKINPUTSOURCE_H)
#define _HKINPUTSOURCE_H

#include "MFInput.h"
#include "HKEvent.h"

struct HKInputSource
{
	int sourceID;

	MFInputDevice device;
	int deviceID;
};

class HKInputManager
{
public:
	enum EventType
	{
		// screen/mouse
		IE_Down,
		IE_Up,
		IE_Tap,
		IE_Hover,
		IE_Drag,
		IE_Pinch,
		IE_Spin,

		// buttons and keys
		IE_ButtonTriggered,
		IE_ButtonDown,
		IE_ButtonUp,
		IE_Wheel,
	};

	struct EventInfo
	{
		EventType ev;			// event

		HKInputSource *pSource;

		int contact;			// for multiple-contact devices (multiple mice/multi-touch screens)
		int buttonID;			// button ID (always 0 for touch screens)

		union
		{
			struct
			{
				float x, y;
				float deltaX, deltaY;
			} hover;
			struct
			{
				float x, y;
				float holdLength;
			} tap;
			struct
			{
				float x, y;
				float deltaX, deltaY;
				float startX, startY;
			} drag;
			struct
			{
				float x, y;
			} down;
			struct
			{
				float x, y;
				float downX, downY;
				float holdLength;
			} up;
			struct
			{
				float centerX, centerY;
				float deltaScale;
				int contact2;
			} pinch;
			struct
			{
				float centerX, centerY;
				float deltaAngle;
				int contact2;
			} spin;
		};
	};

	typedef HKEvent2<HKInputManager &, EventInfo &> InputEvent;

	HKInputManager();
	~HKInputManager();

	void Update();

	float SetDragThreshold(float threshold);

	InputEvent OnInputEvent;

protected:
	struct Contact
	{
		void Init(HKInputSource *pSource, int buttonID = -1, float x = 0.f, float y = 0.f);

		HKInputSource *pSource;
		int buttonID;

		float x, y;			// current position of contact
		float downX, downY;	// position each button was pressed down
		float downTime;		// length of down time for each button
		bool bState;		// bits represent the button pressed state
		bool bDrag;			// bits represent weather the interaction is a tap or a drag
	};

	static const int MaxSources = 64;
	static const int MaxContacts = 16;

	HKInputSource sources[MaxSources];
	int numDevices;

	Contact contacts[MaxContacts];
	bool bCurrentContacts[MaxContacts];
	int mouseContacts[MaxContacts];
	int mouseButtonContacts[MaxContacts][Mouse_MaxButtons];

	int touchContacts[MaxContacts];

	float dragThreshold;

	void ScanForDevices();
	MFVector CorrectPosition(float x, float y);
	void InitEvent(EventInfo &info, EventType ev, int contact);
};

#endif
