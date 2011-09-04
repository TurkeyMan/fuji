#include "Haku.h"
#include "UI/HKInputSource.h"

#include "MFDisplay.h"
#include "MFInput.h"
#include "MFSystem.h"

HKInputManager::HKInputManager()
{
	MFZeroMemory(sources, sizeof(sources));
	MFZeroMemory(contacts, sizeof(contacts));
	MFZeroMemory(bCurrentContacts, sizeof(bCurrentContacts));

	for(int a=0; a<MaxSources; ++a)
		sources[a].sourceID = -1;

	numDevices = 0;

	for(int a=0; a<MaxContacts; ++a)
	{
		touchContacts[a] = -1;

		mouseContacts[a] = -1;
		for(int b=0; b<Mouse_MaxButtons; ++b)
			mouseButtonContacts[a][b] = -1;
	}

	dragThreshold = 16.f;

	ScanForDevices();
}

HKInputManager::~HKInputManager()
{
}

void HKInputManager::Update()
{
	// search for new events
	for(int a=0; a<MaxSources; ++a)
	{
		if(sources[a].sourceID == -1)
			continue;

		MFInputDevice device = sources[a].device;
		int deviceID = sources[a].deviceID;

		if(device == IDD_Mouse)
		{
			if(mouseContacts[deviceID] == -1 && MFInput_IsReady(device, deviceID))
			{
				for(int c=0; c<MaxContacts; ++c)
				{
					if(!bCurrentContacts[c])
					{
						bCurrentContacts[c] = true;
						mouseContacts[deviceID] = c;

						// create the hover contact
						MFVector pos = CorrectPosition(MFInput_Read(Mouse_XPos, device, deviceID), MFInput_Read(Mouse_YPos, device, deviceID));
						contacts[c].Init(&sources[a], -1, pos.x, pos.y);
						contacts[c].downX = contacts[c].downY = 0.f;

//						pNewContactCallback(c);
						break;
					}
				}
			}

			if(MFDisplay_HasFocus())
			{
				for(int b=0; b<Mouse_MaxButtons; ++b)
				{
					if(mouseButtonContacts[deviceID][b] == -1)
					{
						if(MFInput_Read(Mouse_LeftButton + b, device, deviceID))
						{
							for(int c=0; c<MaxContacts; ++c)
							{
								if(!bCurrentContacts[c])
								{
									bCurrentContacts[c] = true;
									mouseButtonContacts[deviceID][b] = c;

									contacts[c].Init(&sources[a], b + Mouse_LeftButton, contacts[mouseContacts[deviceID]].x, contacts[mouseContacts[deviceID]].y);
									contacts[c].bState = true;

//									pNewContactCallback(c);

									// send the down event
									EventInfo info;
									InitEvent(info, IE_Down, c);
									info.down.x = contacts[c].x;
									info.down.y = contacts[c].y;

									OnInputEvent(*this, info);
									break;
								}
							}
						}
					}
				}
/*
				float wheel = MFInput_Read(Mouse_Wheel, device, deviceID);
				if(wheel)
				{
					// we'll simulate a pinch event on the mouses hover contact
					EventInfo info;
					InitEvent(info, IE_Pinch, deviceID);
					info.pinch.centerX = contacts[a].x;
					info.pinch.centerY = contacts[a].y;
					info.pinch.deltaScale = wheel < 0.f ? 0.5f : 2.f;
					info.pinch.contact2 = -1;

					OnInputEvent(*this, info);
				}
*/
			}
		}

		if(device == IDD_TouchPanel)
		{
			MFTouchPanelState *pState = MFInput_GetContactInfo(deviceID);

			for(int b=0; b<MaxContacts; ++b)
			{
				if(b < pState->numContacts && touchContacts[b] == -1 && pState->contacts[b].phase < 3)
				{
					for(int c=0; c<MaxContacts; ++c)
					{
						if(!bCurrentContacts[c])
						{
							bCurrentContacts[c] = true;
							touchContacts[b] = c;

							// create the hover contact
							MFVector pos = CorrectPosition((float)pState->contacts[b].x, (float)pState->contacts[b].y);
							contacts[c].Init(&sources[a], b, pos.x, pos.y);
							contacts[c].bState = true;

//							pNewContactCallback(c);

							// send the down event
							EventInfo info;
							InitEvent(info, IE_Down, b);
							info.down.x = contacts[c].x;
							info.down.y = contacts[c].y;

							OnInputEvent(*this, info);
							break;
						}
					}
				}
				else if(touchContacts[b] != -1 && (b >= pState->numContacts || pState->contacts[b].phase >= 3))
				{
					int c = touchContacts[b];

					if(!contacts[c].bDrag)
					{
						// event classifies as a tap
						EventInfo info;
						InitEvent(info, IE_Tap, contacts[c].buttonID);
						info.tap.x = contacts[c].x;
						info.tap.y = contacts[c].y;
						info.tap.holdLength = contacts[c].downTime;

						OnInputEvent(*this, info);
					}

					// send the up event
					EventInfo info;
					InitEvent(info, IE_Up, contacts[c].buttonID);
					info.up.x = contacts[c].x;
					info.up.y = contacts[c].y;
					info.up.downX = contacts[c].downX;
					info.up.downY = contacts[c].downY;
					info.up.holdLength = contacts[c].downTime;

					OnInputEvent(*this, info);

					bCurrentContacts[c] = false;
					touchContacts[b] = -1;
				}
			}
		}
	}

	// track moved contacts
	struct Moved
	{
		int contact;
		float x, y;
	} moved[16];
	int numMoved = 0;

	for(int a=0; a<MaxContacts; ++a)
	{
		if(!bCurrentContacts[a])
			continue;

		HKInputSource *pContactSource = contacts[a].pSource;
		MFInputDevice device = pContactSource->device;
		int deviceID = pContactSource->deviceID;

		if(device != IDD_TouchPanel)
		{
			if(!MFInput_IsReady(device, deviceID))
			{
				bCurrentContacts[a] = false;

				if(device == IDD_Mouse)
					mouseContacts[deviceID] = -1;
				else if(device == IDD_TouchPanel)
					touchContacts[contacts[a].buttonID] = -1;
			}

			if(device == IDD_Mouse)
			{
				MFVector pos = CorrectPosition(MFInput_Read(Mouse_XPos, device, deviceID), MFInput_Read(Mouse_YPos, device, deviceID));

				if(pos.x != contacts[a].x || pos.y != contacts[a].y)
				{
					EventInfo info;
					InitEvent(info, IE_Hover, a);
					info.hover.x = pos.x;
					info.hover.y = pos.y;
					info.hover.deltaX = pos.x - contacts[a].x;
					info.hover.deltaY = pos.y - contacts[a].y;

					OnInputEvent(*this, info);

					if(!contacts[a].bDrag)
					{
						float distX = pos.x - contacts[a].downX;
						float distY = pos.y - contacts[a].downY;
						if(MFSqrt(distX*distX + distY*distY) >= dragThreshold)
							contacts[a].bDrag = true;
					}

					if(contacts[a].bDrag && contacts[a].bState)
					{
						// send the drag event
						info.ev = IE_Drag;
						info.drag.startX = contacts[a].downX;
						info.drag.startY = contacts[a].downY;

						OnInputEvent(*this, info);
					}

					contacts[a].x = pos.x;
					contacts[a].y = pos.y;
				}
			}

			if(contacts[a].bState)
			{
				if(!MFInput_Read(contacts[a].buttonID, device, deviceID))
				{
					// send the up event
					EventInfo info;
					InitEvent(info, IE_Up, a);
					info.up.x = contacts[a].x;
					info.up.y = contacts[a].y;
					info.up.downX = contacts[a].downX;
					info.up.downY = contacts[a].downY;
					info.up.holdLength = contacts[a].downTime;

					OnInputEvent(*this, info);

					if(!contacts[a].bDrag)
					{
						// event classifies as a tap
						EventInfo info;
						InitEvent(info, IE_Tap, a);
						info.tap.x = contacts[a].x;
						info.tap.y = contacts[a].y;
						info.tap.holdLength = contacts[a].downTime;

						OnInputEvent(*this, info);
					}

					bCurrentContacts[a] = false;

					// if it was a mouse, release the button to we can sense it again
					if(device == IDD_Mouse)
						mouseButtonContacts[deviceID][contacts[a].buttonID - Mouse_LeftButton] = -1;
				}
				else
				{
					contacts[a].downTime += MFSystem_TimeDelta();
				}
			}
		}
		else
		{
			MFVector pos = CorrectPosition(MFInput_Read(Touch_XPos(contacts[a].buttonID), IDD_TouchPanel), MFInput_Read(Touch_YPos(contacts[a].buttonID), IDD_TouchPanel));

			if(pos.x != contacts[a].x || pos.y != contacts[a].y)
			{
				EventInfo info;
				InitEvent(info, IE_Hover, a);
				info.hover.x = pos.x;
				info.hover.y = pos.y;
				info.hover.deltaX = pos.x - contacts[a].x;
				info.hover.deltaY = pos.y - contacts[a].y;

				OnInputEvent(*this, info);

				float distX = pos.x - contacts[a].downX;
				float distY = pos.y - contacts[a].downY;
				if(!contacts[a].bDrag && MFSqrt(distX*distX + distY*distY) >= dragThreshold)
				{
					contacts[a].bDrag = true;
					info.hover.deltaX = pos.x - contacts[a].downX;
					info.hover.deltaY = pos.y - contacts[a].downY;
				}

				if(contacts[a].bDrag && contacts[a].bState)
				{
					// send the drag event
					info.ev = IE_Drag;
					info.drag.startX = contacts[a].downX;
					info.drag.startY = contacts[a].downY;

					OnInputEvent(*this, info);
				}

				// store the old pos for further processing
				moved[numMoved].x = contacts[a].x;
				moved[numMoved].y = contacts[a].y;
				moved[numMoved++].contact = a;

				contacts[a].x = pos.x;
				contacts[a].y = pos.y;
			}
		}
	}

	if(numMoved > 1)
	{
		// calculate rotation and zoom for each pair of contacts
		for(int a=0; a<numMoved; ++a)
		{
			for(int b=a+1; b<numMoved; ++b)
			{
				MFVector center;
				MFVector newa = MakeVector(contacts[moved[a].contact].x, contacts[moved[a].contact].y);
				MFVector newDiff = MakeVector(contacts[moved[b].contact].x, contacts[moved[b].contact].y) - newa;
				MFVector oldDiff = MakeVector(moved[b].x - moved[a].x, moved[b].y - moved[a].y);
				center.Mad2(newDiff, 0.5f, newa);

				float oldLen = oldDiff.Magnitude2();
				float newLen = newDiff.Magnitude2();
				float scale = newLen / oldLen;

				EventInfo info;
				InitEvent(info, IE_Pinch, moved[a].contact);
				info.pinch.contact2 = moved[b].contact;
				info.pinch.centerX = center.x;
				info.pinch.centerY = center.y;
				info.pinch.deltaScale = scale;

				OnInputEvent(*this, info);

				oldDiff.Mul2(oldDiff, 1.f/oldLen);
				newDiff.Mul2(newDiff, 1.f/newLen);
				float angle = oldDiff.GetAngle(newDiff);

				info.ev = IE_Spin;
				info.spin.deltaAngle = angle;

				OnInputEvent(*this, info);
			}
		}
	}
}

