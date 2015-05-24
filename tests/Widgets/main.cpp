//
//  main.c
//  cefixTemplate
//
//  Created by Stephan Huber on 26.11.06.
//  Copyright __MyCompanyName__ 2006. All rights reserved.
//

#include <osg/MatrixTransform>
#include <osgDB/ReadFile>

#include <cefix/AppController.h>
#include <osg/ShapeDrawable>
#include <cefix/ConvertScreenCoord.h>

#include <cefix/DataFactory.h>
#include <cefix/DropShadowGeometry.h>

#include <cefix/VerticalSliderWidget.h>
#include <cefix/HorizontalSliderWidget.h>
#include <cefix/AreaSliderWidget.h>
#include <cefix/TextWidget.h>
#include <cefix/GroupWidget.h>
#include <cefix/TextButtonWidget.h>
#include <cefix/ViewWidget.h>
#include <cefix/DragWidget.h>
#include <cefix/WidgetFactory.h>

#include <cefix/WidgetThemeManager.h>
#include <cefix/SimpleWidgetDrawImplementationDefaults.h>

#include "WindowWidget.h"


class GetterSetterTest : public osg::Referenced {
public:
	float getValue() const { return _value; }
	void setValue(float v) { _value = v; std::cout << "GetterSetter: " << _value << std::endl; }
	
private:
	float _value;
	
};

class RotateSliderResponder : public cefix::AbstractWidget::Responder {

	public:
		RotateSliderResponder(osg::MatrixTransform* mat) : cefix::AbstractWidget::Responder(), _mat(mat), _zangle(0), _xangle(0) {}
		
		virtual void respondToAction(const std::string& action, cefix::AbstractWidget* widget) {
			if (action == cefix::SliderWidget::Actions::valueChanged() ) {
				cefix::HorizontalSliderWidget* hslider = dynamic_cast<cefix::HorizontalSliderWidget*>(widget);
				if (hslider) 
					_zangle = hslider->getValue();
				cefix::VerticalSliderWidget* vslider = dynamic_cast<cefix::VerticalSliderWidget*>(widget);
				if (vslider) 
					_xangle = vslider->getValue();
				
				_mat->setMatrix(osg::Matrix::rotate(osg::DegreesToRadians(_xangle), 1,0,0) * (osg::Matrix::rotate(osg::DegreesToRadians(_zangle), 0,0,1)));
			}				
		}
		
	private:
		osg::observer_ptr<osg::MatrixTransform>	_mat;
		float _zangle, _xangle;
};


class SwitchButtonResponder: public cefix::AbstractWidget::Responder {
	public:	
		SwitchButtonResponder(cefix::GroupWidget* group) : cefix::AbstractWidget::Responder(), _group(group) {}
		
		virtual void respondToAction(const std::string& action, cefix::AbstractWidget* widget) {
			if ( (action == cefix::ButtonWidget::Actions::stateChanged() ) && (widget->getIdentifier() == "SwitchButton")) {
				
				cefix::TextButtonWidget* b = dynamic_cast<cefix::TextButtonWidget*>(widget);
				if (b) {
					switch (b->getState()) {
						case cefix::ButtonWidget::DEPRESSED:
						
							if (b->isSelected()) {
								b->setCaption("show widgets");
								if(_group.valid()) _group->getNode()->setNodeMask(0x0);
							} else {
								b->setCaption("hide widgets");
								if(_group.valid()) _group->getNode()->setNodeMask(0xFFFF);
							}	
							break;
						default:
							break;
							
					}
				}
			}
		}
		
	private:
		osg::observer_ptr<cefix::GroupWidget> _group;
		
};

class ChangeCubeColorResponder : public cefix::AbstractWidget::Responder {

	public:
		ChangeCubeColorResponder(osg::ShapeDrawable* drawable) : cefix::AbstractWidget::Responder(), _drawable(drawable) {}
		virtual ~ChangeCubeColorResponder() { std::cout << "ups" << std::endl;}
		
