#if !defined(_MK_WIDGETS)
#define _MK_WIDGETS

// standard widget messages
enum WidgetMessages
{
  MKWM_INVALIDMESSAGE = -1, // an invalid message value

  MKWM_UNKNOWN = 0, // an undefined/unknown message

  MKWM_CONSTRUCT = 1,
  MKWM_CREATE, // this message is sent to a widget when it is created
  MKWM_DESTROY, // this message is sent to a widget before it is destroyed

  MKWM_MOVE, // requests a widget to move (handled by base message handler)
  MKWM_SIZE, // requests a widget to resize (handled by base message handler)
  MKWM_SCALE,
  MKWM_SETCOLOUR,

  MKWM_GETPOS,  // return the position in a structure passed in as data
  MKWM_GETSIZE, // return the size in a structure passed in as data
  MKWM_GETSCALE,
  MKWM_GETCOLOUR,

  MKWM_GETBOUNDINGVOLUME,
  MKWM_GETCLIENTVOLUME,

  MKWM_GETNAME,
  MKWM_SETNAME,

  MKWM_SHOW, // sent to a widget when its visibility state changes
  MKWM_ENABLE, // sent to a widget when its enabled state changes

  MKWM_SETFOCUS, // sent to a window when it gains/loses focus
  MKWM_SETMOUSEFOCUS, // sent to a widget when it receives or loses mouse focus

  MKWM_GAMEPAD, // a gamepad event, accompanied by a BID and devide ID

  MKWM_KEY, // sent as a result of a keypress
  MKWM_KEYDOWN, // key down
  MKWM_KEYUP, // key released

  MKWM_MOUSEMOVE, // mouse move sent to focus or top most widget
  MKWM_MOUSEOVER, // sent when a mouse hovers over or leaves the region of a widget
  MKWM_MOUSEDOWN, // a mouse button was pressed
  MKWM_MOUSEUP, // a mouse button was released
  MKWM_MOUSECLICK, // a mouse click
  MKWM_MOUSEDOUBLECLICK, // a double click
  MKWM_MOUSEWHEEL, // scroll wheel

  MKWM_DRAGSTART, // request drag begin
  MKWM_DRAGOVER, // sent in addition to MOUSEOVER during a drag operation
  MKWM_DROP, // request to receive a drag event

  MKWM_NOTIFY,  // a notify message passed from a child to its parent notifying of a given event (eg. a command button was clicked, scrollbar changed, etc)

  MKWM_ADDCHILD,
  MKWM_REMOVECHILD,
  MKWM_SENDTOBACK,
  MKWM_BRINGTOFRONT,

  MKWM_CUSTOM = 100,      // custom messages for use in other widgets

  MKWM_USER = 1000,       // messages from this point onwards are safe for custom usage
};

enum WidgetFlags
{
  WF_Enabled = 1,
  WF_Visible = 2,
};

class MKWidget;
class MKWidgetContainer;
class MKWidgetSystem;

extern MKWidgetSystem widgetSystem;

struct MKWidgetAttributes
{
  static MKWidgetAttributes identityAttributes;

  MKVector3 position;
  MKVector3 rotation;
  MKVector3 dimensions;
  MKVector4 colour;

  float scale;
  uint32 attributeFlags;
};

struct MKWidgetRenderContext
{
  static MKWidgetRenderContext identity;

  Matrix tramsform;
  MKVector4 colour;
};

struct MKWidgetBoundingVolume
{
  MKVector3 position;
  MKVector3 dimensions;
};

// Message hook function typedef
typedef int (*MKWidgetMessageHook)(MKWidget *, uint32, uint32, void*);

typedef MKWidget* (*MKWidgetFactoryFunc)(void);


/**** Internal Functions ****/

void MKWidget_InitModule();
void MKWidget_DeinitModule();

void MKWidget_Update();
void MKWidget_Draw();


/**** Interface Functions ****/

MKWidget* MKWidget_Create(
  const char *pWidgetType, // a char* referencing a factories type
  const char *pName, // name of widget
  const MKWidgetAttributes *pAttributes, // default attributes, pass NULL for identity
  MKWidget *pParent, // optional pointer to parent widget (must be a container of some form)
  MKWidgetMessageHook pMessageHook, // optional pointer to message hook function
  void *pUserData // optional pointer to user data
);

