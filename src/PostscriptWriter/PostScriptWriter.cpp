/*
 *  PostScriptWriter.cpp
 *  Circles
 *
 *  Created by Stephan Huber on 17.10.04.
 *  Copyright 2004 __MyCompanyName__. All rights reserved.
 *
 */


#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <list>

#include <osg/io_utils>
#include <osg/Matrix>
#include <osg/Geode>
#include <osg/Geometry>
#include <osgUtil/RenderBin>
#include <osgUtil/StateGraph>
#include <osgUtil/RenderLeaf>
#include <osg/PrimitiveSet>

#include <cefix/PostScriptWriter.h>
#include <cefix/Settings.h>
#include <cefix/NotificationCenter.h>
#include "Frustum.h" 

namespace cefix {
    
    /**
     * PSLineStack is a stack of lines
     * it holds a stack of projected vectors with corresponding z-values
     * the correct name should be POLYGON or LINE-STRIP
     */
    class PSLineStack {
        public: 
            /** Constructor */
            PSLineStack(bool drawFilled = false) : _zcomputed(false), _z(0), _closed(false), _drawFilled(drawFilled), _clipCount(0) {
                static unsigned int s_lineid = 0;
                _id = s_lineid++;
                // osg::notify(osg::INFO) << "creating Linestack " << _id << std::endl;
				_color[0] = -1;


            }
			
			inline void incClipCount() { ++_clipCount; }
            
            void setClosed() {_closed = true;}
			
			inline bool isCompletelyClipped() const { return (_clipCount == _a.size()); }
            
            /** adds a new line and its z-value to the stack */
            inline void add(osg::Vec3 &a, osg::Vec3 &b, float &z) {  
                _a.push_back(a);
                _b.push_back(b);
                _zs.push_back(z);
                _zcomputed = false;
                // osg::notify(osg::INFO) << "adding to Linestack " << _id << "#" << _a.size() << std::endl;
            }
            
            /** returns the interpolated z-value of this stack */
            float getZ()  { 
                if (_zcomputed)
                    return _z;
                
                float z = 0;
                unsigned int s = _zs.size();
                if (0 == s) {
                    _z = 0;
                    return _z;
                }
                for (unsigned int i = 0; i < s; i++) {
                    z += _zs[i];
                }
                _z = z/s;
                           
                _zcomputed = true;
                return _z;
            }
            
            /** sets the z-value for this stack */
            void setZ(float z) { _z = z; _zcomputed = true; _doNotResetZFlag = true;}
            
            /** comparision-operator (uses z-values to compare) */
            inline bool operator <  ( PSLineStack& ls) 
            {
                return (getZ() < ls.getZ());
            }
			
			void setColor(osg::Vec4  c) { _color = c; }
            
			osg::Vec4 getColor() { return _color; }
			
			void setDrawFilled(bool f) { _drawFilled = f; }
			
			/** valid */
			inline bool valid() {
				return ((_a.size() > 0) && (isCompletelyClipped() == false));
			}
			
			
                    
        protected:
            bool _zcomputed, _doNotResetZFlag;
            unsigned int _id;
            float _z;
            std::vector<osg::Vec3> _a;
            std::vector<osg::Vec3> _b;
            std::vector<float> _zs;
            bool _closed;
			osg::Vec4 _color;
			bool	_drawFilled;
			unsigned int	_clipCount;
						
            
        friend inline std::ostream& operator << (std::ostream& output, const PSLineStack &ls);
    };
	
	

    /** streams the postscript code to output */
    inline std::ostream& operator << (std::ostream& output, const PSLineStack &ls)
    {

        unsigned int s = ls._a.size();
        if ((0 == s) || ls.isCompletelyClipped()) {
            return output;
        }
        
        /*
        if (s >= 2) {
            output << "1 1 1 setrgbcolor" << std::endl;
            for (unsigned int i = 0; i < s; i++) {
                if ((i>0) && ( ls._b[i-1] == ls._a[i]))
                    ;
                else
                    output << ls._a[i][0] << " " << ls._a[i][1] << " moveto ";
                    
                output << ls._b[i][0] << " " << ls._b[i][1] << " lineto ";
            }
            
            output << " closepath gsave fill grestore 0 0 0 setrgbcolor stroke" << std::endl;
        
        } else */ 
        {
			if (ls._color[0] >= 0)
				output << ls._color[0] << " " << ls._color[1] << " " << ls._color[2] << " setrgbcolor" << std::endl;
            output << "newpath ";
            
            for (unsigned int i = 0; i < s; i++) {
                if ((i==0) || (ls._b[i-1] != ls._a[i]))
				
				output << ls._a[i][0] << " " << ls._a[i][1] << " moveto ";
                    
                output << ls._b[i][0] << " " << ls._b[i][1] << " lineto ";
            }
            if (ls._closed)
                output << " closepath";
			if ((ls._closed) && (ls._drawFilled))
				output << " fill" << std::endl;
			else
				output << " stroke" << std::endl;
        }
        
        return output; 	// to enable cascading
    }

