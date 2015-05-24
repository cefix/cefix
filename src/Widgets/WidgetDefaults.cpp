/*
 *  WidgetDefaults.cpp
 *  cefixcvTemplate
 *
 *  Created by Stephan Huber on 07.11.08.
 *  Copyright 2008 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "WidgetDefaults.h"
namespace cefix {

osg::Vec2 WidgetDefaults::_sliderThrobberSize = osg::Vec2(10,10);
float WidgetDefaults::_buttonHeight = 15.0;
float WidgetDefaults::_sliderHeight = 11.0;
osg::Vec2  WidgetDefaults::_checkboxSize = osg::Vec2(11.0, 11.0);
osg::Vec2  WidgetDefaults::_radiobuttonSize = osg::Vec2(11.0, 11.0);
float WidgetDefaults::_progressbarHeight = 12.0;

osg::Vec2  WidgetDefaults::_hsliderCaptionDelta = osg::Vec2(0,0);
osg::Vec2  WidgetDefaults::_hsliderValueDelta = osg::Vec2(0,0);
float WidgetDefaults::_textInputHeight = 15;

HSliderWithCaptionAndValueWidget::Style WidgetDefaults::_hSliderWithCaptionAndValueWidgetStyle = HSliderWithCaptionAndValueWidget::CAPTION_AND_VALUE_OUTSIDE;
}