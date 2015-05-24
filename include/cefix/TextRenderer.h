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

#ifndef __TEXT__RENDERER_HEADER__
#define __TEXT__RENDERER_HEADER__

#include <osg/Referenced>
#include <osg/ref_ptr>
#include <osg/Geometry>
#include <osg/StateSet>
#include <cefix/Log.h>
#include <osg/Geode>

#include <cefix/Font.h>
#include <cefix/Export.h>




namespace cefix {

/** 
 * this class is used to render text, it creates a geometry-object
 * for this, it ueses an Iterator which steps char for char through the text-
 * information and adds the vertices-data to the geometry-object
 *
 * @author Stephan Maximilian Huber 
 */
class CEFIX_EXPORT TextRendererBase : public osg::Referenced {

	public:
		
		class CEFIX_EXPORT Container : public osg::Referenced {
			public:
				typedef std::map<unsigned int, osg::ref_ptr<osg::Geometry> > ContainerMap;
				Container();
				
				inline bool select(unsigned int i) { 
					if (i != _currentId) {
						if (_container.find(i) == _container.end()) {
							_container[i] = createNewContainer();
						}
						switchToContainer(i);
						return true;
					}
					return false;
				}
				
				inline osg::Geometry* getGeometry() { return _currentGeometry.get(); }
				inline osg::Vec3Array* getVertices() { return _currentVertices.get(); }
				inline osg::Vec3Array* getNormals() { return _currentNormals.get(); }
				inline osg::Vec4Array* getColors() const { return _currentColors.get(); }
				inline osg::Vec2Array* getTexCoords() { return _currentTexCoords.get(); }
				
				
				
				void reset();
				
				void dirty();
				
				void setOverallNormal(const osg::Vec3& normal);
				
                void setStateSetForFont(Font* font);
				
                void clearStateSets();
                void setStateSet(osg::StateSet* stateset);
            
				inline bool hasStateSet() { return _hasStateSet; }
				
				void setName(const std::string& name);
				
				void setColorArray(osg::Vec4Array* colors);
				
				void setColorBinding(osg::Geometry::AttributeBinding binding);
				
				void setNormalBinding(osg::Geometry::AttributeBinding binding);
				
				void setSupportsDisplayList(bool f);
				
				void attachTo(osg::Geode* geode) { addDrawablesTo(geode); _geode = geode; }
                void detachFrom(osg::Geode* geode) { removeDrawablesFrom(geode); _geode = NULL; }
				
                void dirtyDisplayList();
				
				osg::Drawable::UpdateCallback* getUpdateCallback() { return _callback.get(); }
				
                void setUpdateCallback(osg::Drawable::UpdateCallback* cb);
				
                void updateDataVariance();
               
			protected:
				virtual ~Container();
			
            void addDrawablesTo(osg::Geode* geode);
            void removeDrawablesFrom(osg::Geode* geode);
				
				osg::Geometry* createNewContainer();
				
				void switchToContainer(unsigned int i) 
				{
					_currentId = i;
					update(_container[i].get());
				}
				
				void update(osg::Geometry* geo);
				
									
		
			private:
				osg::ref_ptr<osg::Geometry>		_currentGeometry;
				osg::ref_ptr<osg::Vec3Array>	_currentVertices;
				osg::ref_ptr<osg::Vec2Array>	_currentTexCoords;
				osg::ref_ptr<osg::Vec3Array>	_currentNormals;
				osg::ref_ptr<osg::Vec4Array>	_currentColors;
				osg::ref_ptr<osg::DrawArrays>	_currentDrawArray;
				unsigned int								_currentId;
				ContainerMap					_container;
				bool							_hasStateSet;
				std::string						_name;
				osg::Geometry::AttributeBinding	_colorBinding;
				osg::ref_ptr<osg::Vec4Array>	_colors;
				bool							_useDisplayLists;
				osg::observer_ptr<osg::Geode>	_geode;
				osg::ref_ptr<osg::Drawable::UpdateCallback>	_callback;
				osg::Geometry::AttributeBinding	_normalBinding;
				
				
		};
		
		#pragma mark *** IteratorBase ***
		