    #pragma mark - 
	
	class DrawColorHelper : public osg::Referenced {
	
		public:
			DrawColorHelper(osg::Vec4Array* colors) { _colors = colors; _ndx = 0; }
			
			osg::Vec4 getColor() { return (*_colors)[_ndx++]; }
		private:
			osg::ref_ptr<osg::Vec4Array> _colors; 
			unsigned int _ndx;
	};
		
	#pragma mark - 
    
    /**
     * PSWVertexWriter is a helper class which does the hard work
     * it extracts the vertex data from the geodes, projects it to 2d, and clips the lines, and writs the result to a file 
     */
    class PSWVertexWriter : public osg::PrimitiveFunctor {
            public:
            /** Constructor
             * @param filename, filename to write into
             * @param sv Sceneview to dump as postscript
             */
            PSWVertexWriter(std::string filename, osgUtil::SceneView* sv); 
            /** destructor*/       
            virtual ~PSWVertexWriter();
            
            /** unsupported Vec2-array -- is called from the primitiveFunctor*/
            virtual void setVertexArray(unsigned int,const osg::Vec2d*) 
            {
                log::error("PSWVertexWriter") << "PSWVertexWriter does not support Vec2* vertex arrays"<<std::endl;
            }
			
			/** unsupported Vec2-array -- is called from the primitiveFunctor*/
            virtual void setVertexArray(unsigned int,const osg::Vec2*) 
            {
                log::error("PSWVertexWriter") <<"PSWVertexWriter does not support Vec2* vertex arrays"<<std::endl;
            }
            
            /** sets the current vertice-array to vertices -- is called from the primitiveFunctor*/
            virtual void setVertexArray(unsigned int,const osg::Vec3* vertices) { _vertices = vertices; }
			
			/** sets the current vertice-array to vertices -- is called from the primitiveFunctor*/
            virtual void setVertexArray(unsigned int,const osg::Vec3d* vertices) { 
				log::error("PSWVertexWriter") <<"PSWVertexWriter does not support Vec3d* vertex arrays"<<std::endl;
            }

            
			/** unsupported Vec4-array -- is called from the primitiveFunctor*/
            virtual void setVertexArray(unsigned int,const osg::Vec4d*) 
            {
                log::error("PSWVertexWriter") <<"PSWVertexWriter does not support Vec4* vertex arrays"<<std::endl;
            }
			
			/** unsupported Vec4-array -- is called from the primitiveFunctor*/
            virtual void setVertexArray(unsigned int,const osg::Vec4*) 
            {
                log::error("PSWVertexWriter") <<"PSWVertexWriter does not support Vec4* vertex arrays"<<std::endl;
            }
            
            /** implements drawing of an array -- is called from the primitiveFunctor*/
            virtual void drawArrays(GLenum mode,GLint first,GLsizei count)
            {
                if (_vertices)
                {
                    begin(mode);
                    const osg::Vec3* vert = _vertices+first;
                    for(;count>0;--count,++vert)
                    {
                        _write(*vert);
                    }
                    end();
                }
            }
            /** implements drawing of an array -- is called from the primitiveFunctor*/
            virtual void drawElements(GLenum mode ,GLsizei count,const GLubyte* indices)
            {
                if (_vertices)
                {
                    begin(mode);
                    for(;count>0;--count,++indices)
                    {
                        _write(_vertices[*indices]);
                    }
                    end();
                }
            }
            /** implements drawing of an array -- is called from the primitiveFunctor*/
            virtual void drawElements(GLenum mode,GLsizei count,const GLushort* indices)
            {
                if (_vertices)
                
                {   
                    begin(mode);
                    for(;count>0;--count,++indices)
                    {
                        _write(_vertices[*indices]);
                    }
                    end();
                }
            }
            /** implements drawing of an array -- is called from the primitiveFunctor*/
            virtual void drawElements(GLenum mode ,GLsizei count,const GLuint* indices)
            {
                if (_vertices)
                {   
                    begin(mode);
                    for(;count>0;--count,++indices)
                    {
                        _write(_vertices[*indices]);
                    }
                    end();
                }
            }

