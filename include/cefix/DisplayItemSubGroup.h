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

#ifndef DISPLAY_ITEM_SUB_GROUP_HEADER_
#define DISPLAY_ITEM_SUB_GROUP_HEADER_


#include <cefix/Export.h>
#include <cefix/DisplayItemGroup.h>

/** 
 * ContentSublayer is inherited from ContentLayerBase::Item, it encapsulates a complete ContentLayer, 
 * so you can create hierarchical structures out of ContentLayers...
 */
 
 namespace cefix {
 
class CEFIX_EXPORT DisplayItemSubGroup : public DisplayItemGroup::Item {

    public:
		/** ctor 
		 *  sublayer the layer to attach as sublayer */
        DisplayItemSubGroup(DisplayItemGroup* sublayer) : DisplayItemGroup::Item(), _sublayer(sublayer) {}
        
		/** @return the node describingthe layer */
        virtual osg::Node* getNode() { return _sublayer->getGroup(); }
        
		/** called when the language was changed */
        virtual void languageChanged() {
            _sublayer->languageChanged();
        }
        
		/** adds the show/hide-animations */
        virtual void addAnimation(cefix::AnimationController* ctrl, float starttime, bool shouldHide);
        
		/** @return the layer */
        DisplayItemGroup* getLayer() { return _sublayer.get(); }
        /** adds an item to the sublayer */
        void addItem(DisplayItemGroup::Item* item) { _sublayer->addItem(item); }
		
        void removeItem(DisplayItemGroup::Item* item) { _sublayer->removeItem(item); }
        
		/** renove all items from this itemgroup */
		void removeAllItems() { _sublayer->removeAllItems(); }
		
		/** deactivate sublayer */
		virtual void deactivate() {
			_sublayer->deactivate();
		}
		
		/** activate sublayer */
		virtual void activate() {
			_sublayer->activate();
		}
        
    protected:
		void setLayerVisibility(bool visible) { _sublayer->setVisible(visible); }
        explicit DisplayItemSubGroup() : DisplayItemGroup::Item() { _sublayer = new DisplayItemGroup(); }
        virtual ~DisplayItemSubGroup() {}
        osg::ref_ptr<DisplayItemGroup> _sublayer;
        

};

}

#endif