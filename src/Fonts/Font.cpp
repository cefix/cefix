/*
 *  Font.cpp
 *  NativeContext
 *
 *  Created by Stephan Huber on Wed Jan 08 2003.
 *  Copyright (c) 2003 digital mind. All rights reserved.
 *
 */
#include <cefix/Export.h>
#include <cefix/Font.h>
#include <cefix/Log.h>
#include <osg/Depth>
#include <osg/BlendFunc>
#include <osg/AlphaFunc>
#include <osg/Material>


#include <osgDB/FileUtils>
#include <osgDB/ReadFile>
#include <osgDB/FileNameUtils>

#include <osg/TexEnv>
#include <osg/Texture2D>
#include <osg/TextureRectangle>

#include <cefix/PropertyList.h>
#include <cefix/NotificationCenter.h>



// XML-Inludes:
#include <cefix/Settings.h>
#include <cefix/tinyxml.h>
#include <cefix/XMLWrapper.h>
#include <cefix/ImageUtilities.h>
#include <cefix/AllocationObserver.h>



namespace cefix {

class MissingCodePointsReporter : public osg::Referenced {

	public:
		typedef std::set<unsigned int> CodePointSet;
		typedef std::map<std::string, CodePointSet> FontMap;
		
		static MissingCodePointsReporter* instance();
		
		void report(const std::string& fontname, unsigned int codepoint) {
			_fontMap[fontname].insert(codepoint);
		}
		
		~MissingCodePointsReporter() {
			
			for(FontMap::iterator i = _fontMap.begin(); i != _fontMap.end(); ++i) 
			{
				writeCodePoints(i->first, i->second);
			}
		}
	protected:
		void writeCodePoints(const std::string& fontname, const CodePointSet& codepoints) {
			std::string filename = osgDB::getNameLessExtension(fontname) + ".cpl";
			std::ofstream f(filename.c_str());
			f << std::hex;
			for(CodePointSet::const_iterator i = codepoints.begin(); i != codepoints.end(); ++i) {
				f << (*i) << std::endl;
			}
		}
		
	private:

		
		FontMap	_fontMap;

};

MissingCodePointsReporter* MissingCodePointsReporter::instance() {
	static osg::ref_ptr<MissingCodePointsReporter> s_ptr = new MissingCodePointsReporter();
	return s_ptr.get();
}



    Font::Font() {
	    _images.clear();
		_textures.clear();
		_undefinedChar = new FontCharData();
		_reportMissingCodePoints = cefix::Settings::instance()->getReportMissingCodePointsFlag();
				
    }
	
	Font::Font(const Font& font,const osg::CopyOp& copyop)
	:	osg::Object(font, copyop),
		_fontData(font._fontData),
		_images(),
		_textures(),
		_chars(font._chars),
		_undefinedChar(font._undefinedChar.get()),
		_subPixelAlignment(font._subPixelAlignment),
		_reportMissingCodePoints(font._reportMissingCodePoints)
	{
		// bilder kopieren
		for(ImageVector::const_iterator i = font._images.begin(); i != font._images.end(); ++i) {
			_images.push_back( dynamic_cast<osg::Image*>((*i)->clone(copyop)) );
		}
		
		_buildTexture(Settings::instance()->getFontsUseTextureRectangle());
	}
    
	// ---------------------------------------------------------------------------
	// load
	// lädt die Datei und startet das Parsen
	// ---------------------------------------------------------------------------
	bool Font::load(const std::string& fileName) {

        std::string fileFound = osgDB::findDataFile(fileName);

		if (fileFound.empty()) {
			
			// zweiter Versuch in fonts/
			fileFound = osgDB::findDataFile("fonts/"+fileName);

			if (fileFound.empty()) {
                log::error("FontManager::load") << "could not load font file '" << fileName << "'" << std::endl;
				return false;
			}
		}

		_fileName = fileFound;
		
		return _readFont();
	}

	
	
	

	
    #pragma mark -
    #pragma mark misc ::