            /** starts a new path */
            virtual void begin(GLenum);
            
            /** implements drawing an vertex */
            virtual void vertex(const osg::Vec2& vert) { _write(osg::Vec3(vert[0],vert[1],0.0f)); }
            
            /** implements drawing an vertex */
            virtual void vertex(const osg::Vec3& vert) { _write(vert); }
            
            /** implements drawing an vertex */
            virtual void vertex(const osg::Vec4& vert) { if (vert[3]!=0.0f) _write(osg::Vec3(vert[0],vert[1],vert[2])/vert[3]); }
            
            /** implements drawing an vertex */
            virtual void vertex(float x,float y) { _write(osg::Vec3(x,y,1.0f)); }
            
            /** implements drawing an vertex */
            virtual void vertex(float x,float y,float z) { _write(osg::Vec3(x,y,z)); }
            
            /** implements drawing an vertex */
            virtual void vertex(float x,float y,float z,float w) { if (w!=0.0f) _write(osg::Vec3(x/w,y/w,z/w)); }
            
            /** finishes drawing a bunch of vertices */
            virtual void end() {}
            
            /** returns the # clipping occurances */
            int &getClippedCount() { return _numClipped;}
            
            /** returns the # of rendered vertices */
            int &getVerticesCount() { return _numVertices;}
            
            /** sets the projection matrix*/
            void setProjectionMatrix(osg::RefMatrix* m) { _proj = m; _frustum.setMatrix(m); }
            /** sets the modelview matrix*/
            void setModelViewMatrix(osg::RefMatrix* m) { _mv = m;}
            
            /** writes the processed stuff to the file */
            inline void writeToFile();
			
			void setWireFrameMode(bool flag) { _wireframe = flag; }
			
			void setColor(osg::Vec4 c) { _currentColor = c; }
            
			void setColorArray(osg::Vec4Array* colors) { _colorHelper = new DrawColorHelper(colors); }
			
			void setDebugVertices(osg::Vec3Array* v) { _debugVertices = v; }
            
            const osg::Vec3*    _vertices;
            std::ofstream       _output;
            GLenum              _mode;
            unsigned int        _vertIdx;
            osg::Vec3           _lastV,_lastLastV;
            osg::Vec3           _lastClippedVertex;
            bool                _doClipping;
            float               _width,_height;
            int                 _numClipped,_numVertices;
            
            osg::RefMatrix*     _mv;
            osg::RefMatrix*     _proj;
			osg::ref_ptr<DrawColorHelper> _colorHelper;
            
            
            protected:
                /** writes a vertex*/
                void inline _write(const osg::Vec3 v);
                /** inits a new line-path */
                void init(GLenum mode) {
					_mode = mode; 
					_vertIdx = 0;
				}
                /** writes a line to file */
                inline void _drawLine(osg::Vec3 v1, osg::Vec3 v2);
                
                
                Frustum _frustum;    
                typedef std::list<PSLineStack> List;
                List _list;
				
				bool _wireframe;
				osg::Vec4 _currentColor;
				
				osg::ref_ptr<osg::Vec3Array>	_debugVertices;


    };


    /** 
     * Constructor, creates the file, and adds the EPS-header 
     * @param filename the file to write into
     * @param sv the sceneview to dump
     */
    PSWVertexWriter::PSWVertexWriter(std::string filename, osgUtil::SceneView* sv) : 
		_vertices(0), 
		_doClipping(false),
		_numClipped(0),
		_numVertices(0),
		_currentColor(-1,0,0,1)
	{ 
                  
        osg::notify(osg::INFO) << "writing scene to postscript-file: " << filename << std::endl;
        
        _output.open(filename.c_str());
        
         // Header
        _output << "%!PS-Adobe-3.1 EPSF-3.0" << std::endl;
        _output << "%%Title: " << filename << std::endl;
        _output << "%%Creator: org.cefix.PostScriptWriter" << std::endl;
		osg::Viewport* vp = sv->getViewport();
        int x(vp->x()), y(vp->y()), h(vp->height()), w(vp->width());
        
        _output << "%%BoundingBox: " << x << " " << y << " " << w << " " << h << std::endl;
        _output << "gsave" << std::endl;
        _output << "0.1 setlinewidth" << std::endl;
        
        _width = w/2.0f;
        _height = h/2.0f;
        _output << _width << "  " << _height << " translate" << std::endl;
        
    }

