//
//  main.c
//  cefixTemplate
//
//  Created by Stephan Huber on 26.11.06.
//  Copyright __MyCompanyName__ 2006. All rights reserved.
//

#include "Sketch.h"


int main(int argc, char* argv[])
{
	Sketch::setApplicationPath(argv[0]);
	osg::ref_ptr<Sketch> app = new Sketch();
	return app->run();
}