int MKWidget_Destroy(
  MKWidget *pWidget // pointer to widget to be destroyed
);


//Send a message to a Widget:

//This is the only method of communication with a Widget, with the exception of some wrapper functions for common operations (moving/sizing etc) for simplicity.
int MKWidget_SendMessage(
  MKWidget *pWidget, // widget to receive message
  uint32 message, // message
  uint32 messageData // message data
);


//Management:

//Finding widgets in the system.

MKWidget* MKWidget_Find(const char *pName, MKWidgetContainer *pParent = NULL); // find a widget by name
MKWidgetContainer* MKWidget_GetParent(MKWidget *pWidget); // get a widgets parent

const char* MKWidget_GetName(MKWidget *pWidget);


//Following are some functions to simplify typical tasks:

//These would simply send standard messages that would (usually) be handled by the default message handler. These messages can still be handled by a message hook, and/or not handled by the default handler if chosen.

//Get/Set name
void MKWidget_SetName(MKWidget *pWidget, const char *pName);
const char* MKWidget_SetName(MKWidget *pWidget);

//Move/resize methods:
int MKWidget_Move(MKWidget *pWidget, const MKVector3& position);
int MKWidget_Resize(MKWidget *pWidget, const MKVector3& dimensions);
int MKWidget_Scale(MKWidget *pWidget, float scale);
int MKWidget_SetColour(MKWidget *pWidget, const MKVector4& colour);

//Show/Enable:
int MKWidget_Show(MKWidget *pWidget, int show);
int MKWidget_Enable(MKWidget *pWidget, bool enable);

//For managing input focus (each return previous focus):
MKWidget* MKWidget_SetFocus(MKWidget *pWidget);
MKWidget* MKWidget_SetMouseFocus(MKWidget *pWidget);
MKWidget* MKWidget_ReleaseMouseFocus();

void MKWidget_GetMousePos(MKVector3 *pMousePos);
void MKWidget_GetLocalMousePos(MKWidget *pWidget, MKVector3 *pMousePos);

//Some Advanced functions for use in data driven systems:

//Custom Message Handlers:

//Used to register a custom message handler to be linked up during creation of data driven source data.
void MKWidget_RegisterCustomMessageHandler(
  const char *pName, // identifier for message hook
  MKWidgetMessageHook pMessageHook // pointer to message hook
);


//Load/Store:

void MKWidget_LoadWidgetFile(
  const char *pFilename, // filename to load
  MKWidgetContainer *pParent // base widgets in the file will be loaded as children of given container
);

void MKWidget_SaveWidgetFile(
  const char *pFilename, // filename to write
  MKWidget *pWidget // base of tree to export
);


// **** Internal Stuff ****

class MKWidgetDescriptor;

void MKWidget_AddToDescriptorList(MKWidgetDescriptor *pDescriptor);

//Typical Widget descriptor, stores widget type information.
class MKWidgetDescriptor
{
public:
  MKWidgetDescriptor(const char *name, MKWidgetDescriptor *parent, MKWidgetFactoryFunc creator);

  const char *pName;          // the name of the Widget type
  MKWidgetDescriptor *pBase;  // pointer to the base Widget type descriptor

  MKWidgetDescriptor *pNext;  // next descriptor

  // OPTIONAL: this would be used in a factory to create an instance of given type
  MKWidgetFactoryFunc CreateInstance;
};

class MKWidget
{
public:
  MKWidget();

  virtual int MessageHandler(uint32 message, uint32 data); // default message handler

  virtual void Render(MKWidgetRenderContext *pRenderContext);       // default draw function (draw nothing)
  virtual void Draw(MKWidgetRenderContext renderContext); // draw each child
  virtual void UpdateState();     // this would just be used for animation, functional updated would be triggered by messages
  virtual void Update();          // update each child

  virtual MKWidget* TestPoint(MKVector3 *pPoint); // used to test if a point is within this widgets bounds
  virtual MKWidget* GetWidgetUnderPoint(MKVector3 point); // recursively find top level widget under a given point

  virtual void GetLocalToWorldMatrix(Matrix *pMatrix);

  virtual bool IsContainer() { return false; }

  MKWidgetDescriptor    *pDescriptor;     // data about Widget type, could be used for an RTTI type thing

  MKWidgetMessageHook   pMessageHook;     // message hook callback function, can be used to attach custom events to standard Widgets
  void                  *pUserData;       // pointer to generic user data

