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

#ifndef __POSTSCRIPTWRITER_HEADER__
#define __POSTSCRIPTWRITER_HEADER__

#include <string>
#include <osg/Referenced>
#include <osgUtil/SceneView>
#include <osgUtil/StateGraph>
#include <cefix/Export.h>

class Notification;

namespace cefix {
    class PSWVertexWriter;

    /**  
     * PostscriptWriter implements a postscript-screenshot-utility, it dumps the current scene to postscript line-art
     * with (almost) correct clipping etc
     * it is utilized as a NodeCallback, you have to attach an instance to your graph, and it will wait for a key-stroke
     * (usually p) and then it will dump an eps-file to a specified location
     * @author Stephan Maximilian Huber
     */
    class CEFIX_EXPORT PostScriptWriter : public osg::NodeCallback {
        
        public: 
            
            /**
             * helper-method, will create a new PostScriptWriter and attach it to the Sceneview
             * @param sv the sceneview to attacht to
             */
            static PostScriptWriter* install(osgUtil::SceneView* sv);
            /**
             * Constructor
             * @param sv sceneview to attach to
             */
            PostScriptWriter(osgUtil::SceneView* scene);
            
            /**
             * handles the keypress-notification#
             */
            void handleKeyPressMessage(const int& key);
            
            /**
             * writes the current scene as eps
             */
            void write(const std::string file = "");
            
            /** 
             * sets the output path
             * @param s the new path to store new eps-files
             */
            void setOutputPath(std::string s) { _outputPath = s; }
            
            /** @return the activation key */
            char getActivationKey() { return _activationChar; }
            
            /** sets the activationkey  @param key the new activation key */
            void setActivationKey(char key) { _activationChar = key;}
            
            /** is called when traversing the scene */
            virtual void operator() (osg::Node* node, osg::NodeVisitor* nv) {
                
                 // must continue subgraph traversal.
                traverse(node,nv);
                
                if (nv) {
                    if ((_writeFlag) /*&& (node==_scene->getSceneData())*/ && (nv->getTraversalNumber()!=_previousTraversalNumber)){
                                                   
                        write();
                        _writeFlag = false;
                    }
                    _previousTraversalNumber = nv->getTraversalNumber();
                }            
        
            }
			
			void setWireFrameMode(bool flag) { _wireframe = flag; }

            
        protected:
			virtual ~PostScriptWriter();
			
            osg::ref_ptr<osgUtil::SceneView>    _scene;
            std::string                         _outputPath;
            unsigned int                        _written;
            bool                                _writeFlag;
            unsigned int                        _previousTraversalNumber;
                       
            unsigned int                        _countDrawables, _countMatrices;
			char                                _activationChar;
			bool								_wireframe;
            
            /* writes a StateGraph */
            void writeStateGraph(PSWVertexWriter *psvwr, osgUtil::StateGraph* graph);
            /* writes a RenderBin */
            void writeRenderBin(PSWVertexWriter *psvwr, osgUtil::RenderBin *bin);
            /* writes a renderLeaf */
            void writeRenderLeaf(PSWVertexWriter *psvwr, osgUtil::RenderLeaf* rl);
    };

}


#endif

