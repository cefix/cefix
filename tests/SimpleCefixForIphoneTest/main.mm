//
//  main.m
//  iphoneViewer
//
//  Created by Thomas Hogarth on 10/05/2009.
//  Copyright HogBox 2009. All rights reserved.
//


#import <UIKit/UIKit.h>
#include <cefix/Sketch.h>

int main(int argc, char *argv[]) {
    cefix::Sketch::setApplicationPath(argv[0]);
	NSAutoreleasePool *pool = [NSAutoreleasePool new];
	UIApplicationMain(argc, argv, nil, @"osgAppDelegate");
	[pool release];
	return 0;
}
