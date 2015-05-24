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

#ifndef CEFIX_THEMED_WIDGET_STATE_GEOMETRY_HEADER
#define CEFIX_THEMED_WIDGET_STATE_GEOMETRY_HEADER

#include <cefix/Resizable9x9TexturedGeometry.h>

namespace cefix {

	class ThemedWidgetStateGeometry: public Resizable9x9TexturedGeometry {
	protected:
		struct State {
			osg::Vec4 rect;
			float frameWidth, frameHeight; 
			State(const osg::Vec4& in_rect, float fw, float fh) : rect(in_rect), frameWidth(fw), frameHeight(fh) {}
			State()  {}
		};
			
	public:
		enum 
		{
			NORMAL_DISABLED = 1000,
			SELECTED_DISABLED,
			NORMAL,
			SELECTED,
			NORMAL_DOWN,
			SELECTED_DOWN,
			NORMAL_OVER,
			SELECTED_OVER,
			UNKNOWN = 0xFFFF
		} StateNames;
		
		typedef std::map<unsigned int, State> States;
		ThemedWidgetStateGeometry();
		
		void clearStates() { _states.clear(); _lastState = UNKNOWN; }
		
		void addState(unsigned int state, const osg::Vec4& rect, float bw, float bh)
		{
			_states[state] = State(rect, bw, bh);
		}
		
		virtual void setState(unsigned int s) {
			
			if (1 || (s != _lastState)) {
				States::iterator itr = _states.find(s);
				if (itr != _states.end()) {
					setFrameWidthAndHeight(itr->second.frameWidth, itr->second.frameHeight);
					setTexFrameWidthAndHeight(itr->second.frameWidth, itr->second.frameHeight);
					setTextureRect(itr->second.rect);
					//std::cout << getName() << ": " << s << " r: " << itr->second.rect << std::endl;
				}
				_lastState = s; 
			}
		}
		
		unsigned int getState() { return _lastState;}
		
		bool hasState(unsigned int s) { return _states.find(s) != _states.end(); }
				
		unsigned int getNumStates() { return _states.size(); }
		
		void setPositionAndSize(const osg::Vec3& p, const osg::Vec2& s) {
			setRect(p[0], p[1], s[0], s[1]);
			setLocZ(p[2]);
		}
		
		void addMissingStates();
		
	private:
		States _states;
		unsigned int _lastState;
	};
	
}


#endif