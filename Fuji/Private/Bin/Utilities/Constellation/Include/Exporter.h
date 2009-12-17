#if !defined(_EXPORTER_H)
#define _EXPORTER_H

#include "Global.h"
#include "StatusWindow.h"

class Exporter : public SceneExport {
public:
	Exporter();
	~Exporter();
	int ExtCount();
	const TCHAR * Ext(int i);
	const TCHAR * LongDesc();
	const TCHAR * ShortDesc();
	const TCHAR * AuthorName();
	const TCHAR * CopyrightMessage();
	const TCHAR * OtherMessage1();
	const TCHAR * OtherMessage2();
	unsigned int Version();
	void ShowAbout(HWND hWnd);
	
	int DoExport(const TCHAR *name, ExpInterface *ei, Interface *i, BOOL suppressPrompts = FALSE, DWORD options = 0);

	StatusWindow *status;

private:
	bool ProcessNode(INode *node);
	bool ProcessGeomObject(INode *node, ObjectState *os);

	unsigned long indentation;

	Interface *i;
};

#endif