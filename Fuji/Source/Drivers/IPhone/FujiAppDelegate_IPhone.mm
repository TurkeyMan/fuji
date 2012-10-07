#import "FujiAppDelegate_IPhone.h"
#import "FujiView_IPhone.h"

#include "Fuji.h"
#include "MFSystem_Internal.h"
#include "MFInput_IPhone.h"

int StartIPhone(MFInitParams *pInitParams)
{
	NSAutoreleasePool * pool = [NSAutoreleasePool new];
	
	int ret = UIApplicationMain(pInitParams->argc, (char**)pInitParams->argv, nil, @"FujiAppDelegate");
	
	[pool release];
	
	return ret;
}

@implementation FujiAppDelegate

@synthesize fujiWindow;
@synthesize fujiView;

- (void)applicationDidFinishLaunching:(UIApplication *)application {
	// Create window
	self.fujiWindow = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];

	// set up content view
	// mode: window with status bar
	//self.fujiView = [[[FujiView alloc] initWithFrame:[[UIScreen mainScreen] applicationFrame]] autorelease];
	// mode: full screen
	//self.fujiView = [[[FujiView alloc] initWithFrame:[[UIScreen mainScreen] bounds]] autorelease];
	UIView * glview = [[FujiView alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
	[fujiWindow addSubview:glview];

	// show window
	[fujiWindow makeKeyAndVisible];

	timer = [NSTimer scheduledTimerWithTimeInterval:1.0/30 target:self selector:@selector(onTimer:) userInfo:nil repeats:YES];

	// configure and start accelerometer
	[[UIAccelerometer sharedAccelerometer] setUpdateInterval:(1.0 / 10)];
	[[UIAccelerometer sharedAccelerometer] setDelegate:self];

	// init the rest of Fuji
	MFSystem_Init();

	// allow the app to init
	MFSystemCallbackFunction pInit = MFSystem_GetSystemCallback(MFCB_InitDone);
	if(pInit)
		pInit();
	
	// prime the time delta
	MFSystem_UpdateTimeDelta();
}

- (void)applicationWillTerminate:(UIApplication *)application {
	// allow the app to deinit
	MFSystemCallbackFunction pDeinit = MFSystem_GetSystemCallback(MFCB_Deinit);
	if(pDeinit)
		pDeinit();

	// deinit Fuji
	MFSystem_Deinit();
}

- (void)dealloc {
	[fujiView release];
	[fujiWindow release];
	[super dealloc];
}

- (void) accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration {
	MFInputIPhone_SetAcceleration(acceleration.x, acceleration.y, acceleration.z);
}

- (void) onTimer:(NSTimer*)timer {
	MFSystem_RunFrame();
}

@end
