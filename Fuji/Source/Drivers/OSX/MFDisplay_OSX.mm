#include "Fuji.h"

#if MF_DISPLAY == MF_DRIVER_OSX

#include "MFDisplay_Internal.h"
#include "MFRenderer_Internal.h"
#include "MFView.h"
#include "MFSystem.h"
#include "DebugMenu.h"
#include "MFHeap.h"
#include <stdio.h>

#include <Cocoa/Cocoa.h>

@interface NSApplication (FujiApplication)
- (void) setAppleMenu:(NSMenu*) menu;
@end

@interface FujiApplication : NSObject
{
	NSString* _name;
	NSApplication* _app;
}

@property (readonly, copy, nonatomic) NSString* name;
@property (readonly, retain, nonatomic) NSApplication* app;

- (void) registerApp;
- (void) createWindow;
- (void) createApplicationMenus;
- (void) createWindowMenu;
- (void) run;

@end

static void MFDisplay_ResetDisplay();

void MFDisplay_DestroyWindow()
{
	MFCALLSTACK;
}

int MFDisplay_CreateDisplay(int width, int height, int bpp, int rate, bool vsync, bool triplebuffer, bool wide, bool progressive)
{
	MFCALLSTACK;

	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	FujiApplication* app = [[FujiApplication alloc] init];

	[app registerApp];
	[app createWindow];

	[app run];
	[pool release];

	return 0;
}

void MFDisplay_ResetDisplay()
{
	MFCALLSTACK;

	MFRenderer_ResetDisplay();
}

void MFDisplay_DestroyDisplay()
{
	MFCALLSTACK;

	FreeModes();
}

bool MFDisplay_SetDisplayMode(int width, int height, bool bFullscreen)
{
	// do any stuff the window manager needs to do to swap display modes or swap between window and fullscreen
	//...

	return MFRenderer_SetDisplayMode(width, height, bFullscreen);
}

void MFDisplay_GetNativeRes(MFRect *pRect)
{

}

void MFDisplay_GetDefaultRes(MFRect *pRect)
{

}

float MFDisplay_GetNativeAspectRatio()
{

}

bool MFDisplay_IsWidescreen()
{

}

void MFDisplay_HandleEventsX11()
{
	MFCALLSTACK;
}

@implementation FujiApplication

@synthesize app = _app;

- (NSString*) name
{
	if (_name != nil)
		return _name;

	pid_t pid = getpid();
	NSString* str = [NSString stringWithFormat:@"APP_NAME_%d", pid];
	char* name = getenv([str UTF8String]);

	if (name != NULL)
		_name = [NSString stringWithUTF8String:name];

	if (_name == nil || [_name length] == 0)
		_name = (NSString*) [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleName"];

	if (_name == nil || [_name length] == 0)
		_name = [[NSProcessInfo processInfo] processName];

	return _name;
}

- (void) registerApp
{
	ProcessSerialNumber psn;

	if (!GetCurrentProcess(&psn))
	{
		TransformProcessType(&psn, kProcessTransformToForegroundApplication);
		SetFrontProcess(&psn);
	}

	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	_app = [NSApplication sharedApplication];

	if ([self.app mainMenu] == nil)
		[self createApplicationMenus];

	if ([self.app delegate] == nil)
		[self.app setDelegate: nil];

	[pool release];
}

- (void) createWindow
{
	NSRect rect = NSMakeRect(500, 500, 500, 300);
	NSUInteger style = (NSTitledWindowMask|NSClosableWindowMask|NSMiniaturizableWindowMask|NSResizableWindowMask); // NSResizableWindowMask

	NSWindow* window = [[NSWindow alloc] initWithContentRect:rect styleMask:style backing:NSBackingStoreBuffered defer:NO];
	[window makeKeyAndOrderFront:nil];
}

- (void) createApplicationMenus
{
	NSMenu* appleMenu = [[NSMenu alloc] initWithTitle: @""];
	NSMenu* serviceMenu;
	NSMenuItem* menuItem;
	NSString* title;

	[self.app setMainMenu:[[NSMenu alloc] init]];

	title = [@"About " stringByAppendingString:self.name];
	[appleMenu addItemWithTitle:title action:@selector(orderFrontStandardAboutPanel:) keyEquivalent:@""];
	[appleMenu addItem:[NSMenuItem separatorItem]];

	[appleMenu addItemWithTitle:@"Preferences…" action:nil keyEquivalent:@""];
	[appleMenu addItem:[NSMenuItem separatorItem]];

	serviceMenu = [[NSMenu alloc] initWithTitle:@""];
	menuItem = [appleMenu addItemWithTitle:@"Services" action:nil keyEquivalent:@""];
	[menuItem setSubmenu:serviceMenu];
	[serviceMenu release];
	[self.app setServicesMenu:serviceMenu];
	[appleMenu addItem:[NSMenuItem separatorItem]];

	title = [@"Hide " stringByAppendingString:self.name];
	[appleMenu addItemWithTitle:title action:@selector(hide:) keyEquivalent:@"h"];

	menuItem = (NSMenuItem*) [appleMenu addItemWithTitle:@"Hide Others" action:@selector(hideOtherApplications:) keyEquivalent:@"h"];
	[menuItem setKeyEquivalentModifierMask:(NSAlternateKeyMask|NSCommandKeyMask)];

	[appleMenu addItemWithTitle:@"Show All" action:@selector(unhideAllApplications:) keyEquivalent:@""];
	[appleMenu addItem:[NSMenuItem separatorItem]];

	title = [@"Quit " stringByAppendingString:self.name];
	[appleMenu addItemWithTitle:title action:@selector(terminate:) keyEquivalent:@"q"];

	menuItem = [[NSMenuItem alloc] initWithTitle:@"" action:nil keyEquivalent:@""];
	[menuItem setSubmenu:appleMenu];

	[[self.app mainMenu] addItem:menuItem];
	[menuItem release];

	[self.app setAppleMenu:appleMenu];
	[appleMenu release];

	[self createWindowMenu];
}

- (void) createWindowMenu
{
	NSMenu* windowMenu = [[NSMenu alloc] initWithTitle:@"Window"];

	[windowMenu addItemWithTitle:@"Minimize" action:@selector(performMiniaturize:) keyEquivalent:@"m"];
	[windowMenu addItemWithTitle:@"Zoom" action:@selector(performZoom:) keyEquivalent:@""];

	NSMenuItem* menuItem = [[NSMenuItem alloc] initWithTitle:@"Window" action:nil keyEquivalent:@""];
	[menuItem setSubmenu:windowMenu];
	[[self.app mainMenu] addItem:menuItem];
	[menuItem release];

	[self.app setWindowsMenu:windowMenu];
	[windowMenu release];
}

- (void) run
{
	[self.app run];
}

@end

#endif
