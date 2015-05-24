/*
 *  ImageFactory_test.cpp
 *  SimpleGLUTViewer
 *
 *  Created by Stephan Huber on Tue Nov 18 2003.
 *  Copyright (c) 2003 __MyCompanyName__. All rights reserved.
 *
 */
 
#define __USE_OSX_AGL_IMPLEMENTATION__

#include <cefix/Log.h>
#include <osg/Group>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osg/Material>
#include <osg/Depth>
#include <osg/Stateset>



#include <cefix/AppController.h>
#include <cefix/PostScriptWriter.h>





int main(int argc, char *argv[])
{
	osg::setNotifyLevel(osg::INFO);
	// Model laden
#ifdef __APPLE__
    osg::Node* n = osgDB::readNodeFile("ich.osg");
#else
	osg::Node* n = osgDB::readNodeFile("../../resources/cow.osg");
#endif

	osg::ref_ptr<cefix::AppController> app = new cefix::AppController();
	app->setUseOptimizerFlag(false);
	app->applyWorld(n);
	app->realize();
	app->requestPostScriptWriter();
	
	app->run();
	app = NULL;
		
	return 0;
}