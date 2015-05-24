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

#ifndef GROUP_WIDGET_HEADER
#define GROUP_WIDGET_HEADER

#include <osg/Group>
#include <cefix/AbstractWidget.h>
#include <cefix/PropertyList.h>

namespace cefix {
	
	/** exception-class, thrown from GroupWidget::get and ::find if the widget cwith the given name could not be found */
	class WidgetNotFoundException : public std::exception {
	public:
		WidgetNotFoundException(const std::string& widgetName) : std::exception(), _widgetName(widgetName) {}
		virtual ~WidgetNotFoundException() throw() {}
		virtual const char* what() const throw() {
			return std::string("Widget not found: " + _widgetName).c_str();
		}

		const std::string& widgetName() { return _widgetName; }
	private:
		std::string _widgetName;
	};


/** a group-widget groups a collection of widgets. YOu should not use a widget without adding it to a group, the group takes care of initializing the widget (calling update)
	Note: there is no graphical representation of a WidgetGroup, it is only a logical class. It adds all widgets to an osg::Group and return this osg::group as Node */

class CEFIX_EXPORT GroupWidget : public AbstractWidget {
	
	public:
		typedef std::map<std::string, osg::ref_ptr< AbstractWidget > > WidgetMap;
        typedef WidgetMap::iterator iterator;
        typedef WidgetMap::const_iterator const_iterator;
        
		/** ctor
		    @param identifier identiefier for this group 
		*/
		GroupWidget(const std::string& identifier) : 
			AbstractWidget(identifier), 
			_group(new osg::Group()),
			_firstRun(true) 
		{
			_group->setName(identifier+" Widget");
			// group->setUserData(this); // add a reference of this WidgetGroup to the group, so we don't get deleted;
		}
		
		GroupWidget(cefix::PropertyList* pl);
		
		~GroupWidget();
		
		/** adds a widget to the group, if a widget with the given identifier already exists it gets removed */
		void add(AbstractWidget* widget);
		
		/** @return true, if this group has a widget with given identifier */
		bool has(const std::string& identifier) {
			return (_map.find(identifier) != _map.end());
		}
		
		/** gets a widget with given identifier, does a dynamic-cast to the given typename */
		template<typename T> T get(const std::string& identifier, bool throwException = true) throw (WidgetNotFoundException) {
			WidgetMap::iterator i = _map.find(identifier);
			if (i != _map.end()) {
				return dynamic_cast<T>((*i).second.get());
			}
			if (throwException) 
				throw WidgetNotFoundException(identifier);
			return NULL;
		}
		
		/** find a widget in the hierarchy with specified identifier */
		template<typename T>T find(const std::string& identifier, bool throwException = true) throw (WidgetNotFoundException) {
			
			if (has(identifier))
				return get<T>(identifier);
			
			for(WidgetMap::iterator itr = _map.begin(); itr != _map.end(); ++itr) {
				GroupWidget* g = dynamic_cast<GroupWidget*>(itr->second.get());
				if (g) {
					T t = g->find<T>(identifier, false);
					if (t != NULL) return t;
				}
			}
			if (throwException)
				throw WidgetNotFoundException(identifier);

			return NULL;
		
		}
		
		/** remove widget */
		void remove(AbstractWidget* widget);
		
		/** remove widget with given identifier */
		void remove(std::string identifier);
		
		/** clear removes all children from this group */		
		void clear();
		
		/** update all widgets of this group */
		virtual void update();
		
		/** return the graphical representation for the scenegraph */
		virtual osg::Node* getNode();
		
		template<class T> void findOfType(std::vector<T*>& t_vec) {
			for (WidgetMap::iterator i = _map.begin(); i != _map.end(); ++i) {
				T* t= dynamic_cast<T*>((*i).second.get());
				if (t)
					t_vec.push_back(t);
			}
		}
		
		
		/// get the group node
		osg::Group* getGroupNode() { return _group.get(); }
		
		
		/// disable all childs
		void disableChilds() {
			for(WidgetMap::iterator i = _map.begin(); i != _map.end(); ++i) 
			{
				i->second->disable();
			}
		}
		
		/// enable all childs
		void enableChilds() {
			for(WidgetMap::iterator i = _map.begin(); i != _map.end(); ++i) 
			{
				i->second->enable();
			}
		}
        
        iterator begin() { return _map.begin(); }
        iterator end() { return _map.end(); }
        const_iterator begin() const { return _map.begin(); }
        const_iterator end() const { return _map.end(); }
		
	protected:
		osg::ref_ptr<osg::Group>	_group;
		WidgetMap	_map;
		bool		_firstRun;
		
	
};

} // end of namespace


#endif