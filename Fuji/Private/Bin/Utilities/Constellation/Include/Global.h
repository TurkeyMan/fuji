#if !defined(_GLOBAL_H)
#define _GLOBAL_H

#define _WIN32_WINNT 0x0500

#include "Types.h"
#include "max.h"
#include "MAX_Mem.h"
#include "Res.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define MAX(a, b) ((a) > (b)? (a) : (b))
#define MIN(a, b) ((a) < (b)? (a) : (b))

// *** Globals ***
#define STATUS_MAX_LINES 500

#define PLUGIN_VERSION 100
#define PLUGIN_CLASS_ID Class_ID(0x20582baa, 0x4fef129f)
#define PLUGIN_CLASS_NAME _T("constel")
#define FILE_EXTENSION _T("CNS")
#define SHORT_FF_DESCRIPTION _T("Constellation")
#define LONG_FF_DESCRIPTION _T("Constellation Object Format")
#define PLUGIN_DESCRIPTION _T("Constellation Format Exporter")
#define AUTHOR_NAME _T("Lewis Jackson")
#define COPYRIGHT _T("Copyright © 2004 DotBlip")
#define SCENEEXPORT_CATEGORY _T("Scene Export")

extern HINSTANCE hInstance;

// *** Prototypes ***

#endif