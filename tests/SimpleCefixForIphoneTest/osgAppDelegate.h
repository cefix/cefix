//Created by Thomas Hogarth 2009

//force the link to our desired osgPlugins
#include "osgPlugins.h"

#include "Sketch.h"


#import <UIKit/UIKit.h>
#include <QuartzCore/QuartzCore.h>


@interface osgAppDelegate : NSObject <UIApplicationDelegate, UIAccelerometerDelegate> {

	UIAccelerationValue		accel[3];
	
	osg::ref_ptr<cefix::Sketch> _sketch;
    
    CADisplayLink* _displayLink;

}

- (void)updateScene: (CADisplayLink *)sender;

@end