  MKWidget              *pParent;         // pointer to parent Widget, can be NULL. (note: a parent will always be a container)
  MKWidget              *pNextSibling;    // next sibling in the linked list

  MKWidgetAttributes    attributes;       // maintains the render context

  const char            *pName;           // Widget name
  uint32                widgetFlags;      // generic flags
};

class MKWidgetContainer : public MKWidget
{
public:
  typedef MKWidget parent_class;

  MKWidgetContainer();

  // containers message handler will handle messages to add/remove children and manage ordering etc.
  virtual int MessageHandler(uint32 message, uint32 data);

  virtual void Draw(MKWidgetRenderContext renderContext);          // draw each child
  virtual void Update();        // update each child

  virtual MKWidget* GetWidgetUnderPoint(MKVector3 point);

  virtual bool IsContainer() { return true; }

  MKWidget  *pChildren;                 // pointer to start of children linked list
  uint32    childCount;                 // number of children
};

class MKWidgetSystem : public MKWidgetContainer
{
public:
  typedef MKWidgetContainer parent_class;

  MKWidgetSystem();

  virtual void Draw(MKWidgetRenderContext renderContext);
  virtual void UpdateState();

  MKWidget* ChangeFocus(MKWidget *pWidget);
  MKWidget* ChangeMouseFocus(MKWidget *pWidget);

  char keyState[256];     // stores state info for each key
  float keyDownTime[256]; // stores how long keys have been held down for key repeats

  MKWidget *pFocus;       // object with input focus
  MKWidget *pMouseFocus;  // last item mouse was hovering above
  MKWidget *pMouseLock;   // exclusive lock of mouse focus

  MKWidget *pClick;
  MKWidget *pDoubleClick;
  int clickTestButton;
  int doubleClickTestButton;

  MKVector3 oldPointerPos;
};

class MKWidgetFrame : public MKWidgetContainer
{
public:
  typedef MKWidgetContainer parent_class;

  enum FrameStyle
  {
    FRM_None = 0,

    FRM_Raised = 1,
    FRM_Sunken = 2,
    FRM_ClientEdge = 3,
    FRM_SunkenClientEdge = 4,

    FRM_FillCenter = 16,
    FRM_UseTexture = 32,
    FRM_DisplayCaption = 64,
    FRM_CenterCaption = 128
  };

  enum FrameMessages
  {
    FM_SETSTYLE = MKWM_CUSTOM,
    FM_GETSTYLE,
  };

  MKWidgetFrame();

  virtual int MessageHandler(uint32 message, uint32 data);

  virtual void Render(MKWidgetRenderContext *pRenderContext);

  float edgeWidth;
  float outerMargin;
  float clientMargin;

  uint32 frameStyle;
};

class MKWidgetString : public MKWidget
{
public:
  typedef MKWidget parent_class;

  enum StringMessages
  {
    WSM_SETTEXT = MKWM_CUSTOM,
    WSM_GETTEXT,
    WSM_SETJUSTIFY
  };

  MKWidgetString();

  virtual int MessageHandler(uint32 message, uint32 data);

  virtual void Render(MKWidgetRenderContext *pRenderContext);

  MKFont *pFont;
  char *pText;
  uint32 fontJustify;
};

class MKWidgetButton : public MKWidget
{
public:
  typedef MKWidget parent_class;

  MKWidgetButton();

  virtual int MessageHandler(uint32 message, uint32 data);

  virtual void Render(MKWidgetRenderContext *pRenderContext);

  virtual MKWidget* TestPoint(MKVector3 *pPoint);

  MKWidgetString label;
  bool buttonPressed, buttonDown;
};

class MKWidgetWindowTitleBar : public MKWidget
{
public:
  typedef MKWidget parent_class;

  enum TitleStyle
  {
    WF_ShowCaption = 1,
    WF_CloseButton = 2,
    WF_SysMenu = 3
  };

  MKWidgetWindowTitleBar();

  virtual int MessageHandler(uint32 message, uint32 data);

  virtual void Render(MKWidgetRenderContext *pRenderContext);

  virtual MKWidget* TestPoint(MKVector3 *pPoint);

  MKVector3 mousePos;

  MKWidgetString title;
  MKWidgetButton close;

