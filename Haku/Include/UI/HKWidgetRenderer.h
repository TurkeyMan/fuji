#if !defined(_HKWIDGET_RENDERER_H)
#define _HKWIDGET_RENDERER_H

class HKWidget;

class HKWidgetRenderer
{
public:
	virtual void Render(const HKWidget &widget, const MFMatrix &worldTransform) = 0;

	virtual void SetPropertyB(const char *pProperty, bool bValue) {}
	virtual void SetPropertyI(const char *pProperty, int value) {}
	virtual void SetPropertyF(const char *pProperty, float value) {}
	virtual void SetPropertyV(const char *pProperty, const MFVector& value) {}
	virtual void SetPropertyS(const char *pProperty, const char *pValue) {}

	virtual bool GetPropertyB(const char *pProperty)			{ return false; }
	virtual int GetPropertyI(const char *pProperty)				{ return 0; }
	virtual float GetPropertyF(const char *pProperty)			{ return 0.f; }
	virtual const MFVector &GetPropertyV(const char *pProperty)	{ return MFVector::zero; }
	virtual MFString GetPropertyS(const char *pProperty)		{ return NULL; }
};

#endif
