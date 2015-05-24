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

#ifndef DISPLAY_ITEM_GROUP_HEADER_
#define DISPLAY_ITEM_GROUP_HEADER_

#include <vector>
#include <osg/Referenced>
#include <osg/MatrixTransform>

#include <cefix/AnimationFactory.h>
#include <cefix/AnimationController.h>

#include <cefix/AbstractDataContainer.h>

namespace cefix {

/**
 * base class for all display item groups. You can hide and show them. A display-item-group has a vector of Items. 
 * You set the data of this layer via the setData-method.
 * The content is shown/hidden automatically, even removed from the scenegraph */
class CEFIX_EXPORT DisplayItemGroup : public osg::Referenced {

    public:
        /** this class represents an item of the content-layer. Your items should be s subclass of this base-class */
        class CEFIX_EXPORT Item : public osg::Referenced {
            public:
				typedef unsigned int ID;
				/** ctor */
                Item();
                /** gets the node which represents this item -- for building the scenegraph */
                virtual osg::Node* getNode() = 0;
				
                /** adds an animation-object to the animationController
                 *  @param ctrl the controller, which controls the animation
                 *  @param shouldHide true, if the content gets hidden/feaded out 
                 */
                virtual void addAnimation(cefix::AnimationController* ctrl, float starttime, bool shouldHide) {}
                
                /** this method gets called from the Content-Layer, when the language got changed */
                virtual void languageChanged() {}
				
				/** activate this item */
				virtual void activate() {}
				/** deactivate this item */
				virtual void deactivate() {}
                
                /** sends an error-msg to the logger */
                void error(std::string msg);
				
				unsigned int getId() { return _id; }
				
			protected:
				virtual ~Item() {}
			private:
				ID _id;
				
		};
            
        /** ctor */
        DisplayItemGroup();
		
        /** show this layer */
        virtual void show(float starttime = 0.0f);       
		   
        /** hide this layer */             
        virtual void hide(float starttime = 0.0f);
		
        /** sets the data for this layer */
		virtual void setData(AbstractDataContainer* container) {};
                
        /** @return true, if this content is visible */
		bool isVisible() const { return _isVisible; }
        /** @return the group representing the subgraph of the content */
		osg::MatrixTransform* getGroup() { return _transform.get(); }
        
		/**  gets called from the animationController, when the hide-animation is finished */
		void finishHideAnimation();
		
		/**  gets called from the animationController, when the hide-animation is finished */
		void finishShowAnimation();
        
        /** inform all items of a changed language */
        virtual void languageChanged();
        
        /** sets the transform-matrix of this content-layer */
        void setMatrix(osg::Matrix m) { _transform->setMatrix(m); }
        
        /** sets the top left position */
        inline void setTopLeft(osg::Vec3 v) {
            setMatrix(osg::Matrix::translate(v));
        }
		
		/** called, when the hiding is finished */
		inline bool isHideFinished() { return _hideFinished; }
		
		inline bool isShowFinished() { return _showFinished; }
		
		/** sets the animation starttime for a particular item */
		inline void setAnimationStartTimeFor(Item* item, float starttime) {
			if (item) _starttimes[item->getId()] = starttime;
		}
		
		/** deactivates all items of this group */
		virtual void deactivate() {
			for (ItemVector::iterator i = _items.begin(); i != _items.end(); i++) {
				Item* item = (*i).get();
				item->deactivate();
			}
		}
		
		/** activates all items of this group */
		virtual void activate() {
			for (ItemVector::iterator i = _items.begin(); i != _items.end(); i++) {
				Item* item = (*i).get();
				item->activate();
			}
		}
		
		void setNodeMask(unsigned int mask) { _nodeMask = mask; }
		unsigned int getNodeMask() const { return _nodeMask; }
        
        unsigned int getNumItems() const { return _items.size(); }
        Item* getItemAt(unsigned int i) const { return _items[i]; }
        
        /** creates the animationcontroller to hide the content */
        virtual void createHideAnimation(cefix::AnimationController* ctrl, float starttime);
        
		/** creates the animatonController to show the content */
        virtual void createShowAnimation(cefix::AnimationController* ctrl, float starttime);
        
        /** adds an item to the list of items */
		void addItem(Item* item);
        
		void removeItem(Item* item);
		
	 protected:
		/** dtor */
        virtual ~DisplayItemGroup();

        
        /** sets the visibility-flag */
        void setVisible(bool flag) { 
            _isVisible = flag; 
        }
		
		/** renove all items from this itemgroup */
		void removeAllItems() {
			_items.clear();
			_starttimes.clear();
			_transform->removeChild(0, _transform->getNumChildren());
		}
		    
        typedef std::vector< osg::ref_ptr< Item > >  ItemVector;
		typedef std::map<Item::ID, float>			 ItemsAnimationStartTimeMap;
        ItemVector                                  _items;
		ItemsAnimationStartTimeMap					_starttimes;
    
        bool                                        _isVisible;
		bool										_hideFinished, _showFinished;
        osg::ref_ptr<osg::MatrixTransform>          _transform;
		osg::ref_ptr<cefix::AnimationController>	_animationController;
		unsigned int								_nodeMask;
        
        //friend class ContentLayerHideAdapter;
        friend class DisplayItemSubGroup;
        
        
};

}

#endif