  uint32 titleFlags;
  uint32 grabLocation;
  bool dragging, sizing, scaling, rotating;
};

class MKWidgetWindow : public MKWidgetFrame
{
public:
  typedef MKWidgetFrame parent_class;

  enum FrameStyle
  {
    WF_Border = 1,
    WF_Sizable = 2,
    WF_Title = 4,
    WF_CloseButton = 8,
    WF_SysMenu = 16,
  };

  enum FrameMessages
  {
    WWM_SETFLAGS = MKWM_CUSTOM,
    WWM_GETFLAGS,
    WWM_CLOSE // close request (probably close gadget clicked)
  };

  MKWidgetWindow();

  virtual int MessageHandler(uint32 message, uint32 data);

  virtual void Render(MKWidgetRenderContext *pRenderContext);

  virtual MKWidget* TestPoint(MKVector3 *pPoint);

  MKVector3 scrollOffset;
  uint32    windowFlags;

  MKWidgetWindowTitleBar  titleBar;

  // some static data
  static MKVector4 windowColour;
  static MKVector4 clientColour;
  static MKVector4 titleColour;
  static float borderWidth;
  static float titleHeight;
};

class MKWidgetCheckMark : public MKWidget
{
public:
  typedef MKWidget parent_class;

  MKWidgetCheckMark();

  virtual int MessageHandler(uint32 message, uint32 data);

  virtual void Render(MKWidgetRenderContext *pRenderContext);

  int checkState;
  bool buttonPressed, buttonDown;
};

class MKWidgetCheckBox : public MKWidget
{
public:
  typedef MKWidget parent_class;

  MKWidgetCheckBox();

  virtual int MessageHandler(uint32 message, uint32 data);

  virtual void Render(MKWidgetRenderContext *pRenderContext);

  virtual MKWidget* TestPoint(MKVector3 *pPoint);

  MKWidgetCheckMark check;
  MKWidgetString label;
};

class MKWidgetThumb : public MKWidget
{
public:
  typedef MKWidget parent_class;

  enum ThumbMessages
  {
    WTM_SETMINPOS = MKWM_CUSTOM,
    WTM_SETMAXPOS
  };

  MKWidgetThumb();

  virtual int MessageHandler(uint32 message, uint32 data);

  virtual void Render(MKWidgetRenderContext *pRenderContext);

  MKVector3 minScroll, maxScroll;
  MKVector3 mousePos;

  bool dragging;
};

class MKWidgetScrollBar : public MKWidget
{
public:
  typedef MKWidget parent_class;

  enum ScrollBarMessages
  {
    WSM_SETVALUE = MKWM_CUSTOM,
    WSM_GETVALUE,
    WSM_SETMIN,
    WSM_SETMAX,
    WSM_SETINCREMENT
  };

  MKWidgetScrollBar();

  virtual int MessageHandler(uint32 message, uint32 data);

  virtual void Render(MKWidgetRenderContext *pRenderContext);

  virtual MKWidget* TestPoint(MKVector3 *pPoint);

  MKWidgetButton increase, decrease;
  MKWidgetThumb thumb;

  float value, minVal, maxVal, inc;
  float thumbWidth;
};

class MKWidgetSlider : public MKWidget
{
public:
  typedef MKWidget parent_class;

  enum SliderMessages
  {
    WSM_SETVALUE = MKWM_CUSTOM,
    WSM_GETVALUE,
    WSM_SETMIN,
    WSM_SETMAX,
    WSM_SETINCREMENT
  };

  MKWidgetSlider();

  virtual int MessageHandler(uint32 message, uint32 data);

  virtual void Render(MKWidgetRenderContext *pRenderContext);

  virtual MKWidget* TestPoint(MKVector3 *pPoint);

  MKWidgetThumb thumb;

  float value, minVal, maxVal, inc;
  float thumbWidth;
};

class MKWidgetTextBox : public MKWidgetFrame
{
public:
  typedef MKWidgetFrame parent_class;

};

class MKWidgetRadioButton : public MKWidget
{
public:
  typedef MKWidget parent_class;

};

class MKWidgetListBox : public MKWidgetContainer
{
public:
  typedef MKWidgetContainer parent_class;

};

class MKWidgetListItem : public MKWidgetContainer
{
public:
  typedef MKWidgetContainer parent_class;


};

#endif // _MK_WIDGETS
