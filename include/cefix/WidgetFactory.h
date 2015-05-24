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

#ifndef WIDGET_FACTORY_HEADER
#define WIDGET_FACTORY_HEADER

#include <osg/Referenced>
#include <cefix/ObjectFactory.h>
#include <cefix/AbstractWidget.h>
#include <cefix/PropertyList.h>
#include <cefix/Log.h>

namespace cefix {

class CEFIX_EXPORT WidgetFactory : public osg::Referenced {

	public:
		class ResponderMap  : public std::multimap<std::string, osg::ref_ptr< cefix::AbstractWidget::Responder > > 
		{
		public:
			template<class WidgetClass, class T, typename U> void add(const std::string& responder, T* t, U(T::*getter)(), void(T::*setter)(const U& u) )
			{
				insert(std::make_pair(responder, new cefix::AbstractWidget::GetterSetterResponder<WidgetClass, T, U>(t, getter, setter) ));
			}
			
			template<class WidgetClass, class T, typename U> void add(const std::string& responder, T* t, U(T::*getter)() const, void(T::*setter)(const U& u) )
			{
				insert(std::make_pair(responder, new cefix::AbstractWidget::GetterSetterResponder<WidgetClass, T, U>(t, getter, setter) ));
			}
			
			template<class WidgetClass, class T, typename U> void add(const std::string& responder, T* t, U(T::*getter)(), void(T::*setter)(U u) )
			{
				insert(std::make_pair(responder, new cefix::AbstractWidget::GetterSetterResponder<WidgetClass, T, U>(t, getter, setter) ));
			}

			template<class WidgetClass, class T, typename U> void add(const std::string& responder, T* t, U(T::*getter)() const, void(T::*setter)(U u) )
			{
				insert(std::make_pair(responder, new cefix::AbstractWidget::GetterSetterResponder<WidgetClass, T, U>(t, getter, setter) ));
			}

			
			template<class WidgetClass, class T, typename U> void add(const std::string& responder, T* t, void(T::*setter)(const U& u) )
			{
				insert(std::make_pair(responder, new cefix::AbstractWidget::GetterSetterResponder<WidgetClass, T, U>(t, setter) ));
			}
			
			template<class WidgetClass, class T, typename U> void add(const std::string& responder, T* t, void(T::*setter)(U u) )
			{
				insert(std::make_pair(responder, new cefix::AbstractWidget::GetterSetterResponder<WidgetClass, T, U>(t, setter) ));
			}
			
			template<class T> void add(const std::string& responder, T* t, void(T::*fpt)(const std::string& action, AbstractWidget*)) {
				insert(std::make_pair(responder, new AbstractWidget::FunctorResponderBridge<T>(t, fpt) ));
			}
			
			template<class WidgetClass, typename T> void add(const std::string& responder, T* t)
			{
				insert(std::make_pair(responder, new cefix::AbstractWidget::ChangeReferenceResponder<WidgetClass, T>(t) ));
			}
			
			void add(const std::string& responder, AbstractWidget::Responder* t)
			{
				insert(std::make_pair(responder, t));
			}


			
		};

		
		
		/** helper object for the factory */
		class WidgetCreatorBase : public osg::Referenced {
			public:
				virtual AbstractWidget* operator()(cefix::PropertyList* pl) const { return NULL; }
				virtual ~WidgetCreatorBase() {};
		};

		/** helper template for the factory */
		template<class T>class WidgetCreatorBaseT : public WidgetCreatorBase {
			public: 
				virtual AbstractWidget* operator()(cefix::PropertyList* pl) const { return new T(pl); }
				virtual ~WidgetCreatorBaseT() {}
		};
		
		/** helper class to register Widgets from their cpp-file */
		template <class T> class RegisterProxy {
		
			public:
				RegisterProxy(const std::string& key, const std::string& requiredKeys) { cefix::WidgetFactory::instance()->registerWidgetClass(key, new WidgetCreatorBaseT<T>(), requiredKeys); }
				
		};
		/** ctor */
		WidgetFactory();
		