		/**
		 * this abstract class does the hard work -- it steps through the text charwise 
		 * and adds the different kind of vertices to a geometry-object
		 * If you want custom rendering, then you should use this class as your base-class
		 */
		class CEFIX_EXPORT IteratorBase : public osg::Referenced {
			public:
				/** ctor */
				IteratorBase();
									
				/** resets the Iterator, must be implemented  */
				virtual void reset() = 0;
				
				virtual void begin() {};
				
				/** 
				 * this function  gets called from the textrenderer to advance to the next char
				 * here you should implement the movement of the "cursor"
				 * @param left the current char
				 * @param right the following char
				 */
				virtual void advance(const unsigned int &left, const unsigned int &right) = 0;
				
				/** @return true, if this iterator needs an extra preflight-step over the data */
				virtual bool needsPreflight() { return false; }
				
				/** preflight the char to be rendered */
				virtual void preflight(const unsigned int &left, const unsigned int &right) {};
				
                virtual void prerender(const unsigned int& ch) {}
				/**
				 * renders the current char
				 */
				virtual void render(const unsigned int &ch) = 0;
				
				/** sets the current font to
				 * @param font the new font to use
				 */
				inline void setFont(Font* font) { _font = font;  _fontChanged = true; }
				
				/** @return the font currently used */
				inline Font* font() { return _font.get(); }
				
				/**
				 * sets the cursor to the position p (0 is the beginning)
				 */
				virtual void setTextPosition(unsigned int &p) { _textPosition = p; }
				
				/** @return the current cursor-position inside the text-stream */
				inline const unsigned int &position() { return _textPosition; }
				
				/** do some work to finish the rendering */
				virtual void finish() {};
				
				/**
				 * sets the container-geometry, 
				 * @param container the new geometry-object 
				 */
				inline void setContainer(Container* container)  {
					_container = container;
				}
				/** @return the container-geometry */
				inline Container* getContainer() { return _container.get(); }
				
				/** @return true, if this iterator has an container attached */
				inline bool hasContainer() {
					return _container.valid();
				}
				
				/** resets the container */
				inline void resetContainer() {
					if (_container.valid())
						_container->reset();
				}
				
				/**
				 * sets the start-position
				 * @param pos the new start-position
				 */
				inline void setStartPosition(osg::Vec3 pos) { _startPos = pos; };
				
				inline void setRenderPlane(FontCharData::RenderPlane plane) { 
					_renderPlane = plane; 
					//std::cout << "setting renderplane to " << plane << std::endl;
					}
				inline FontCharData::RenderPlane getRenderPlane() const { return _renderPlane; }
				
				/**
				 * sets the linespacing to value
				 * @param value new linespacing
				 */
				inline void setLineSpacing(float value) { _lineSpacing = value; }
				
				/** @return the current used line-spacing */
				inline float getLineSpacing() const { return _lineSpacing; }

				/**
				 * sets the spatio
				 * @param spatio the new spatio
				 */
				inline void setSpatio(float spatio) { _spatio = spatio; }
				
				void setSubPixelAlignment(bool flag) { _subPixelAlignment = flag; }
				
				bool getSubPixelAlignment() const { return _subPixelAlignment; }
				
				/** return the nr of lines rendered */
				virtual int getRenderedLineCount() const { return 0; }
            
                virtual void startNewLine() {}
            
                bool isDirty() const { return _dirty; }
                void setDirty(bool f) { _dirty = f; }
            
                virtual void clearStateSets() { if (_container) _container->clearStateSets(); }
                virtual void setStateSet(osg::StateSet* ss) { if (_container) _container->setStateSet(ss); }
            
			protected:
				virtual ~IteratorBase() {}
				
            

                    
				
				/** used font */
				osg::ref_ptr<Font> _font;
									
				/** the container-geometry */
				osg::ref_ptr<Container> _container;
				
				unsigned int _textPosition;
				
				/** the starting-position */
				osg::Vec3                   _startPos;
				FontCharData::RenderPlane   _renderPlane;
				bool						_fontChanged;
				/** linespacing and spacio */
				float _lineSpacing, _spatio;
				bool  _subPixelAlignment, _dirty;
				
		};
		
