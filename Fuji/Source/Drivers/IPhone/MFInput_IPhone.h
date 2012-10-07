#if !defined(_MFINPUT_IPHONE_H)
#define _MFINPUT_IPHONE_H

enum ContactFlags
{
	CF_Tap = 1,
	CF_First = 2,
	CF_Warped = 4,
	CF_Delayed = 8,
	CF_Ended = 16
};

struct Contact
{
	int x, y;
	int tapCount;
	int phase;
	int flags;
};

extern "C"
{
	void MFInputIPhone_SetContacts(int numContacts, Contact *pContacts);
	void MFInputIPhone_SetAcceleration(float x, float y, float z);
	void MFInputIPhone_Shake();
}

#endif