float HKInputManager::SetDragThreshold(float threshold)
{
	float old = dragThreshold;
	dragThreshold = threshold;
	return old;
}

void HKInputManager::ScanForDevices()
{
	// *** TODO: support hot-swapping devices... ***

	// scan for mouse devices
	int count = MFInput_GetNumPointers();
	for(int a=0; a<count && numDevices<MaxSources; ++a)
	{
		HKInputSource &source = sources[numDevices];

		source.sourceID = numDevices;
		source.device = IDD_Mouse;
		source.deviceID = a;

		numDevices++;
	}

	count = MFInput_GetNumKeyboards();
	for(int a=0; a<count && numDevices<MaxSources; ++a)
	{
		HKInputSource &source = sources[numDevices];

		source.sourceID = numDevices;
		source.device = IDD_Keyboard;
		source.deviceID = a;

		numDevices++;
	}

	count = MFInput_GetNumGamepads();
	for(int a=0; a<count && numDevices<MaxSources; ++a)
	{
		HKInputSource &source = sources[numDevices];

		source.sourceID = numDevices;
		source.device = IDD_Gamepad;
		source.deviceID = a;

		numDevices++;
	}

	count = MFInput_GetNumTouchPanels();
	for(int a=0; a<count && numDevices<MaxSources; ++a)
	{
		HKInputSource &source = sources[numDevices];

		source.sourceID = numDevices;
		source.device = IDD_TouchPanel;
		source.deviceID = a;

		numDevices++;
	}
}