		virtual void respondToAction (const std::string &action, cefix::AbstractWidget *widget) {
			osg::Vec4 c =_drawable->getColor();
			cefix::VerticalSliderWidget* slider = dynamic_cast<cefix::VerticalSliderWidget*>(widget);
			if (slider) {
				std::cout << action << " " << slider->getState() << std::endl;
			
				if (slider->getIdentifier() == "slider_r") c[0] = slider->getValue();
				if (slider->getIdentifier() == "slider_g") c[1] = slider->getValue();
				if (slider->getIdentifier() == "slider_b") c[2] = slider->getValue();
				if (slider->getIdentifier() == "slider_a") c[3] = slider->getValue();
				_drawable->setColor(c);
			}
		}
		
	private:
		osg::ref_ptr<osg::ShapeDrawable> _drawable;

};


class MyAppController: public cefix::AppController {

	public:
		void debugWidgetStateResponder() {
			std::cout << "geht" << std::endl;
			
		}
		
		
		osg::Group * createWorld() {
        
            cefix::SimpleWidgetDrawImplementationDefaults* defaults = cefix::SimpleWidgetDrawImplementationDefaults::instance();
            defaults->setBtnBgColorFor(cefix::AbstractWidget::NORMAL, osg::Vec4(1,0,0,1));
            
		
			_getterSetterTest = new GetterSetterTest();
			/* use theme: */
			
			cefix::WidgetFactory::instance()->setWidgetImplementationSet("themed");
			
			cefix::WidgetThemeManager::instance()->loadTheme("sw_widget_theme.xml");
			cefix::WidgetThemeManager::instance()->setCurrentTheme("simple_bw");
			
			
			getMainWindow()->getCamera()->setClearColor(osg::Vec4(0,0,0,1));
		
			osg::Group* g = new osg::Group();
			
			
			osg::ref_ptr<cefix::GroupWidget> mainGroupWidget = new cefix::GroupWidget("mainSampleGroup");
			
			// Button
			mainGroupWidget->add(new cefix::SimpleTextButtonWidget("SwitchButton", osg::Vec3(10, 565,0), osg::Vec2(150, 18), "Hide Widgets"));
			
			osg::ref_ptr<cefix::SliderWidgetT<cefix::VerticalSliderWidget, cefix::SimpleSliderDrawImplementation> >vslider;
			osg::ref_ptr<cefix::SimpleHorizontalSliderWidget> hslider;
			
			if (1) {
			
				// horizontaler slider:
				hslider = new cefix::SimpleHorizontalSliderWidget(
					"SampleHSlider", 
					osg::Vec3(20,20,0), 
					osg::Vec2(750,10), 
					osg::Vec2(40, 10)
				);
				hslider->setRange(cefix::doubleRange(-100.0f, 100.0f));
				
				// vertikaer Slider
				vslider = new cefix::SliderWidgetT<cefix::VerticalSliderWidget, cefix::SimpleSliderDrawImplementation>(
					"SampleVSlider", 
					osg::Vec3(770,30,0), 
					osg::Vec2(10,540), 
					osg::Vec2(10, 40)
				);
				vslider->setRange(cefix::doubleRange(-100.0f, 100.0f));
				
				// Area Slider
				osg::ref_ptr<cefix::SimpleAreaSliderWidget> gslider = new cefix::SimpleAreaSliderWidget("AreaSlider",osg::Vec3(0,0,0), osg::Vec2(200, 200), osg::Vec2(10,10));
				gslider->setRange(cefix::doubleRange(-100.0f, 100.0f),cefix::doubleRange(-100.0f, 100.0f));
				
				
				// miteinander verknüpfen, sodass sich vertikaler und horizontaler Slider updaten. falls sich am areaSlider was tut und vice versa
				gslider->addResponder(hslider.get());
				gslider->addResponder(vslider.get());
				
				hslider->addResponder(gslider.get());
				vslider->addResponder(gslider.get());
				
				
				// TextWidgets für die Werte:
				osg::ref_ptr<cefix::TextWidget> htext = new cefix::TextWidget("HSliderOutput", osg::Vec3(100,10,0));
				htext->getTextGeode()->setTextColor(osg::Vec4(1,1,1,1));
				hslider->addResponder(htext.get());
				
				osg::ref_ptr<cefix::TextWidget> vtext = new cefix::TextWidget("VSliderOutput", osg::Vec3(200,10,0));
				vtext->getTextGeode()->setTextColor(osg::Vec4(1,1,1,1));
				vslider->addResponder(vtext.get());
				
				osg::ref_ptr<cefix::TextWidget> gtext = new cefix::TextWidget("GSliderOutput", osg::Vec3(300,10,0));
				gtext->getTextGeode()->setTextColor(osg::Vec4(1,0,1,1));
				gslider->addResponder(gtext.get());
				
				// window fuer gslider erzeugen:
				
				cefix::SimpleWindowWidget* sliderwin= new cefix::SimpleWindowWidget("SliderWindow", "Rotate cow", osg::Vec3(50,50,100), osg::Vec2(201,201), cefix::ViewWidget::ORTHO, cefix::ViewWidget::DIRECT);
				sliderwin->add(gslider.get());
				
				
				// Gruppe erzeugen
				osg::ref_ptr<cefix::GroupWidget> groupWidget = new cefix::GroupWidget("sampleGroup");
				groupWidget->add(hslider.get());
				groupWidget->add(vslider.get());
				groupWidget->add(sliderwin);
				
				groupWidget->add(htext.get());
				groupWidget->add(vtext.get());
				groupWidget->add(gtext.get());
				
				
				mainGroupWidget->add(groupWidget.get());
				

				
				// Beispiel wie man an ein Widget in einer Gruppe zugreifen kann, wenn man dessen Namen und Typ kennt.
				mainGroupWidget->get<cefix::TextButtonWidget*>("SwitchButton")->setCaption("hide widgets");
				mainGroupWidget->get<cefix::TextButtonWidget*>("SwitchButton")->setSelectableFlag(true);
				mainGroupWidget->get<cefix::TextButtonWidget*>("SwitchButton")->addResponder(new SwitchButtonResponder(groupWidget.get()));
			}
		
									
			
			if (1) {
				osg::ref_ptr<cefix::GroupWidget> groupWidget = new cefix::GroupWidget("sampleGroup2");
				mainGroupWidget->add(groupWidget.get());
				// groupWidget->add(new cefix::SimpleDragWidget("Drag", osg::Vec3(100, 550, 0), osg::Vec2(100,16)));
				
				// Window:
				groupWidget->add(new cefix::SimpleWindowWidget("View", "Test-Window", osg::Vec3(400, 200, 100), osg::Vec2(400,300), cefix::ViewWidget::PERSPECTIVE, cefix::ViewWidget::BITMAP));
				

				osg::Node* node = osgDB::readNodeFile("cow.osg");
				osg::MatrixTransform* m = new osg::MatrixTransform();		
				m->addChild(node);
				groupWidget->get<cefix::WindowWidget*>("View")->getCamera()->addChild(m);
				groupWidget->get<cefix::WindowWidget*>("View")->getCamera()->setClearColor(osg::Vec4(1,1,1,0.5));
				
				
				groupWidget->get<cefix::WindowWidget*>("View")->getCamera()->setViewMatrix(osg::Matrix::lookAt(osg::Vec3(0,-15, 0), osg::Vec3(0,0,0), osg::Vec3(0,0,1)));
				groupWidget->get<cefix::WindowWidget*>("View")->getCamera()->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
				
				// jetzt unseren Responder erzeugen, der auf die Slider "antwortet" und eine matrixtransform steuert
				RotateSliderResponder* rotateSliderResponder = new RotateSliderResponder(m);
				if (hslider.valid()) hslider->addResponder(rotateSliderResponder);
				if (vslider.valid()) vslider->addResponder(rotateSliderResponder);
						
			}
			
			
			
			// die MainGruppe merken wir uns, sonst geht das alles nachher out-of-scope und wird gelöscht.
			_widgets = mainGroupWidget;
			
			// in den Szenengraph hängen
			get2DLayer()->addChild(mainGroupWidget->getNode());			
			

			osg::setNotifyLevel(osg::INFO);
			cefix::WidgetFactory* wf = cefix::WidgetFactory::instance();
			cefix::AbstractProperty* widgetsprop = cefix::DataFactory::instance()->getPreferencesFor("/WidgetTest/GroupWidget");
			if ((widgetsprop != NULL) && (widgetsprop->getType() == cefix::AbstractProperty::PROPERTY_LIST)) {

			
				osg::Geode* geode2 = new osg::Geode();
				osg::ShapeDrawable* drawable = new osg::ShapeDrawable(new osg::Box(osg::Vec3(0,0,0), 1));
				drawable->setColor(osg::Vec4(1,0,0,1));
				geode2->addDrawable(drawable);
				g->addChild(geode2);
				g->addChild(osgDB::readNodeFile("cow.osg"));
				geode2->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);

				
				// responder-map:
				cefix::WidgetFactory::ResponderMap respondermap;
				respondermap.add("change_cube_color", new ChangeCubeColorResponder(drawable));
				respondermap.add(
					"change_cube_color", 
					new cefix::WidgetStateFunctorResponder<cefix::SliderWidget, MyAppController>(
						cefix::AbstractWidget::NORMAL_OVER, this, &MyAppController::debugWidgetStateResponder)
				);
				respondermap.add(
					"getter_setter_test", 
					cefix::AbstractWidget::createResponderFromSetter<cefix::HorizontalSliderWidget>(_getterSetterTest.get(), &GetterSetterTest::setValue));
				respondermap.add("save_preferences", cefix::AbstractWidget::createFunctorResponderBridge(this, &MyAppController::savePreferences));
								
				osg::ref_ptr<cefix::AbstractWidget> widgetsFromXML = wf->create(widgetsprop, respondermap);
				_widgets->add(widgetsFromXML.get());
				
				// so noch das FieldOfView-Widget suchen und registrieren
				
                /*
                if (widgetsFromXML.valid()) {
					cefix::FieldOfViewWidget* fov = _widgets->find<cefix::FieldOfViewWidget*>("fov"
						);
					if (fov)
						fov->applyToWindow(getMainWindow());
					
					cefix::ViewPointManagerWidget* vpmw = _widgets->find<cefix::ViewPointManagerWidget*>("viewpointManager");
					if (vpmw)
						vpmw->applyToCameraManipulator(getMainWindow()->getTrackballManipulator());
				}
                */
				
			}
			osg::setNotifyLevel(osg::WARN);
			return g;
		}		
		
		void savePreferences(const std::string& action, cefix::AbstractWidget* widget) {
			cefix::ButtonWidget* button = dynamic_cast<cefix::ButtonWidget*>(widget);
			if (button && button->getState() == cefix::AbstractWidget::DEPRESSED)
				cefix::DataFactory::instance()->savePreferencesFile();
			std::cout << " saved " << std::endl;
		}
		
	private:
		osg::ref_ptr<cefix::GroupWidget>	_widgets;
		osg::ref_ptr<GetterSetterTest>		_getterSetterTest;
		
};


int main(int argc, char* argv[])
{
	
	osgDB::Registry::instance()->getDataFilePathList().push_back("E:\\cefix\\cefix\\tests\\Widgets");

	cefix::DataFactory::instance()->setApplicationPath(argv[0]);
	osg::ref_ptr<MyAppController> app = new MyAppController();
	app->setUseOptimizerFlag(false);
	//app->setThreadingModel(osgViewer::ViewerBase::DrawThreadPerContext);
	// create the world and apply it
    app->applyWorld(app->createWorld());

    app->requestPicking();
    //app->requestPostScriptWriter();
    
    //create a viewer-window/fullscreen
    app->realize();
	//app->getMainWindow()->setWindowTitle("cefix Widget Test");
	//app->getMainWindow()->useCursor(false);
	app->run();
	return 0;
}


