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

#ifndef __TEXT_ON_SPINE_RENDERER__
#define __TEXT_ON_SPINE_RENDERER__

#include <cefix/StandardTextRenderIterator.h>
#include <cefix/Curve.h>

namespace cefix {
	/** namespace encapsulating the different Resolvers for the TextOnSplineRenderIterator-Template */
	namespace TextOnSplineResolvers {
		
		/** this class represents a simple linestrip with one upvector for the whole linestrip */
		class CEFIX_EXPORT SimpleLineStrip {
		
			public:
				/** CTOR */
				SimpleLineStrip(osg::Vec3Array* linestrip, osg::Vec3 upVector) 
					: 
					_linestrip(linestrip), 
					_upVector(upVector),
					_linestripSize(linestrip->size()),
					_lastDistance(0.0),
					_lastNdx(0)
				{
				}
				
				/** operator() get the point on the spline and its vector */
				bool operator()(double distanceOnSpline, osg::Vec3& pointOnSpline, osg::Vec3& upVector);
			
			protected:
			    /** helper method to find the point on a linstrip at length posOnSpline */
				bool findSegment(double posOnSpline, unsigned int& ndx, osg::Vec3& a, osg::Vec3& b); 

				osg::ref_ptr<osg::Vec3Array> _linestrip;
				osg::Vec3	_upVector;
				unsigned int _linestripSize;
				double _lastDistance;
				unsigned int _lastNdx;
		
		};
		/** this class represents a linestrip and a same-sized vector of up-vectors */
		class CEFIX_EXPORT LineStrip : public SimpleLineStrip {
			
			public:
				/** ctor */
				LineStrip(osg::Vec3Array* linestrip, osg::Vec3Array* upvectors) :
					SimpleLineStrip(linestrip, osg::Vec3(1,1,1)), 
					_upVectors(upvectors)
				{
					//std::cout << "LineStrip" << std::endl;
				}
				
				/** operator() get the point on the spline and its vector */
				bool operator()(double distanceOnSpline, osg::Vec3& pointOnSpline, osg::Vec3& upVector);
				
			protected:
				osg::ref_ptr<osg::Vec3Array> _upVectors;
		};
		
		
		/** a resolver-adapter for curve based classes */
		class Curve {
		
			public:
				/** ctor */
				Curve(cefix::spline::Curve* curve, osg::Vec3 upVector) 
					: 
					_curve(curve), 
					_upVector(upVector)
				{
				}
				
				/** operator() get the point on the spline and its vector */
				bool operator()(double distanceOnSpline, osg::Vec3& pointOnSpline, osg::Vec3& upVector) {
					if (distanceOnSpline > _curve->getLength())
						return false;
				
					_curve->getWorldPosition(pointOnSpline, distanceOnSpline);
					upVector = _upVector;
					return true;
				}
			
			protected:
				 

				osg::ref_ptr<cefix::spline::Curve> _curve;
				osg::Vec3	_upVector;
				unsigned int _segment;


		};
	}
	
	/** template RenderIterator-template-class, renders a bunch of characters onto a spline. The characteristics of the spline is described by the template-class-parameter,
	    it provides an operator() which computes the position on a spline and its up-vector for a given length, the Renderiterator knows nothing about the
		internals of the spline, what the resolver must provide is an upvector and a position for a given distance on the spline. */
    template <class SplineResolver=TextOnSplineResolvers::SimpleLineStrip, bool kdebug = true>class TextOnSplineRenderIteratorT : public StandardTextRenderIterator {
        protected:
			/** DebugGeometry visualizing the normals and up-vectors of the rendered text */
			class DebugGeometry : public osg::Geometry {
			
				public:
					DebugGeometry() : osg::Geometry() {
						getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
						getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::OFF);
						
						_v = new osg::Vec3Array();
						_c = new osg::Vec4Array();
						_da = new osg::DrawArrays(GL_LINES, 0,0);
						
						addPrimitiveSet(_da);
						setVertexArray(_v);
						setColorArray(_c);
						setColorBinding(BIND_PER_PRIMITIVE);
					}
					
					void addBaseLine(const osg::Vec3& a, const osg::Vec3&b, const osg::Vec3& up_a, const osg::Vec3& up_b ) {
						_v->push_back(a);
						_v->push_back(b);
						_c->push_back(osg::Vec4(0.5,0.5,0.5,0.8));
						
						_v->push_back(a);
						_v->push_back(a+up_a);
						_c->push_back(osg::Vec4(0.5,0,0,0.8));
						
						_v->push_back(b);
						_v->push_back(b+up_b);
						_c->push_back(osg::Vec4(0.5,0,0,0.4));
						
						update();
					}
					
					void update() {
						dirtyDisplayList();
						dirtyBound();
						_da->setCount(_v->size());
					}
				private:
					// wir sparen uns das ref_ptr hier, da sie eh teil der geometry sind 
					osg::Vec3Array*	_v;
					osg::Vec4Array* _c;
					osg::DrawArrays* _da;
									
			};
		public:
			
