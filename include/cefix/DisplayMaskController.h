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

#ifndef DISPLAY_MASK_CONTROLLER_HEADER
#define DISPLAY_MASK_CONTROLLER_HEADER
#include <string>

#include <osg/ref_ptr>
#include <osg/Referenced>
#include <osg/Vec4>
#include <osg/Group>

namespace osg {
class Image;
class Geode;
class Node;
}


namespace cefix {

class DisplayMask;
class AbstractWidget;
class Quad2DGeometry; 
class Serializer;

/** a displayMaskController provides an interface to interact with an display mask. 
 *  A display mask is one or more splines defining the visible areas for a given display
 *  the DisplayMaskController can provide an editing-interface to edit the splines
 */
class DisplayMaskController : public osg::Referenced {
public:
	DisplayMaskController();
	
	/// set to true, to show the editing interface
	void setEditable(bool e);
	/// returns true, if the display mask is editable 
	bool isEditable() const { return _editable; }
	
	/// gets or creates the display mask with a given width and height
	osg::Node* getOrCreateDisplayMask(unsigned int w, unsigned int h) { if (!_group.valid()) createMask(w,h); return _group.get(); }
	
	/// sets the mask-color
	void setMaskColor(const osg::Vec4&c);
	
	/// sets the mask-alpha
	void setMaskAlpha(float a);
	
	
	/// adds a new spline to the mask
	void createNewSpline();
	
	/// removes the current selected spline from the mask
	void deleteSelectedSpline();
	
	/// delete the current selected point of a spline
	void deleteControlPoint();
	
	void readFrom(cefix::Serializer& serializer);
	void writeTo(cefix::Serializer& serializer);

	/// load the mask from a file
	void load(const std::string& filename);
	
	/// save the mask to a file
	void save(const std::string& filename);
	
	void setFileName(const std::string& filename) { _filename = filename; }
	const std::string& getFileName() { return _filename; }

	void exitEditMode();
protected:
	void createMask(unsigned int w, unsigned int h);
	void createWidgets();
	~DisplayMaskController();
private:
	osg::ref_ptr<osg::Geode>				_maskGeode;
	osg::ref_ptr<cefix::AbstractWidget>		_widgets;
	osg::ref_ptr<cefix::DisplayMask>		_displayMask;
	osg::ref_ptr<osg::Image>				_maskImage;
	osg::ref_ptr<osg::Group>				_group;
	bool									_editable;
	osg::ref_ptr<cefix::Quad2DGeometry>		_maskGeometry;
	std::string								_filename;

};
}

#endif