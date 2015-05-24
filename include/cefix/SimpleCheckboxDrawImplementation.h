/*
 *
 *      (\(\
 *     (='.')           cefix --
 *    o(_")")           a multipurpose library on top of OpenSceneGraph
 *  
 *
 *                      Copyright 2001-2011, stephanmaximilianhuber.com 
 *                      Stephan Maximilian Huber. 
 *
 *                      All rights reserved.
 *
 */
#ifndef SIMPLE_CHECKBOX_DRAWIMPLEMENTATION
#define SIMPLE_CHECKBOX_DRAWIMPLEMENTATION

#include <cefix/SimpleTextButtonDrawImplementation.h>

namespace cefix {
class CheckboxWidget;

class CEFIX_EXPORT SimpleCheckboxDrawImplementation : public SimpleTextButtonDrawImplementation {

	public: 
		SimpleCheckboxDrawImplementation(const osg::Vec3& pos, const osg::Vec2&size);
		
		void update(CheckboxWidget* widget);
		
		void applyPropertyList(cefix::PropertyList* pl);
	
	protected:
		osg::ref_ptr<osg::Geode>		_check;

};

}


#endif