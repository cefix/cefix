/*
 *  DisplayMask.h
 *  DisplayMaskEditor
 *
 *  Created by Stephan Huber on 10.08.09.
 *  Copyright 2009 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#ifndef DISPLAY_MASK_HEADER
#define DISPLAY_MASK_HEADER

#include "EditableSpline.h"

namespace cefix {
class Serializer;


class DisplayMask : public osg::Referenced {

public:
	DisplayMask(bool editable = false);
	
	void resetSelection() {
		for(std::vector< osg::ref_ptr<EditableSpline> >::iterator i = _splines.begin(); i!=_splines.end(); ++i) {
			(*i)->setSelected(false);
		}
	}
	
	void readFrom(cefix::Serializer& serializer);
	void writeTo(cefix::Serializer& serializer);
	
	void setDimensions(unsigned int w, unsigned int h) { _width = w; _height = h; _backdrop->setRect(0,0, w,h);}
	
	void newSpline(const osg::Vec3& center);
	
	void removeSelectedSpline() 
	{
		if (!_editable) return;
		for(std::vector< osg::ref_ptr<EditableSpline> >::iterator i = _splines.begin(); i!=_splines.end(); ++i) {
			if ((*i)->isSelected()) {
				_childs->removeChild(*i);
				_splines.erase(i);
				return;
			}
		}
	}
	
	osg::Node* getNode() { return _group.get(); }
	
	~DisplayMask();
	
	void setEditable(bool e) {
		_editable = e;
		for(std::vector< osg::ref_ptr<EditableSpline> >::iterator i = _splines.begin(); i!=_splines.end(); ++i) {
			(*i)->setEditable(e);
		}
		
		_backdropGeode->setNodeMask(0xffff * e);
	}
	
	void removeSelectedControlPoint() {
		if (!_editable) return;
		for(std::vector< osg::ref_ptr<EditableSpline> >::iterator i = _splines.begin(); i!=_splines.end(); ++i) {
			if ((*i)->isSelected()) {
				(*i)->removeSelectedControlPoint();
			}
		}
	}
	
	void hilightNearestPoint(const osg::Vec3 p) 
	{
		if (!_editable) return;
		for(std::vector< osg::ref_ptr<EditableSpline> >::iterator i = _splines.begin(); i!=_splines.end(); ++i) {
			if ((*i)->isSelected()) {
				(*i)->hilightNearestPoint(p, false);
			}
		}
	}
	
	void createNewControlPoint(const osg::Vec3 p) {
		if (!_editable) return;
		for(std::vector< osg::ref_ptr<EditableSpline> >::iterator i = _splines.begin(); i!=_splines.end(); ++i) {
			if ((*i)->isSelected()) {
				(*i)->hilightNearestPoint(p, true);
			}
		}
	}
	
	void setInverted(bool b)  { _inverted = b;}
	bool getInverted() const { return  _inverted;}
	
	void setDoBlur(bool b)  { _blur = b;}
	bool getDoBlur() const { return _blur;}
	
	void updateDisplayMaskInto(osg::Image* img);
	
	unsigned int getWidth() const {return _width; }
	unsigned int getHeight() const { return _height; }
	
	void setWidth(unsigned int w) { _width = w; }
	void setHeight(unsigned int h) { _height = h; }

	osg::Group* getSplineGroup() { return _childs.get(); }
private:
	std::string			_filename;
	bool				_editable;
	unsigned int		_width, _height;
	std::vector< osg::ref_ptr<EditableSpline> > _splines;
	osg::ref_ptr<osg::Group>    _group, _childs;
	bool				_inverted, _blur;
	osg::ref_ptr<cefix::Quad2DGeometry> _backdrop;
	osg::ref_ptr<osg::Geode>			_backdropGeode;
	unsigned int						_multiplier;

friend class DisplayMaskController;
	
};

}

#endif