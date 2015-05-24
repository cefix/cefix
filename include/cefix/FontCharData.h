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

#ifndef __FONT_CHAR_DATA_HEADER__
#define __FONT_CHAR_DATA_HEADER__

#include <osg/Referenced>
#include <vector>
#include <map>
#include <osg/Array>

#include <cefix/FontGlobalData.h>
#include <cefix/Export.h>

class TiXmlElement;


namespace cefix {

	/**
	 * this class encapsulates all information related to a single char of a font
	 * this includes texture-coordinates, kerning etc.
	 *
	 * every char gets rendered as a textured quad, the quad has the smallest size possible,
	 * this size is defined in the xml-font-definition. The quad gets textured with the font-texture
	 * the texture-coords are computed accordingly
	 * 
	 * @author Stephan Maximilian Huber
	 */
	class CEFIX_EXPORT FontCharData : public osg::Referenced {
        
		protected:
            /** default constructor, not for public use -- it constructs an invalid char */
            FontCharData();
			virtual ~FontCharData();
            
		public:
			
			enum RenderPlane { 
				XZ_PLANE = 10,		/**< text should be rendered on the XZ-plane */
				XY_PLANE = 20,		/**< text should be rendered on the XY-plane */ 
                YZ_PLANE = 30,		/**< text should be rendered on the YZ-plane */ 
                NEGATIVE_XZ_PLANE = 40,
                NEGATIVE_XY_PLANE = 50,
                NEGATIVE_YZ_PLANE = 60
                
			};

   			typedef  std::map<unsigned int, float> KerningTable;

            
                
            /** 
			 * Constructor, reads its information out of an XML-node, 
			 * @param node the xml-node describing this char
			 * @param fontData some global font-information
			 */    
			FontCharData(TiXmlElement* node, FontGlobalData* fontData);
			
			/** @return the ID of this char (the ascii-code) */
			inline unsigned int ID() const { return _id;}
            
            bool operator<(const FontCharData& y) const { return (_id < y.ID()); }
			
			/** @return the width of the char */
			inline float charWidth() const { return _charWidth;}
			
			/** @return the Kerning-delta to the char toID */
			inline float getKerningTo(unsigned int toID) const { 
                KerningTable::const_iterator itr = _kernings.find(toID); 
                return (itr != _kernings.end()) ? itr->second : 0;
                /*
                if (itr != _kernings.end())
                    return itr->second;
                else
                    return 0;
                */
            }
            
			/** @return computes the widht for this char in relation to the following char toID */
			inline float computeWidth(unsigned int toID) const { return charWidth() + getKerningTo(toID); }
			
			/** @return an array of vertices describing the char-quad */
			osg::Vec3Array* getVertices(int s, int t, RenderPlane renderPlane);
			
			/** @return an Array of texture coordinates to render this char correctly */
			osg::Vec2Array* getTexCoords();
			
			/** 
			 * this function adds vertices to an existing array
			 * @param vertices existing osg::Vec3-array to append
			 * @param s x-delta
			 * @param t y-delta
			 * @param renderPlane render to specified plane
			 * @param startLoc starting location
			 */
			void addVertices(osg::Vec3Array* vertices, float s, float t, RenderPlane renderPlane, const osg::Vec3 &startLoc);
			
			/** 
			 * this function adds vertices to an existing array
			 * @param vertices existing osg::Vec3-array to append
			 * @param s x-delta
			 * @param t y-delta
			 * @param renderPlane render to specified plane
			 */
			inline  void addVertices(osg::Vec3Array* vertices,float s, float t, RenderPlane renderPlane)
				{ addVertices(vertices,s,t,renderPlane,_fontData->startPosition()); }
			/**
			 * adds the texture coordinates for this char
			 * @param texCoords exisiting Array of texture-coordinates
			 */
			void addTexCoords(osg::Vec2Array* texCoords);
			
			/**
			 * dump information for this char to the console
			 */
			void dump();
			
			inline unsigned int getTexID() const { return _texId; }
			
			void setDummyValues(FontGlobalData* data);
            
            int getTexLeft() const { return _texLeft; }
            int getTexTop() const { return _texTop; }
            int getTexWidth() const { return _texWidth; }
            int getTexHeight() const { return _texHeight; }
            int getBaseLeft() const { return _baseLeft; }
            int getBaseTop() const { return _baseTop; }
            
            const KerningTable& getKerningTable() const { return _kernings; }

		protected:
			/** read the kernings out of a xml-node
			 * @param node the xml-node for this char
			 * @param tagName name of the tag describing the kernings (Kernings/CustomKernings)
			 */
			void _readKernings(TiXmlNode* node,const char* tagName);
            /** the id of the char -- the unicode-codepoint */    
			unsigned int _id;
			/** the width of the char */
			float _charWidth;
			int _baseLeft,_baseTop;
			int _texLeft,_texTop,_texWidth,_texHeight;
			unsigned int _texId;

			/** vector of kerning-information */
			KerningTable _kernings;
            /** reference of font-global-data */    
			osg::ref_ptr<FontGlobalData> _fontData;
            
            
            friend class Font;
	};


}

#endif

