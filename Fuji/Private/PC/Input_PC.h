//#ifndef ME_INPUT
//#define ME_INPUT
#pragma once

#define ME_CTRLTYPE_UNDEFINED 0x0
#define ME_CTRLTYPE_KEY 0x1
#define ME_CTRLTYPE_AXIS 0x2
#define ME_CTRLTYPE_BUTTON 0x3

#define CTRL_UNDEFINED 0x0
#define CTRL_KEY 0x1
#define CTRL_JOYSTICK 0x2
#define CTRL_MOUSE 0x3

#define CTRL_JOYSTICK_X			0
#define CTRL_JOYSTICK_Y			1
#define CTRL_JOYSTICK_Z			2
#define CTRL_JOYSTICK_RX		3
#define CTRL_JOYSTICK_RY		4
#define CTRL_JOYSTICK_RZ		5
#define CTRL_JOYSTICK_SLIDER	6
#define CTRL_JOYSTICK_BUTTON	12
#define CTRL_JOYSTICK_VX		44
#define CTRL_JOYSTICK_VY		45
#define CTRL_JOYSTICK_VZ		46
#define CTRL_JOYSTICK_VRX		47
#define CTRL_JOYSTICK_VRY		48
#define CTRL_JOYSTICK_VRZ		49
#define CTRL_JOYSTICK_VSLIDER	50
#define CTRL_JOYSTICK_AX		52
#define CTRL_JOYSTICK_AY		53
#define CTRL_JOYSTICK_AZ		54
#define CTRL_JOYSTICK_ARX		55
#define CTRL_JOYSTICK_ARY		56
#define CTRL_JOYSTICK_ARZ		57
#define CTRL_JOYSTICK_ASLIDER	58
#define CTRL_JOYSTICK_FX		60
#define CTRL_JOYSTICK_FY		61
#define CTRL_JOYSTICK_FZ		62
#define CTRL_JOYSTICK_FRX		63
#define CTRL_JOYSTICK_FRY		64
#define CTRL_JOYSTICK_FRZ		65
#define CTRL_JOYSTICK_FSLIDER	66

#define CTRL_MOUSE_X			0
#define CTRL_MOUSE_Y			1
#define CTRL_MOUSE_Z			2
#define CTRL_MOUSE_BUTTON		3

class ME_CONTROL
{
public:
	char Type;
	void *Data;
	int Sign;

	int Source;
	int Index;

	int Object;
	int Property;

	char Desc[30];

	ME_CONTROL();
	~ME_CONTROL();
};

class KEYPRESS
{
public:
	KEYPRESS();

	void SetKey(DWORD key);
	void SetControl(ME_CONTROL *control);
	void SetPointer(char *ptr);
	bool IsPressed();

protected:
	DWORD Key;
	ME_CONTROL *Control;
	char *Pointer;
	bool Pressed;
};

int inInit();
int inSetCooperativeLevels();
void inKillIn();
void inAcquire(bool acquire);

void meProcessInput();
void meUpdateKeyboard();
void meUpdateMouse();
void meUpdateJoystick();

void meSetScreenMouseRange(float x,float y);
void meSetScreenMouseSensitivity(float sens);

int meGetKeyboardCount();
int meGetMouseCount();
int meGetJoystickCount();

void meGetInput(ME_CONTROL *ctrl, void (*ProcessFunc)());
int meDigitalReading(ME_CONTROL *ctrl);
int meAnalogReading(ME_CONTROL *ctrl);
void meSetDigitalRatio(float ratio);
void meSetAnalogRange(int Minimum, int Range);
void meSetAnalogDeadZone(float DeadZone);

void meEnumerateString(ME_CONTROL *ctrl, char *String);
void meAcquireDataPointer(ME_CONTROL *ctrl);

const char DIK2ASCIILC[256]={NULL,NULL,'1','2','3','4','5','6','7','8','9','0','-','=','\b','\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',NULL,'a','s','d','f','g','h','j','k','l',';','\'','`',NULL,'\\','z','x','c','v','b','n','m',',','.','/',NULL,'*',NULL,' ',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'7','8','9','-','4','5','6','+','1','2','3','0','.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'=',NULL,NULL,NULL,'@',':','_',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'\n',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,',',NULL,'/',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
const char DIK2ASCIIUC[256]={NULL,NULL,'!','@','#','$','%','^','&','*','(',')','_','+',NULL,NULL,'Q','W','E','R','T','Y','U','I','O','P','{','}',NULL,NULL,'A','S','D','F','G','H','J','K','L',':','"','~',NULL,'|','Z','X','C','V','B','N','M','<','>','?',NULL,'*',NULL,' ',NULL};

//#endif //ME_INPUT
