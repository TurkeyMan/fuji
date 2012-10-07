#import <UIKit/UIKit.h>

@class FujiView;

@interface FujiAppDelegate : NSObject  <UIApplicationDelegate, UIAccelerometerDelegate>
{
	UIWindow *fujiWindow;
	FujiView *fujiView;
	NSTimer *timer;
}

@property (nonatomic, retain) IBOutlet UIWindow *fujiWindow;
@property (nonatomic, retain) IBOutlet FujiView *fujiView;

- (void) onTimer:(NSTimer*)timer;

@end
