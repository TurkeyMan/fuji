#include "Global.h"
#include "Exporter.h"
#include "ConstelFile.h"

Exporter::Exporter()
{
	status = NULL;
}

Exporter::~Exporter()
{
	if(status != NULL) {
		delete status;
	}
}

int Exporter::ExtCount()
{
	return(1);
}

const TCHAR * Exporter::Ext(int i)
{
	switch(i) {
		case 0:
			return(FILE_EXTENSION);
		default:
			return(_T(""));
	}
}

const TCHAR * Exporter::LongDesc()
{
	return(LONG_FF_DESCRIPTION);
}

const TCHAR * Exporter::ShortDesc()
{
	return(SHORT_FF_DESCRIPTION);
}

const TCHAR * Exporter::AuthorName()
{
	return(AUTHOR_NAME);
}

const TCHAR * Exporter::CopyrightMessage()
{
	return(COPYRIGHT);
}

const TCHAR * Exporter::OtherMessage1()
{
	return(_T(""));
}

const TCHAR * Exporter::OtherMessage2()
{
	return(_T(""));
}

unsigned int Exporter::Version()
{
	return(PLUGIN_VERSION);
}

void Exporter::ShowAbout(HWND hWnd)
{
	return;
}

int Exporter::DoExport(const TCHAR *name, ExpInterface *ei, Interface *i, BOOL suppressPrompts, DWORD options)
{
	this->i = i;
	INode *rootNode;

	/* Be VERY careful what locals you define before this.  Something might use the
	 * status window in its constructor, which would cause a segfault.
	 */
	if(status == NULL) {
		status = new StatusWindow(NULL);
	}

//	status->Print("Hello World\r\n");

	ConstelFile constelFile;
	try {
		constelFile.Save(name);
	}
	catch(ChunkedFileException ex) {
		status->Print("Error while saving file (%s)\r\n", ex.message);
	}

	indentation = 0;
	rootNode = i->GetRootNode();
	
	if(!ProcessNode(rootNode)) {
		return(IMPEXP_FAIL);
	}

	status->WaitForClose();

	return(IMPEXP_SUCCESS);
}

bool Exporter::ProcessNode(INode *node)
{
	int numChildren;
	ObjectState os;

	os = node->EvalWorldState(0);
	
	if(os.obj != NULL) {
		SClass_ID superClass = os.obj->SuperClassID();

		switch(superClass) {
			case GEOMOBJECT_CLASS_ID:
				ProcessGeomObject(node, &os);
				break;
		}
	}


	// Recurse into child nodes 
	numChildren = node->NumberOfChildren();
	
	if(numChildren > 0) {
		for(int childNum = 0; childNum < numChildren; childNum++) {
			INode *childNode;
		
			childNode = node->GetChildNode(childNum);
			indentation++;
			ProcessNode(childNode);
			indentation--;
		}
	}

	return(true);
}

bool Exporter::ProcessGeomObject(INode *node, ObjectState *os)
{
	Object *object;
	Matrix3 *matrix;

	if(!node->Renderable())
		return(false);
	
	object = os->obj->ConvertToType(0, Class_ID(TRIOBJ_CLASS_ID, 0));
	if(object == NULL)
		return(false);

	matrix = os->GetTM();


	// Cleanup
	if(object != os->obj) {
		object->DeleteMe();
	}

	return(true);
}
/*
void OnExit(void *param, NotifyInfo *info)
{
	if(status != NULL) {
		delete status;
		status = NULL;
		inited = false;
	}
}
*/