    /** destructor*/
    PSWVertexWriter::~PSWVertexWriter() {
        _output << std::endl << "grestore" << std::endl;
        _output.close();
    }


    void PSWVertexWriter::begin(GLenum mode) {
        init(mode);
        
        
    }

    /** renders a 3d-line to 2d and clips it if necessary */
    void PSWVertexWriter::_drawLine(osg::Vec3 av1, osg::Vec3 av2) {

        osg::Vec3 v1,v2, ov1, ov2;
        
        v1 =  av1;
        v2 = av2;
        
        v1 = v1 * (*_mv);
        v2 = v2 * (*_mv);
		
		ov1 = v1;
		ov2 = v2;
		
        
        float z = (v1[2] + v2[2]) / 2.0f;
        bool clipped = false;
        if (!_frustum.project(v1,v2)) // Linie nicht im view-Frustum ...
            clipped = true;
        
		if (_mode == GL_QUADS) {
			v1 = ov1;
			v2 = ov2;
		}
		
		if (_debugVertices.valid() && (!clipped || _mode==GL_QUADS)) {
			_debugVertices->push_back(v1);
			_debugVertices->push_back(v2);
		}
		
        v1 = v1 * (*_proj);
        v1[0] *= _width;
        v1[1] *= _height;
        

        v2 = v2 * (*_proj);
        v2[0] *= _width;
        v2[1] *= _height;
        if (!clipped || _mode==GL_QUADS) {
			_list.back().add(v1,v2,z);
			if (clipped) _list.back().incClipCount();
        }
    }

    /** "writes" a vertex to the file,
     *  adds the vertex to the last line-stack and constructs necessary helper-points */
    void PSWVertexWriter::_write(const osg::Vec3 v) {

        _numVertices++;
        
        if (0 == _vertIdx) {
            PSLineStack ls(!_wireframe);
			if (_colorHelper.valid()) 
				setColor(_colorHelper->getColor());
				
			ls.setColor(_currentColor);
            _vertIdx++;
            _lastV= v;
            if ((_mode == GL_QUADS) || (_mode == GL_TRIANGLES) || (_mode == GL_LINE_LOOP))
                ls.setClosed();
                
            _list.push_back(ls);
            
            return;
        } else {
            switch (_mode) {
				case GL_LINE_LOOP:
					_drawLine(_lastV, v);
                    _lastV = v;
                    _vertIdx++;
                    break;

                case GL_LINE_STRIP:
                    _drawLine(_lastV, v);
                    _lastV = v;
                    _vertIdx++;
                    if ((_wireframe) && (0 == (_vertIdx % 2))) {
                        PSLineStack ls;
						ls.setColor(_currentColor);
                        _list.push_back(ls);
						
                    }
                    break;
                
                case GL_LINES:
                    _drawLine(_lastV, v);
                    _lastV = v;
                    _vertIdx=0;
                    break;
                    
                case GL_TRIANGLES:
                    _drawLine(_lastV, v);
                    _lastV = v;
                    _vertIdx++;
                    if (_vertIdx ==3) _vertIdx=0;
                    break;
                    
                case GL_TRIANGLE_STRIP:
                    _drawLine(_lastV, v);
                    _vertIdx++;
                    if (_vertIdx > 2) {
                        _drawLine(v, _lastLastV);
                        PSLineStack ls;
						ls.setColor(_currentColor);
                        _list.push_back(ls);
                    }
                    _lastLastV = _lastV;
                    _lastV = v;
                    break;
                
                case GL_QUADS:
                    _drawLine(_lastV, v);
                    _lastV = v;
                    _vertIdx++;
                    if (_vertIdx ==4) _vertIdx=0;
                    break;
                    
                default:
                    osg::notify(osg::INFO) << "Postscriptwriter: don't know how to write mode " << _mode << std::endl;
                    break;
            }
        }             
                
        
    }

