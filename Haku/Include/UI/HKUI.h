#if !defined(_HKUI_H)
#define _HKUI_H

#include "HKFactory.h"
#include "HKWidget.h"
#include "HKWidgetEvent.h"

class HKWidgetRenderer;
class HKInputSource;

typedef HKFactory<HKWidget> HKWidgetFactory;

class HKUserInterface
{
public:
	static void Init();
	static void Deinit();

	static HKWidgetFactory::FactoryType *RegisterWidget(const char *pWidgetType, HKWidgetFactory::CreateFunc createDelegate, HKWidgetFactory::FactoryType *pParent);
	static HKWidgetFactory::FactoryType *FindWidgetType(const char *pWidgetType);

	static HKWidget *CreateWidget(const char *pWidgetType);

	static void SetActiveUI(HKUserInterface *pUI) { pActive = pUI; }
	static HKUserInterface &Get() { return *pActive; }

	HKUserInterface();
	~HKUserInterface();

	void Update();
	void Draw();

	void SetRootWidget(HKWidget *pWidget) { pWidget->pParent = NULL; pRoot = pWidget; }

protected:
	HKWidget *pRoot;
	HKWidgetRenderer *pRenderer;

	HKInputSource **ppFocusList;
	HKInputSource **ppHoverList;

	static HKUserInterface *pActive;
};

#endif
