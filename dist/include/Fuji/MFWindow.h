#if !defined(_MFWINDOW_H)
#define _MFWINDOW_H

struct MFDisplay;

struct MFWindow;

enum MFWindowFlags
{
	MFWF_WindowFrame = MFBIT(0),
	MFWF_CanResize = MFBIT(1),
	MFWF_AlwaysOnTop = MFBIT(2),

	MFWF_Max,
	MFWF_ForceInt = 0x7FFFFFFF
};

struct MFWindowParams
{
	const char *pWindowTitle;

	int x;
	int y;
	int width;
	int height;

	uint32 flags;	// MFWindowFlags

	int monitor;

	bool bFullscreen;
};

MF_API MFWindow* MFWindow_Create(MFWindowParams *pParams);
MF_API MFWindow* MFWindow_BindExisting(void *pWindowHandle);
MF_API void MFWindow_Destroy(MFWindow *pWindow);

MF_API void MFWindow_Update(MFWindow *pWindow, const MFWindowParams *pParams);

MF_API const MFWindowParams* MFWindow_GetWindowParameters(const MFWindow *pWindow);
MF_API MFDisplay* MFWindow_GetDisplay(MFWindow *pWindow);
MF_API void* MFWindow_GetSystemWindowHandle(MFWindow *pWindow);

#endif // _MFWINDOW_H
