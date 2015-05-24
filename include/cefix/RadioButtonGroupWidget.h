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

#ifndef RADIO_BUTTON_GROUP_WIDGET
#define RADIO_BUTTON_GROUP_WIDGET


#include <cefix/GroupWidget.h>
namespace cefix {


class CheckboxWidget;

class CEFIX_EXPORT RadioButtonGroupWidget : public GroupWidget {
	public:

		class Actions: public AbstractWidget::Actions {
		public:
			static const char* radioButtonChanged() { return "radioButtonChanged"; }
		protected:
			Actions(): AbstractWidget::Actions() {}
		};
		class Responder : public virtual AbstractWidget::Responder {
			
			public:
				Responder() : AbstractWidget::Responder() {}
				
				virtual void radioButtonChanged() {}
				
				virtual void respondToAction(const std::string& action, AbstractWidget* w)
				{
					_radioButtonGroup = dynamic_cast<RadioButtonGroupWidget*>(w);
					if (_radioButtonGroup) {
						if (action == Actions::radioButtonChanged() )
							radioButtonChanged();
					}
				}
			protected:
				RadioButtonGroupWidget* getCheckboxGroup() { return _radioButtonGroup; }
			private:
				RadioButtonGroupWidget* _radioButtonGroup; 
		};
			
			
		RadioButtonGroupWidget(const std::string& identifier) : 
			GroupWidget(identifier),
			_currentSelection(NULL)
		{
		}
		
		RadioButtonGroupWidget(cefix::PropertyList* pl);
		
		void setNewSelection(CheckboxWidget* widget);
		CheckboxWidget* getCurrentSelected() { return _currentSelection; }
	protected:
		CheckboxWidget*		_currentSelection;
};

}
#endif