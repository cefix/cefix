//
//  Sketch.h
//  cefix-for-iphone
//
//  Created by Stephan Huber on 01.12.11.
//  Copyright (c) 2011 Digital Mind. All rights reserved.
//

#pragma once

#include <cefix/Sketch.h>
#include <cefix/Quad2DGeometry.h>
#include <cefix/TextGeode.h>

class Sketch : public cefix::Sketch {
	public:
		Sketch();

        void init();
		void setup(); 
		void update();
    
        void createTouchTest();
        void createSoundTest();
		void debugTouches();
    
        void createDraggables();
	
	private:
		std::vector<cefix::Quad2DGeometry*> _quads;
        std::vector<cefix::Utf8TextGeode*> _tgs;
        
        osg::ref_ptr<cefix::AbstractWidget> _widgets;
};


