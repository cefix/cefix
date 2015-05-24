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

#ifndef TABBED_TEXT_RENDER_ITERATOR_HEADER_
#define TABBED_TEXT_RENDER_ITERATOR_HEADER_

#include <cefix/AttributedTextRenderIterator.h>
#include <cefix/Export.h>

namespace cefix {

/** this class extends AttributedTextRenderIterator to implement tab-aligned text */
class CEFIX_EXPORT TabbedTextRenderIterator : public cefix::AttributedTextRenderIterator {

	public:
		enum TabAlignment { TAB_LEFT, TAB_CENTER, TAB_RIGHT };
		typedef std::map<float, TabAlignment> TabMap;
		typedef std::vector< std::pair<float, TabAlignment> > TabList;
		
		/**ctor*/
		TabbedTextRenderIterator() : cefix::AttributedTextRenderIterator(), _tabs() {}
	
		TabbedTextRenderIterator(const TabbedTextRenderIterator& itr)
		:	cefix::AttributedTextRenderIterator(),
			_tabs(itr._tabs),
			_tablist(itr._tablist)
		{
		}
		
		/** clear all tabs */
		void clearTabs() { _tabs.clear(); }
		
		/** @return tab at pos */
		TabAlignment getTabAt(float pos) { return _tabs.find(pos)->second; }
		
		
		/** @return true if there is an tab at pos */
		bool hasTabAt(float pos) { return (_tabs.find(pos) != _tabs.end()); }
		
		/** set a tab at position with alignment align */
		void setTabAt(float pos, TabAlignment align) { 
			_tabs[pos] = align; 
		}
		
		/** this iterator needs preflight before rendering */
		virtual bool needsPreflight() { return 1; /*(_tabs.size() > 0);*/ }
		
		virtual void preflight(const unsigned int& left_ch, const unsigned int& right_ch);
		virtual void advance(const unsigned int &left, const unsigned int &right);
		
		virtual void reset() {
			cefix::AttributedTextRenderIterator::reset();
			_tabbedTextWidths.clear();
			
			std::vector<float> widths;
			widths.push_back(0);
			_tabbedTextWidths.push_back(widths);
			
			_currentTabIndex = 0;
			_tabCurrentLine = 0;
			_currentRenderTabIndex = 0;
			
			_tablist.clear();
			for(TabMap::iterator i = _tabs.begin(); i != _tabs.end(); i++) {
				_tablist.push_back(std::pair<float, TabAlignment>(i->first, i->second));
			}
		}
		
		osg::Geometry* createDebugGeometry();
		
	
	protected:
		
		virtual ~TabbedTextRenderIterator() {}
		
		TabMap	_tabs;
		TabList	_tablist;
		std::vector< std::vector<float> > _tabbedTextWidths;
		unsigned int _currentTabIndex, _tabCurrentLine, _currentRenderTabIndex; 
		

};

}

#endif