		/** get the singleton's instance */
		static WidgetFactory* instance();
		
		/** create a hierarchiy of widgets, which are described by a propertylist p, applying a map of responder to the widgets.
		 *  Most of the time you'll get the abstractProperty from a xml-file or a subtree of a xml-file, 
		 * see https://wiki.cefix.org/doku.php?id=widgetfactory for more details 
		*/
		cefix::AbstractWidget* create(cefix::AbstractProperty* p, cefix::AbstractWidget::Responder* responder = NULL);
				
		/** create a hierarchiy of widgets, which are deseibed by a propertylist p, applying a map of responder to the widgets.
		 *  Most of the time you'll get the abstractProperty from a xml-file or a subtree of a xml-file, 
		 * see https://wiki.cefix.org/doku.php?id=widgetfactory for more details 
		*/
		cefix::AbstractWidget* create(cefix::AbstractProperty* p,  ResponderMap& respondermap);
				
		cefix::AbstractWidget* createFromPrefsKey(const std::string& prefsKey, ResponderMap& respondermap);
		cefix::AbstractWidget* createFromXMLFile(const std::string& filename, ResponderMap& respondermap);
		
		
		
		/** register a Widget-class with a space-separated list of needed xml-tags (requiredKeys), called by a RegisterProxy-class */
		void registerWidgetClass(const std::string& key, WidgetCreatorBase* creator, const std::string& requiredKeys) {
			_factory->registerObjectClass(strToLower(key), creator);
			_requiredKeysMap[strToLower(key)] = requiredKeys;
		}
		
		
		/** register a Widget-class with a space-separated list of needed xml-tags (requiredKeys) */
		template<class WidgetClass>
		void registerWidgetClass(const std::string& key, const std::string& requiredKeys) {
			registerWidgetClass(key, new WidgetCreatorBaseT<WidgetClass>(), requiredKeys);
		}
		
		/** deregister a Widget-class */
		void unregisterWidgetClass(const std::string& key) {
			_factory->unregisterObjectClass(strToLower(key));
		}
		
		
		void setWidgetImplementationSet(const std::string& s) { _currentWidgetImplementationSet = s; }
		const std::string& getCurrentWidgetImplementationSet() { return _currentWidgetImplementationSet; }
		
		template <class T> T* createWidget(const std::string& widgetType, cefix::PropertyList* pl) 
		{
			osg::ref_ptr< cefix::PropertyList > dummy_pl = new cefix::PropertyList();
			osg::ref_ptr< cefix::PropertyT<osg::Referenced*> > p;
			p = new cefix::PropertyT<osg::Referenced*>(cefix::AbstractProperty::PROPERTY_LIST, pl);
			dummy_pl->addValue(widgetType, p.get());
			ResponderMap map;
			
			osg::ref_ptr<cefix::AbstractWidget> t = create(p.get(), map);
			return (t.valid()) ? dynamic_cast<T*>(t.release()) : NULL; 
		}
				
	protected:

		typedef std::vector< std::pair< osg::ref_ptr< AbstractWidget >, std::string > > WidgetResponderVector;
		typedef std::map<std::string, osg::ref_ptr< AbstractWidget::Responder > > WidgetMap;
				
		/// create the widget (recursive)
		cefix::AbstractWidget* createWidgets(cefix::AbstractProperty* p, WidgetResponderVector& widgetResponder, WidgetMap& widgetMap);
		
		/** add the responder-instances to the created widgets */
		void addResponder( ResponderMap& respondermap, WidgetResponderVector& widgetResponder, WidgetMap& widgetMap);
		
	
	private:
		typedef ObjectFactory< AbstractWidget, std::string, WidgetCreatorBase, cefix::PropertyList*> WidgetFactoryImplementation;
		osg::ref_ptr< WidgetFactoryImplementation > _factory;
		

		typedef std::map<std::string, std::string> RequiredKeysMap;
		

		RequiredKeysMap			_requiredKeysMap;
		std::string				_currentWidgetImplementationSet;

};

}

#endif