#pragma once
#if !defined(_HKWIDGET_STYLE_H)
#define _HKWIDGET_STYLE_H

#include "MFOpenHashTable.h"
#include "MFArray.h"

class TiXmlElement;
class HKWidget;

class HKWidgetStyle
{
public:
	static void Init();
	static void Deinit();

	static bool LoadStylesFromXML(const char *pFilename);

	static HKWidgetStyle* FindStyle(const char *pStyle);

	HKWidgetStyle() { numProperties = 0; }
	~HKWidgetStyle() {}

	int GetNumProperties() { return numProperties; }
	int GetProperty(int property) { return properties[property].property; }
	int GetPropertyValue(int property) { return properties[property].value; }

	void Apply(HKWidget *pWidget);

protected:
	struct Property
	{
		MFString property;
		MFString value;
	};

	MFString name;
	MFString parent;

	int numProperties;
	MFArray<Property> properties;

	static MFOpenHashTable<HKWidgetStyle> sStyles;
};

#endif