    /**
     * writes all collected linestacks to the file
     */
    void PSWVertexWriter::writeToFile() {
		if (_list.size() == 0) return;
		
        _list.sort();
        
        float minZ,maxZ;
        minZ = _list.front().getZ();
        maxZ = _list.back().getZ();
        /*
        List::iterator first = _list.begin();
        while (first->getZ()<=0)
            first++;
        
        minZ = first->getZ();
        List::iterator last = _list.end()--;
        while (last->getZ() == 0)
            last--;
        
        maxZ = last->getZ();
        */
        osg::notify(osg::INFO) << _list.size() << " primitives " << " min-z: " << minZ << " max-z: " << maxZ <<std::endl;
        
        osg::Vec4 backColor(0.7,0.7,0.7,1);
        osg::Vec4 frontColor(0,0,0,1);
        
        backColor = Settings::instance()->getPostscriptWriterBackcolor();
        frontColor = Settings::instance()->getPostscriptWriterFrontcolor();
        osg::notify(osg::INFO) << "forecolor: " << frontColor << std::endl;
        osg::notify(osg::INFO) << "backcolor: " << backColor << std::endl;
        
        double invscalar, scalar,r,g,b;
        for (List::iterator i=_list.begin(); i != _list.end(); i++) {
            float z = i->getZ();
            
                if (fabs(maxZ-minZ) < 0.0001)
					scalar = 1;
				else
					scalar =  (z-minZ) / (maxZ-minZ);
                invscalar = 1 - scalar;
                r = backColor[0] * invscalar + frontColor[0] * scalar;
                g = backColor[1] * invscalar + frontColor[1] * scalar;
                b = backColor[2] * invscalar + frontColor[2] * scalar;
                
                // osg::notify(osg::INFO) << "z:" << z << " -> " << scalar << " color: " << r << "/" << g << "/" << b << std::endl;
                
                if (i->valid())
					_output << r << " " << g << " " << b << " setrgbcolor" << std::endl;
				
                _output << (*i);
            
        }
    }

            
    #pragma mark - 
    
    
    PostScriptWriter* PostScriptWriter::install(osgUtil::SceneView* sv) {
         PostScriptWriter* psw = new PostScriptWriter(sv);
        if ((sv) && (sv->getSceneData())) {
            sv->getSceneData()->setCullCallback(psw);
		}
        else {
            log::error("PostScriptWriter::install") << "no valid SceneData found" << std::endl;
        }
		return psw;
    }


    PostScriptWriter::PostScriptWriter(osgUtil::SceneView* scene) : osg::NodeCallback(), 
            _scene(scene), 
            _outputPath(""), 
            _written(0),
            _writeFlag(false),
            _countDrawables(0),
            _countMatrices(0),
            _activationChar('p'),
			_wireframe(false)
    {
        cefix::NotificationCenter::instance()->subscribe("keyPress", this,&PostScriptWriter::handleKeyPressMessage);
        _outputPath = Settings::instance()->getPostscriptWriterOutputPath();

    }
	
	
	PostScriptWriter::~PostScriptWriter() {
		cefix::NotificationCenter::instance()->unsubscribe("keyPress", this,&PostScriptWriter::handleKeyPressMessage);
        
	}
	

    void PostScriptWriter::handleKeyPressMessage(const int& key) 
	{                
        if (key == _activationChar)
            _writeFlag = true;
    }

    void PostScriptWriter::writeRenderLeaf(PSWVertexWriter *psvwr, osgUtil::RenderLeaf* rl) {
        osg::Drawable* dw= rl->_drawable.get();

        if (rl->_modelview.get()) {
            psvwr->setModelViewMatrix(rl->_modelview.get());
            _countMatrices++;
        }
        if (rl->_projection.get()) {
            psvwr->setProjectionMatrix(rl->_projection.get());
        }
        if ((dw) && (dw->asGeometry()))
        {   
            osg::Geometry* geo = dw->asGeometry();
            osg::Vec3Array* sourceVec = dynamic_cast<osg::Vec3Array*>(geo->getVertexArray());
		
            
            if (!sourceVec) {
                log::error("PostScriptWriter::writeRenderLeaf")  << "could not write drawable, no vertices ..." << std::endl;
                return;
            }
			
			if (geo->getColorBinding() == osg::Geometry::BIND_PER_VERTEX) {
				log::error("PostScriptWriter::writeRenderLeaf") << "Colorbinding BIND_PER_VERTEX not supported! " << std::endl;
			}
			
			// farbe setzen
			if ((geo->getColorBinding() == osg::Geometry::BIND_PER_VERTEX) || (geo->getColorBinding() == osg::Geometry::BIND_OVERALL)) {
				if (geo->getColorArray()) {
					osg::Vec4Array* c = dynamic_cast<osg::Vec4Array*>(geo->getColorArray());
					if ((c) && (c->size() > 0)) psvwr->setColor((*c)[0]);
				}
				else
					psvwr->setColor(osg::Vec4(0,0,0,1));
			}
        
            psvwr->setVertexArray(sourceVec->getNumElements(),static_cast<const osg::Vec3*>(sourceVec->getDataPointer()));
            
			if (geo->getColorArray() && (geo->getColorBinding() == osg::Geometry::BIND_PER_PRIMITIVE))
				psvwr->setColorArray(dynamic_cast<osg::Vec4Array*>(geo->getColorArray()));
				
            // alle Primitivesets durchgehen
            for (unsigned int i = 0; i <  geo->getNumPrimitiveSets(); ++i) {
				
				if (geo->getColorBinding() == osg::Geometry::BIND_PER_PRIMITIVE_SET) {
					osg::Vec4Array* c = dynamic_cast<osg::Vec4Array*>(geo->getColorArray());
					psvwr->setColor((*c)[i]);
				}
				
                osg::PrimitiveSet* ps = geo->getPrimitiveSet(i);
                ps->accept(*psvwr);
                _countDrawables++;
                                
            }
        }
    }