			/** ctor */
            TextOnSplineRenderIteratorT(const SplineResolver& splineResolver) 
			:	
				StandardTextRenderIterator(),
				_splineResolver(splineResolver),
				_startPositionOnSpline(0.0f)
			{	
				reset();
				if (kdebug) _debugGeometry = new DebugGeometry();
			}
			
			/** get the spline-resolver */
            const SplineResolver& getSplineResolver() { return _splineResolver; }
            
			/** reset this iterator */
            virtual void reset() 
			{
				_positionOnSpline = _startPositionOnSpline;
				resetContainer();
			}
			
			/** implementation of Iterator::advance */
            virtual void advance(const unsigned int &left, const unsigned int &right) 
			{
				if (left>31) 
					_positionOnSpline += _font->getCharWidth(left,right) + _spatio;
			}
			
			/** implementation of Iterator::render, computes the position of the glyph on the spline */
            virtual void render(const unsigned int &ch)
			{
				FontCharData* charData = _font->getCharData(ch);
				_container->select(charData->getTexID());
				
				// Berechnung des Quads
				
				float yDelta = - _font->getFontSize() /3.0f;
				float charWidth = charData->charWidth() * _font->getPreScale();
			
				osg::Vec3 a,b;
				osg::Vec3 up_a,up_b;
			
				if (!_splineResolver(_positionOnSpline, a, up_a))
					return;
		
				if (!_splineResolver(_positionOnSpline + charWidth ,b, up_b))
					return;
				
				osg::Vec3 weightedUp = (up_a+up_b) * 0.5f;
				osg::Vec3 baseLine(b-a);
				
				if (kdebug) _debugGeometry->addBaseLine(a,b, up_a, up_b);
				
				if (ch <= 32)  // nicht weiter rendern, das ist kein sichtbares Zeichen
					return; 

				charData->addTexCoords(_container->getTexCoords());
				
				// Koordinaten des Quads (lokal)
				osg::ref_ptr<osg::Vec3Array> quad = charData->getVertices(0, yDelta, FontCharData::XY_PLANE);

				
				baseLine.normalize();
				
				// ideales Normal für das buchstabenquad ermitteln:
				osg::Vec3 n(weightedUp ^ baseLine);
				n.normalize();
				
				osg::Vec3 c(baseLine ^ n);
				c.normalize();
				
				osg::Matrix m;
				m.identity();
				m.makeLookAt(osg::Vec3(0,0,0) , n, c);
				m = osg::Matrix::inverse(m);
				m *= osg::Matrix::translate(a);
			   

				for (unsigned int i=0; i < quad->size(); ++i)
					_container->getVertices()->push_back( (*quad)[i]* m );
					
				_container->getNormals()->push_back(-n);

			}
            
			/** return the last position on the spline */
			float getLastPositionOnSpline() { return _positionOnSpline; }
            
			/** sets the start position for the rendering, 0 = beginning of the spline */
			void setStartPositionOnSpline(double p) { _startPositionOnSpline = p; }
			
			/** get the start position */
			double getStartPositionOnSpline() { return  _startPositionOnSpline; }
			
			/** implemetation of Iterator::finish */
			virtual void finish() {
				getContainer()->setNormalBinding(osg::Geometry::BIND_PER_PRIMITIVE);
				getContainer()->dirty();
				
				if ( _fontChanged || (!_container->hasStateSet())) {
					_container->setStateSetForFont(_font.get());
					_fontChanged = false;
				}
				
			}

			/** returns the debug-geometry, if debugging is enabled */
			osg::Geometry* getDebugGeometry() { return _debugGeometry.get(); }
        protected:
		
			
        
			double _positionOnSpline;
			double _startPositionOnSpline;
			SplineResolver	_splineResolver;
			osg::ref_ptr<DebugGeometry>	_debugGeometry;

    
    };
	
	typedef TextOnSplineRenderIteratorT<> TextOnSplineRenderIterator;



}

#endif

