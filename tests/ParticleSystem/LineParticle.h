/*
 *  LineParticle.h
 *  ForceFields
 *
 *  Created by Stephan Huber on 06.04.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef LINEPARTICLE
#define LINEPARTICLE

#include <osg/Array>
#include <cefix/Particle.h>


class LineParticle : public cefix::Particle
{
public:
	LineParticle(unsigned int verticescount = 10);
	
	osg::Vec3Array* getVertexArray() { return _linestrip.get();}
	
	void setVertexCount(unsigned int c) { _linestrip->resize(0); _verticesCount = c; }
		
	void updateRepresentation() 
	{
		osg::Vec3 d = (getPosition() - _lastAdded);
		if (d.length() > 0.005f) {
			_lastAdded = getPosition();
			_linestrip->push_back(getPosition());
			if (_linestrip->size() > _verticesCount)
				_linestrip->erase(_linestrip->begin());
			else
				while(_linestrip->size() < _verticesCount)
					_linestrip->push_back(getPosition());
		}
		
	}
	
	
	virtual ~LineParticle() {}

private:
	unsigned int _verticesCount;
	osg::ref_ptr<osg::Vec3Array>	_linestrip;
	osg::Vec3 _lastAdded;

};


#endif