    osg::StateSet* Font::getStandardStateSet(unsigned int texId) {

        osg::StateSet* state = new osg::StateSet();
		osg::Texture* tex = getTexture(texId);
		state->setTextureAttributeAndModes(0, tex,osg::StateAttribute::ON);
		        
		state->setTextureAttribute(0,new osg::TexEnv(osg::TexEnv::BLEND));
		state->setMode(GL_BLEND,osg::StateAttribute::ON);
        
        state->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
        osg::Depth* depth = new osg::Depth();
        depth->setWriteMask(false);
        state->setAttribute(depth);
        
        osg::BlendFunc* b = new osg::BlendFunc();
        b->setFunction(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
        state->setAttribute(b);
        return state;
    }
	
    #pragma mark -
    #pragma mark XML-PARSING ::

	
	// ---------------------------------------------------------------------------
	// _readFont
	// wirft das XML-System an und parst das XML-File
	// ---------------------------------------------------------------------------
	bool Font::_readFont() {
	
		TiXmlDocument doc(_fileName );
		doc.LoadFile();
		
		if (doc.Error()) {
			log::error("Font::_readFont ") << "Error parsing font: "  << _fileName << " " << doc.ErrorDesc() << " at col: " << doc.ErrorCol() << " row: " << doc.ErrorRow() << std::endl;
			return false;
		}
		TiXmlElement* root = doc.RootElement();
		if ("FontTexture" != root->ValueStr()) {
			log::error("Font::_readFont ") << "Error parsing font: "  << _fileName << " " << "unknown root-tag, should  be FontTexture" << std::endl;
			return false;
		}
		
		_fontData = new FontGlobalData(root);
		
		_images.clear();
		std::ostringstream ss;
		std::string orgTextureFilePath = XML::getStringValueFromSubNode(root, "TextureFile", "");;
		
		for(unsigned int i = 0; i < _fontData->getTextureCount(); ++i) {
			osg::ref_ptr<osg::Image> image = NULL;
		
			string textureFileName = orgTextureFilePath;
			
			if (i > 0) {
				ss.clear();
				ss.str("");
				ss << osgDB::getNameLessExtension(orgTextureFilePath) << "_" << i << "." << osgDB::getFileExtension(orgTextureFilePath);
				
				textureFileName = ss.str();
			}
		
			if (!textureFileName.empty()) {
				textureFileName = osgDB::getFilePath(_fileName) + "/" + textureFileName;
				image = osgDB::readImageFile(textureFileName);
			}
			
			if (!image.valid()) {
				osg::notify(osg::ALWAYS) << "Font :: Could not load texturefile '" 
				<< textureFileName << "' for font '" << _fileName << "'" << std::endl;
				
				return false;
			}
			{
				osg::ref_ptr<osg::Image> temp_image = image.get();
				ImageUtilities utils(temp_image.get());
				image = utils.extractChannel(ImageUtilities::ALPHA, GL_ALPHA, GL_ALPHA);
                image->setFileName(textureFileName);
			}
			_images.push_back(image.get());
		} // for
			
		_buildTexture(Settings::instance()->getFontsUseTextureRectangle());
		
		bool b = _extractCharData(root->FirstChild("Chars"));
		if (b) {
			osg::notify(osg::INFO) << "Font " << _fontData->fontName() << " sucessfully loaded from file " << _fileName << std::endl;
			cefix::AllocationObserver::instance()->observe("Font", this);
			
			if (_reportMissingCodePoints) {			
				_undefinedChar->setDummyValues(_fontData.get());
			
				if (_chars.find(0x258C)  != _chars.end())
					_undefinedChar = _chars[0x258C].get();
			}

		}
		return b;
	}

	
	
	void Font::_buildTexture(bool useRectangleExtensionFlag) {
        
        _fontData->setAbsoluteTexCoordsFlag(useRectangleExtensionFlag);
		
		_textures.clear();
		
		for(ImageVector::iterator i = _images.begin(); i != _images.end(); ++i) {
			osg::Texture* texture = NULL;
			if (_fontData->useAbsoluteTexCoords()) {
				osg::TextureRectangle* t = new osg::TextureRectangle();
				t->setImage((*i).get());
				texture = t;
			} else {
				osg::Texture2D* t = new osg::Texture2D();
				t->setImage((*i).get());
				texture = t;
			}
			
			texture->setFilter(osg::Texture::MIN_FILTER, cefix::Settings::instance()->useMipMappedFonts() ? osg::Texture::LINEAR_MIPMAP_LINEAR : osg::Texture::LINEAR);
			texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
			
			
			cefix::AllocationObserver::instance()->observe("FontTexture", texture);
			cefix::AllocationObserver::instance()->observe("FontImage", (*i).get());
			
			
			_textures.push_back(texture);
		}
    }
	
	
	
	
    // ---------------------------------------------------------------------------
    // _extractCharData
    // interpretiert den chars-subbaum des xml-files und baut char-table auf...
    // ---------------------------------------------------------------------------
    
    bool Font::_extractCharData(TiXmlNode* charRootNode) {
		
		if (NULL == charRootNode) {
			osg::notify(osg::ALWAYS) << "malformed font-file '" << _fileName << "', missing 'Chars'-tags" << std::endl;
			return false;
		}
		
		_chars.clear();
		TiXmlNode* child = charRootNode->FirstChild("char");
    
		if (NULL == child) {
            osg::notify(osg::ALWAYS) << "malformed font-file '" << _fileName << "', missing 'Char'-tags" << std::endl;
            return false;
        }
		int m=0;
		while (child) {
			m++;
			child = charRootNode->IterateChildren("char", child);
		}
		
		child = charRootNode->FirstChild("char");
		
		while (child) {
			FontCharData* fontCharData = new FontCharData(child->ToElement(), _fontData.get());
			_chars[fontCharData->ID()] = fontCharData;
			child = charRootNode->IterateChildren("char", child);
		}  

        return true;
    }
	


double Font::computeMinuscleHeight()
{
	float height =  getCharData('x')->_texHeight - (_fontData->getBleed() * 2);
	_fontData->setMinuscleHeight(height);

    cefix::log::info("Font") << "minuscle height: " << height << std::endl;
    
	return height;
}


double Font::computeMajuscleHeight()
{
	float height =  getCharData('X')->_texHeight - (_fontData->getBleed() * 2);
	_fontData->setMajuscleHeight(height);
    cefix::log::info("Font") << "majuscle height: " << height << std::endl;
    return height;
}

void Font::reportMissingCodePoints(unsigned int codepoint) 
{	
	MissingCodePointsReporter::instance()->report(_fileName, codepoint);
}

}


