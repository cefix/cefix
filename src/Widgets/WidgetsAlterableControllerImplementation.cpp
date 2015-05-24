/*
 *  WidgetsAlterableController.cpp
 *  cefix_alterable
 *
 *  Created by Stephan Huber on 02.09.11.
 *  Copyright 2011 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "WidgetsAlterableControllerImplementation.h"
#include <cefix/WidgetComposer.h>

namespace cefix {


class AlterableSliderWidgetResponderHelper : public osg::Referenced {
public:
    AlterableSliderWidgetResponderHelper(Alterable* a, unsigned int composite_ndx)
    :   osg::Referenced(),
        _a(a),
        _ndx(composite_ndx)
    {
    }
    
    void setValue(float f) { _a->setFloatValueAt(_ndx, f); }
    float getValue() { return _a->getFloatValueAt(_ndx); }
    
private:
    osg::observer_ptr<Alterable> _a;
    unsigned int _ndx;
};

class AlterableSwitchWidgetResponderHelper : public osg::Referenced {
public:
    AlterableSwitchWidgetResponderHelper(Alterable* a, unsigned int composite_ndx)
    :   osg::Referenced(),
        _a(a),
        _ndx(composite_ndx)
    {
    }
    
    void setValue(bool f) { 
        _a->setFloatValueAt(_ndx, f); 
    }
    bool getValue() { 
        return _a->getFloatValueAt(_ndx); 
    }
    
private:
    osg::observer_ptr<Alterable> _a;
    unsigned int _ndx;
};


WidgetsAlterableControllerImplementation::WidgetsAlterableControllerImplementation(float group_width, float group_gap, float dy, float start_z) 
:   AlterableControllerImplementation(), 
    _group(),
    _widgets(),
    _groupWidth(group_width),
    _groupGap(group_gap),
	_dy(dy),
    _startZ(start_z)
{
    _group = new osg::Group();
    _group->getOrCreateStateSet()->setRenderBinDetails(20, "DepthSortedBin");
    
    _widgets = new cefix::GroupWidget("all");
}


void WidgetsAlterableControllerImplementation::rebuild()
{
    _responderHelper.clear();
    _group->removeChildren(0, _group->getNumChildren());
    _widgets = new cefix::GroupWidget("all");
    
    AlterableController* ctrl = getController();
    osg::Vec3 anchor(10, 0, _startZ);
    for(AlterableController::iterator i = ctrl->begin(); i != ctrl->end(); ++i) 
    {
        cefix::WidgetComposer composer(osg::Vec3(10,10,10), _groupWidth, 1);
        
        AlterableGroup::List& l = i->second.items;
        for(AlterableGroup::List::reverse_iterator j = l.rbegin(); j != l.rend(); ++j) {
            Alterable* a = *j;
            for(int k = a->getNumComposites()-1; k >=0; --k) {
                switch(a->getRepresentationType()) {
                    case Alterable::VALUE:
                        {
                        AlterableSliderWidgetResponderHelper* rh = new AlterableSliderWidgetResponderHelper(a, k);
                        _responderHelper.push_back(rh);
                        composer.addSlider(
                            a->getCompositeName(k),
                            a->getSliderRange(), 
                            rh, 
                            &AlterableSliderWidgetResponderHelper::getValue, 
                            &AlterableSliderWidgetResponderHelper::setValue);
                        } 
                        break;
                    case Alterable::SWITCH:
                        {
                        AlterableSwitchWidgetResponderHelper* rh = new AlterableSwitchWidgetResponderHelper(a, k);
                        _responderHelper.push_back(rh);
                        composer.addButton(
                            a->getCompositeName(k),
                            rh, 
                            &AlterableSwitchWidgetResponderHelper::getValue, 
                            &AlterableSwitchWidgetResponderHelper::setValue)->setSelectableFlag(true);
                        } 
                        break;
                    case Alterable::BUTTON:
                        {
                            AlterableCallFunctor* acf = dynamic_cast<AlterableCallFunctor*>(a);
                            if (acf) {
                                composer.addButton(
                                    acf->getCompositeName(k),
                                    cefix::ButtonWidget::DEPRESSED,
                                    acf, 
                                    &AlterableCallFunctor::call
                                );
                            }
                        } 
                        break; 
                    
                    default:
                        cefix::log::error("WidgetsAlterableControllerImplementation") << "can't handle AlterableRepresentationType " << a->getRepresentationType() << std::endl;
                        break;
                        
                }
            }
            composer.advance(_dy);
        }
        cefix::DrawerWidget* widget = composer.createDrawer(i->first, anchor, false);
        anchor += osg::Vec3(_groupWidth+_groupGap, 0, 20);
        _widgets->add(widget);
        _group->addChild(widget->getNode());
    }
}

}