    void PostScriptWriter::writeStateGraph(PSWVertexWriter *psvwr, osgUtil::StateGraph* graph) {

        for(osgUtil::StateGraph::LeafList::iterator dw_itr = graph->_leaves.begin();
            dw_itr != graph->_leaves.end();
            ++dw_itr)
        {
            osgUtil::RenderLeaf* rl = dw_itr->get();
            writeRenderLeaf(psvwr, rl);
            
        }

    }


    void PostScriptWriter::writeRenderBin(PSWVertexWriter *psvwr, osgUtil::RenderBin *bin) {

        osgUtil::RenderBin::RenderBinList::iterator rbitr;
        osgUtil::RenderBin::RenderBinList binList = bin->getRenderBinList();
        for(rbitr = binList.begin();
            rbitr!=binList.end() && rbitr->first<0;
            ++rbitr)
        {
            writeRenderBin(psvwr, rbitr->second.get());
        }
        

        // draw fine grained ordering.
        osgUtil::RenderBin::RenderLeafList leafList = bin->getRenderLeafList();
        for(osgUtil::RenderBin::RenderLeafList::iterator rlitr=leafList.begin();
            rlitr!= leafList.end();
            ++rlitr)
        {
            osgUtil::RenderLeaf* rl = *rlitr;
            writeRenderLeaf(psvwr, rl);
        }

      
        osgUtil::RenderBin::StateGraphList StateGraphList = bin->getStateGraphList();
        for(osgUtil::RenderBin::StateGraphList::iterator oitr=StateGraphList.begin();
            oitr!=StateGraphList.end();
            ++oitr)
        {
            
            writeStateGraph(psvwr, (*oitr));
            
        }
        
        // write post bins.
        for(;
            rbitr!=binList.end();
            ++rbitr)
        {
            writeRenderBin(psvwr, rbitr->second.get());
        }
    }


    void PostScriptWriter::write(const std::string file) {
	
		osg::Vec3Array* v(NULL); // = new osg::Vec3Array();

        std::ostringstream ss;
        ss <<  std::setfill('0') << std::setw(4) << _written;    
        std::string filename = (file.empty()) ?  _outputPath+"output_"+ss.str()+".eps" : file;
        
        PSWVertexWriter psvwr(filename, _scene.get());
		psvwr.setDebugVertices(v);
		psvwr.setWireFrameMode(_wireframe);
        
		// jetzt alles aus der SceneView rauspopeln
        osgUtil::RenderStage* rs = _scene->getRenderStage();
        writeRenderBin(&psvwr, rs);
        psvwr.writeToFile();
		
		if (v) {
			osg::Geode* geode = new osg::Geode();
			osg::Geometry* geo = new osg::Geometry();
			geo->setVertexArray(v);
			osg::Vec4Array* c = new osg::Vec4Array();
			c->push_back(osg::Vec4(1,0,0,1));
			_scene->setSceneData(geode);
			
			geo->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, v->size()));
			geode->addDrawable(geo);
		}
        
        osg::notify(osg::INFO) << "written " << (psvwr.getVerticesCount()-psvwr.getClippedCount()) << " vertices (" << psvwr.getClippedCount() << " clipped), " <<_countDrawables << " drawables and " << _countMatrices << " matrices" << std::endl;
        // finished ...
        _written++;
        
    }
}