MFVector HKInputManager::CorrectPosition(float x, float y)
{
	MFDebug_Assert(MFDisplay_GetDisplayOrientation() == MFDO_Normal, "Support display rotation!");

	if(MFDisplay_GetDisplayOrientation() == MFDO_Normal)
	{
		return MakeVector(x, y);
	}
	else
	{
//		MFMatrix inputMat;
//		GetInputMatrix(&inputMat);
//		return inputMat.TransformVectorH(MakeVector(x, y, 0.f, 1.f));
		return MakeVector(x, y);
	}
}

void HKInputManager::InitEvent(EventInfo &info, EventType ev, int contact)
{
	info.ev = ev;
	info.contact = contact;
	info.pSource = contacts[contact].pSource;
	info.buttonID = contacts[contact].buttonID;

	// the mouse buttons are stored according to the MFInput enum values
	if(info.buttonID != -1 && info.pSource->device == IDD_Mouse)
		info.buttonID -= Mouse_LeftButton;
}

void HKInputManager::Contact::Init(HKInputSource *_pSource, int _buttonID, float _x, float _y)
{
	pSource = _pSource;
	buttonID = _buttonID;
	downTime = 0.f;
	bDrag = false;
	bState = false;
	x = downX = _x;
	y = downY = _y;
}
