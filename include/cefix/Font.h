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


#ifndef FONT_HEADER
#define FONT_HEADER

#include <string>
#include <osg/Referenced>
#include <cefix/Log.h>
#include <osg/Image>
#include <osg/Vec3>
#include <osg/Texture2D>

#include <cefix/FontCharData.h>
#include <cefix/FontGlobalData.h>
#include <cefix/Export.h>

using namespace std;

class TiXmlNode;


namespace osg {
	class Geometry;
        class StateSet;
        class Geode;
}


namespace cefix {


	/**
	 * this class encapsulates all information required to render text with this font
	 * the font-kernings are loaded from a xml-file, the font-texture from a png.
	 *
	 * rendering text is done by a TextRenderer-object. See the doc there.
	 * @see TextRenderer
	 *
	 * @author Stephan Maximilian Huber
	 */
	class CEFIX_EXPORT Font: public osg::Object {
		public:
			typedef std::vector< osg::ref_ptr< osg::Image > > ImageVector;
			typedef std::vector< osg::ref_ptr< osg::Texture > > TextureVector;
            typedef std::map<unsigned int, osg::ref_ptr<FontCharData> >     CharList;
			
			/** default constructor */
			Font();
			
			/** Copy constructor using CopyOp to manage deep vs shallow copy. */
			Font(const Font& font,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);

			virtual Object* cloneType() const { return new Font(); }
			virtual Object* clone(const osg::CopyOp& copyop) const { return new Font(*this,copyop); }
			virtual bool isSameKindAs(const osg::Object* obj) const { return dynamic_cast<const Font*>(obj)!=0; }
			virtual const char* libraryName() const { return "cefix"; }
			virtual const char* className() const { return "Font"; }

			
			/** @return the filename/identiefier of this font */
			const string& getName() { return _fileName; }
			
			void setName(const std::string& name) { _fileName = name; }
			/**
			 * loads the font from the specified location
			 * @param fileName the filename of the xml-font-specification
			 */
			bool load(const string& fileName) ;
			
			/**
			 * computes the width of a specified text
			 * this code is very simple, it does not handle any formatting etc.
			 * @param text the text to measure the width
			 */
			template<class T>float getWidth(const T& text) 
			{
				float len = 0;
				int i,m;
				
				m = text.length();
				
				for (i=0;i<m;i++) {
					unsigned int ch = text[i];
					if (ch > 31)  {
						len += getCharWidth( ch, i < (m-1) ? text[i+1] : 0);
					}
				}
				
				return len;
			}
		
		
			
			/** @return the size of the font */
			inline float getNativeFontSize()  {return _fontData->getNativeFontSize(); }
			
			inline float getFontSize() { return _fontData->getCurrentFontSize(); }
			
			/** @return the prescaling-factor */
			inline float getPreScale() const { return _fontData->preScale();}
			
			/** 
			 * computes the width of a char, in relation to the following char (applies kerning-information)
			 * @param left the left char
			 * @param right the char following left
			 * @return the width of the char
			 */
			inline float getCharWidth( unsigned int left, unsigned int right)  {
				return (_subPixelAlignment) ? getCharData(left)->computeWidth(right) * _fontData->preScale() : (int)(getCharData(left)->computeWidth(right) * _fontData->preScale()); 
			}
            
			/** sets the prescale @param scale  the new prescale-value    */
			inline void setPreScale(float scale) { _fontData->setPreScale(scale); }
			
			/** sets the fontsize @ param pointSize the new size in points */
			inline void setFontSize(float pointSize) { _fontData->setFontSize(pointSize); }
			
			/** @return the font-texture-object */
			inline osg::Texture* getTexture(unsigned int texID = 0) { return _textures[texID].get(); }
			
			/**
			 * creates a standard stateset (no lighting, blending enabled, etc) to render text correctly
			 * @return the newly created Stateset
			 */
			osg::StateSet* getStandardStateSet(unsigned int texId = 0);
			
			/** @return the stored dato for a specifed char */ 
			inline FontCharData* getCharData(unsigned int ch) { 
                CharList::iterator itr = _chars.find(ch);
				return (itr == _chars.end()) ? getUndefinedChar(ch) : itr->second.get(); 
            } 
            
			inline FontCharData* getUndefinedChar(unsigned int ch) { 
				FontCharData* created = createFontCharData(ch);
                if (!created && _reportMissingCodePoints) reportMissingCodePoints(ch);
				return created ? created : _undefinedChar.get();
			}
			
			void setSubPixelAlignment(bool flag) {
				_subPixelAlignment = flag; 
			}
			
			bool getSubPixelAlignment() {
				return _subPixelAlignment;
			}
			
			/** returns the height of a "X" */
			double getMajuscleHeight() {
				return  ((_fontData->getMajuscleHeight() >= 0) ? _fontData->getMajuscleHeight() : computeMajuscleHeight()) * _fontData->preScale();
			}
			
			/** returns the height of a "x" */
			double getMinuscleHeight() {
				return  ((_fontData->getMajuscleHeight() >= 0) ? _fontData->getMinuscleHeight() : computeMinuscleHeight()) * _fontData->preScale();
			}
						
			ImageVector& getImages() { return _images; }
            const ImageVector& getImages() const { return _images; }
                            
            CharList::iterator begin() { return _chars.begin(); }
            CharList::iterator end() { return _chars.end(); }
            
            FontGlobalData* getFontData() { return _fontData; }
                                		
		protected:
            virtual FontCharData* createFontCharData(unsigned int ch) { return NULL; }
			/**
			 * reads the font-files
			 * @return true if successful
			 */
			bool _readFont();
			
			/**
			 * extracts the chars' data out of a xml-node
			 * @param charRootNode the root-node for the chars' data
			 * @param true, if successful
			 */
			bool _extractCharData(TiXmlNode* charRootNode);  
			/**
			 * creates the font-texture
			 * @param useRectangleExtensionFlag true if a rectangular texture should be created
			 */
			void _buildTexture(bool useRectangleExtensionFlag);
			
			double computeMinuscleHeight();
			double computeMajuscleHeight();
			
			void reportMissingCodePoints(unsigned int ch);

			virtual ~Font() {
				//osg::notify(osg::ALWAYS) << "clearing Font: " << getName() << std::endl;
				_chars.clear();
			}
			
            /** the global font data */                
			osg::ref_ptr<FontGlobalData> _fontData;
			
			
			/** the images storing the font-textures */
			ImageVector _images;
			
			/** the textures holding the font-textures */
			TextureVector _textures;
			
                
			/** the list of chars */
			CharList _chars;
			/** the filename of the xml-font-file */
			std::string _fileName;

			osg::ref_ptr<FontCharData> _undefinedChar;
			
			bool _subPixelAlignment;
			bool _reportMissingCodePoints;
		

	};

}

#endif

