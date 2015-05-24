/*
 *  TabbedTextRenderIterator.cpp
 *  AudiParis
 *
 *  Created by Stephan Huber on 25.07.06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include <cefix/TabbedTextRenderIterator.h>
#include <cefix/FontCharData.h>

namespace cefix {

// --------------------------------------------------------------------------------------------------------------------
// advance
// --------------------------------------------------------------------------------------------------------------------

void TabbedTextRenderIterator::advance(const unsigned int &left, const unsigned int &right) {

	switch (left) {
		case '\t':
			
			if (_tablist.size() <= _currentRenderTabIndex)
				break;
				
			switch(_tablist[_currentRenderTabIndex].second) {
				case TAB_LEFT:
					_position[0] = _leftMargin + _tablist[_currentRenderTabIndex].first;
					break;
					
				case TAB_RIGHT:
					_position[0] = _leftMargin + _tablist[_currentRenderTabIndex].first - _tabbedTextWidths[_currentLine][ _currentRenderTabIndex + 1];
					break;
				
				case TAB_CENTER:
					_position[0] = _leftMargin + _tablist[_currentRenderTabIndex].first - _tabbedTextWidths[_currentLine][ _currentRenderTabIndex + 1] / 2.0f;
					break;
			}
			_currentRenderTabIndex++;
			break;
		
		case '\n':
			_currentRenderTabIndex = 0;
			//durchfallen lassen
		default:
			cefix::AttributedTextRenderIterator::advance(left, right);
			break;
	}

}


// --------------------------------------------------------------------------------------------------------------------
// preflight
// --------------------------------------------------------------------------------------------------------------------

void TabbedTextRenderIterator::preflight(const unsigned int &left, const unsigned int &right) {
	
	cefix::AttributedTextRenderIterator::preflight(left, right);
	
	switch (left) {
		case '\n': 
			{
				
				/*
				if (_tabCurrentLine >= 0) {
					std::cout << "line " << (_tabCurrentLine + 1) << ":  |";
					for(std::vector<float>::iterator i = _tabbedTextWidths[_tabCurrentLine].begin(); i != _tabbedTextWidths[_tabCurrentLine].end(); i++) {
						std::cout << (*i) << "|";
					}
					std::cout << std::endl;
				}
				*/
				
				_currentTabIndex = 0 ;
				std::vector<float> widths;
				widths.push_back(0);
				_tabbedTextWidths.push_back(widths);
				_tabCurrentLine++;
			}
			break;
			
		case '\t':
			_currentTabIndex++;
			_tabbedTextWidths[_tabCurrentLine].push_back(0);
			break;
		
		default:
			if (isAttributedChar(left,true) == false)
				_tabbedTextWidths[_tabCurrentLine][ _currentTabIndex ] += _font->getCharWidth(left,right) + _spatio;
			break;
	}

}


osg::Geometry* TabbedTextRenderIterator::createDebugGeometry() 
{
	osg::Geometry* geo = new osg::Geometry();
	osg::Vec3Array* vertices = new osg::Vec3Array();
	osg::Vec4Array* colors = new osg::Vec4Array();
	
	for (TabMap::iterator i = _tabs.begin(); i != _tabs.end(); ++i) 
	{
		switch (getRenderPlane()) 
		{
			case FontCharData::NEGATIVE_XY_PLANE:
            case FontCharData::XY_PLANE:
				vertices->push_back(_startPos + osg::Vec3(i->first, 20, 0));
				vertices->push_back(_startPos + osg::Vec3(i->first, -100, 0));
				break;
			
			case FontCharData::NEGATIVE_XZ_PLANE:
			case FontCharData::XZ_PLANE:
				vertices->push_back(_startPos + osg::Vec3(i->first, 0, 20));
				vertices->push_back(_startPos + osg::Vec3(i->first, 0, -100));
				break;
			
			case FontCharData::NEGATIVE_YZ_PLANE:
			case FontCharData::YZ_PLANE:
				vertices->push_back(_startPos + osg::Vec3(0, i->first, 20));
				vertices->push_back(_startPos + osg::Vec3(0, i->first, -100));
				break;
		}
		
		osg::Vec4 c;
		
		switch(i->second) {
			case TAB_LEFT:
				c = osg::Vec4(1,0,0,1);
				break;
			case TAB_CENTER:
				c = osg::Vec4(0,1,0,1);
				break;
			case TAB_RIGHT:
				c = osg::Vec4(0,0,1,1);
				break;
		}
		colors->push_back(c);
		colors->push_back(c);

	}
	
	geo->setVertexArray(vertices);
	geo->setColorArray(colors);
	geo->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	geo->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, vertices->size()));
	geo->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	return geo;
	
}



}


