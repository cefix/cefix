//Created by Thomas Hogarth 2009

#import "osgAppDelegate.h"
#include <osgGA/TrackballManipulator>
#include <osg/ShapeDrawable>

#define kAccelerometerFrequency		30.0 // Hz
#define kFilteringFactor			0.1

@implementation osgAppDelegate

//
//Called once app has finished launching, create the viewer then realize. Can't call viewer->run as will 
//block the final inialization of the windowing system
//
- (void)applicationDidFinishLaunching:(UIApplication *)application {
    
    osg::setNotifyLevel(osg::INFO);
    
    _sketch = new Sketch();  
    _sketch->prepareLaunch();
	//osg::setNotifyLevel(osg::DEBUG_FP);
	
	_displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(updateScene:)];
	[_displayLink setFrameInterval:1];
	[_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];  
}


//
//Timer called function to update our scene and render the viewer
//
- (void)updateScene:(CADisplayLink *)sender {
	_sketch->perFrame();
}


- (void)applicationWillResignActive:(UIApplication *)application {

}


- (void)applicationDidBecomeActive:(UIApplication *)application {

}


-(void)applicationWillTerminate:(UIApplication *)application{
    [_displayLink invalidate];
	_sketch = NULL;
} 

//
//Accelorometer
//
- (void)accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration
{
	//Use a basic low-pass filter to only keep the gravity in the accelerometer values
	accel[0] = acceleration.x * kFilteringFactor + accel[0] * (1.0 - kFilteringFactor);
	accel[1] = acceleration.y * kFilteringFactor + accel[1] * (1.0 - kFilteringFactor);
	accel[2] = acceleration.z * kFilteringFactor + accel[2] * (1.0 - kFilteringFactor);
}


- (void)dealloc {
	_sketch = NULL;
	[super dealloc];
}

@end