	protected:
		TextRendererBase() : osg::Referenced() {}					
		
};
	
	
template <class IteratorClass> class TextRendererT : public TextRendererBase{

	public:
		typedef IteratorClass iterator;
		typedef typename iterator::TextAlignment TextAlignment;
		
		/*
		 * sets the render-iterator 
		 * @param itr the new render-iterator
		 */
		inline void setIterator(iterator* itr) { 
			osg::ref_ptr<Container> container = (_iterator.valid()) ? _iterator->getContainer() : NULL;
			_iterator = itr; 
			if (container.valid()) _iterator->setContainer(container.get());
		}
		
		/** @return the currently uses render-iterator */ 
		inline iterator* getIterator() { return _iterator.get(); }
		
		inline const iterator* getIterator() const { return _iterator.get(); }
		
		/** resets the currently used iterator */
		void reset() { if (_iterator.valid()) _iterator->reset(); }
		
		/**
		 * renders the text into the geometry using the iterator, if no container is specified a new one is created
		 * if you don't specify an iterator, the standard-iterator is used.
		 * @param text to render
		 * @param container the container holding all rendering
		 * @param itr the iterator to use, leave empty to use the standard iterator
		 */
		template<class T> void render(const T &text, Container* container = NULL, iterator* itr = NULL) {

			if ((itr) && (!_iterator.valid()))
				setIterator(itr);
			
			if (!_iterator.valid()) {
				log::error("TextRenderer::render ") << "no iterator specified! using standard "<< std::endl;                    
				setIterator(new iterator());
			}
			
			if ((container) && (!_iterator->hasContainer()))
				_iterator->setContainer(container);
			
			unsigned int m = text.length();
			unsigned int ch,nextch;
			
			_iterator->begin();
			
			// check, if this iterator needs to preflight the rendered data, e.g. for measuring the widths
			if (_iterator->needsPreflight()) {
				for (unsigned int i=0; i < m; ++i) {
					ch = text[i];
                    _iterator->setTextPosition(i);
					nextch = (i<m) ? text[i+1] : 0;
					_iterator->preflight(ch, nextch);
				}
			}
			//finally, start the rendering
			for (unsigned int i=0; i < m; ++i) {
				_iterator->setTextPosition(i);
				ch = text[i];
                _iterator->prerender(ch);
				_iterator->render(ch);
						   
				nextch = (i<m) ? text[i+1] : 0;
				_iterator->advance(ch,nextch);
			   
			}
			
			_iterator->finish();
		}
		
		
		/**
		 * renders a paragraph with the standard iterator
		 * @param font fotn to use
		 * @param size size of the font to use
		 * @param text text to render
		 * @return a container-object holding the rendered text
		 */
		inline Container* renderParagraph(Font* font, float size, const std::string text) 
			{ return renderParagraph(font, size, text, osg::Vec3(0,0,0)); }
		
		template<class T> inline Container* renderParagraph(Font* font, float size, const T text) 
			{ return renderParagraph<T>(font, size, text, osg::Vec3(0,0,0)); }
		
		/**
		 * renders a paragraph with the standard iterator
		 * @param font fotn to use
		 * @param size size of the font to use
		 * @param text text to render
		 * @param pos starting position of the text-rendering
		 * @return a geometry-object holding the rendered text
		 */       

		template<class T> Container* renderParagraph(Font* font, float size, const T text,osg::Vec3 pos) {
		
			if (!font) {
				log::error("TextRenderer::renderParagraph") << "could not render text because of invalid font! " << std::endl;
				return NULL;
			}
			
			osg::ref_ptr<Container> container = new Container();
			osg::ref_ptr<iterator> itr = (_iterator.valid()) ? _iterator.get() : new iterator();
			font->setFontSize(size);
			itr->setStartPosition(pos);
			itr->setFont(font);
			
			render(text, container.get(), itr.get());
			return container.release();
		}

	protected:
		virtual ~TextRendererT() {}
		
		/** currently used render-iterator */
		osg::ref_ptr<iterator> _iterator;

	
		

};
    

}




#endif