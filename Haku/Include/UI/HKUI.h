#if !defined(_HKUI_H)
#define _HKUI_H

#include "HKFactory.h"
#include "HKWidget.h"
#include "HKInputSource.h"

#include "MFSystem.h"

class HKWidgetRenderer;
class HKWidgetLayoutFrame;

typedef HKFactory<HKWidget> HKWidgetFactory;
typedef HKFactory<HKWidgetRenderer> HKWidgetRendererFactory;

class HKUserInterface
{
public:
	static void Init();
	static void Deinit();

	static HKWidgetFactory::FactoryType *RegisterWidget(const char *pWidgetType, HKWidgetFactory::CreateFunc createDelegate, HKWidgetFactory::FactoryType *pParent);
	static HKWidgetRendererFactory::FactoryType *RegisterWidgetRenderer(const char *pWidgetType, HKWidgetRendererFactory::CreateFunc createDelegate, HKWidgetRendererFactory::FactoryType *pParent);
	static HKWidgetFactory::FactoryType *FindWidgetType(const char *pWidgetType);

	static HKWidget *CreateWidget(const char *pWidgetType);

	static void SetActiveUI(HKUserInterface *pUI) { pActive = pUI; }
	static HKUserInterface &Get() { return *pActive; }

	HKUserInterface();
	~HKUserInterface();

	void Update();
	void Draw();

	void AddTopLevelWidget(HKWidget *pWidget, bool bOwnWidget);

	HKWidget *SetFocus(HKInputSource *pSource, HKWidget *pFocusWidget);

protected:
	HKWidgetLayoutFrame *pRoot;

	HKInputManager *pInputManager;

	HKWidget *pFocusList[HKInputManager::MaxSources];
	HKWidget *pHoverList[HKInputManager::MaxSources];

	static HKUserInterface *pActive;
	static HKWidgetFactory *pFactory;
	static HKWidgetRendererFactory *pRendererFactory;

	void OnInputEvent(HKInputManager &manager, HKInputManager::EventInfo &ev);

	static MFSystemCallbackFunction pChainResizeCallback;
	static void ResizeCallback();
};

#endif
