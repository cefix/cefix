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

#ifndef TEXTGEODE_CREATOR_HEADER_
#define TEXTGEODE_CREATOR_HEADER_

#include <cefix/DataFactory.h>
#include <cefix/ConvertScreenCoord.h>
#include <cefix/TabbedTextGeode.h>
#include <cefix/StringUtils.h>


namespace cefix {
 
 
/** 
 * small helper template class, which will create a custom textgeode class from a propertylist, holding all properties
 */
template<class TextGeodeClass>
class TextGeodeCreator {
	public:
		enum Options {	DEFAULT = 0,
						LEAVE_POSITION_UNTOUCHED = 1,
					 };
        struct AttributeMapping {
            unsigned int code_point;
            std::string html_tag;
            AttributeMapping(unsigned int in_code_point, const std::string& in_html_tag) : code_point(in_code_point), html_tag(in_html_tag) {}
            AttributeMapping() : code_point(0), html_tag("") {}
        };
    
        typedef  std::map<std::string, AttributeMapping> Mapping;
		/** ctor */
		TextGeodeCreator()
        {
            addMapping("bold", AttributeMapping(0x1A00, "b"));
            addMapping("italic", AttributeMapping(0x1A10, "i"));
            addMapping("bolditalic", AttributeMapping(0x1A20,"strong"));
            addMapping("custom", AttributeMapping(0x1A30, "custom"));
        }
		
		
		/** creates the TextGeodeClass */
		TextGeodeClass* operator()(std::string key, Options options = DEFAULT) {
			TextGeodeClass* textgeode;
			DataFactory* df = DataFactory::instance();
						
			osg::Vec4 color		= df->getPreferencesFor(key+"/color", osg::Vec4(0.5f, 0.5f, 0.5f,1));
			std::string font	= df->getPreferencesFor<std::string>(key+"/font", "system.xml");
			float fontsize		= df->getPreferencesFor(key+"/fontsize", 0.0f);
			float linespacing	= df->getPreferencesFor(key+"/linespacing", fontsize);
            std::string alignment = cefix::strToLower(df->getPreferencesFor(key+"/alignment", "left"));
            
			textgeode = new TextGeodeClass(font,fontsize);
			textgeode->setLineSpacing(linespacing);
			textgeode->setTextColor(color);
            
            if (alignment=="center") 
                textgeode->setTextAlignment(TextGeodeClass::RenderIterator::ALIGN_CENTER);
            else if (alignment == "right")
                textgeode->setTextAlignment(TextGeodeClass::RenderIterator::ALIGN_RIGHT);
                
			
			if (df->getPreferencesFor(key+"/position")) {
				osg::Vec3 pos		= df->getPreferencesFor(key+"/position")->asVec3();
				if ((options == DEFAULT) || ((options & LEAVE_POSITION_UNTOUCHED) == 0))
					pos = convertScreenCoord(pos);
				
				textgeode->setPosition(pos);
			}
			
			{
                typedef cefix::TabbedTextGeode<typename TextGeodeClass::string_type> TTG;
                TTG* ttg = dynamic_cast<TTG*>(textgeode);
				if(ttg)
                    applyTabs(ttg, key);
			}
            {
                typedef cefix::AttributedTextGeode< typename TextGeodeClass::string_type, typename TextGeodeClass::TextRenderer> ATG;
                ATG* ttg = dynamic_cast<ATG*>(textgeode);
				if (ttg)
                    applyAttributionFonts(ttg, key);
			}
            
			return textgeode;
											
			
		}
    
        void addMapping(const std::string& str, const AttributeMapping& m) {
            _attributeMap[str] = m;
        }
	
	
		template <typename T>void applyTabs(cefix::TabbedTextGeode<T>* textgeode, std::string key) {
            DataFactory* df = DataFactory::instance();
            cefix::PropertyList* pl =  df->getPreferencesPropertyListFor(key+"/tabs");
			if (pl) {
				for(unsigned int i = 0; i < pl->size(); i++) {
					if (pl->getKeyAt(i) == "left")
						textgeode->setTabAt(pl->get(i)->asFloat(), cefix::TabbedTextRenderIterator::TAB_LEFT);
					else if  (pl->getKeyAt(i) == "center")
						textgeode->setTabAt(pl->get(i)->asFloat(), cefix::TabbedTextRenderIterator::TAB_CENTER);
					else if (pl->getKeyAt(i) == "right")
						textgeode->setTabAt(pl->get(i)->asFloat(),cefix::TabbedTextRenderIterator::TAB_RIGHT);
				}
			}
		}
		
		template <typename T>void applyAttributionFonts(cefix::AttributedTextGeode<T>* textgeode, const std::string& key) {
			
            DataFactory* df = DataFactory::instance();
			for(typename Mapping::iterator i = _attributeMap.begin(); i != _attributeMap.end(); ++i) {
			
			
				std::string attr_font_name = df->getPreferencesFor<std::string>(key+"/"+i->first+"/Font", "");
				
				if (attr_font_name.empty())
					attr_font_name = df->getPreferencesFor<std::string>(key+"/"+i->first+"Font", "");
				
				if (!attr_font_name.empty()) {
					textgeode->setFont(attr_font_name, i->second.code_point);

				}
                else {
                    continue;
                }
				
				{
					float attr_font_size = df->getPreferencesFor(key+"/"+i->first+"/FontSize", -1.0f);
					if (attr_font_size < 0)
						attr_font_size = df->getPreferencesFor(key+"/"+i->first+"FontSize", -1.0f);
					if (attr_font_size >= 0) {
						textgeode->setFontSize(attr_font_size, i->second.code_point);
					}
                    else
                        textgeode->setFontSize(textgeode->getFontSize(), i->second.code_point);
				}
				
				{
					osg::Vec4 attr_font_color = df->getPreferencesFor(key+"/"+i->first+"/Color", osg::Vec4(-1,1,1,1));
					if (attr_font_color[0] < 0)
						attr_font_color = df->getPreferencesFor(key+"/"+i->first+"Color", osg::Vec4(-1,1,1,1));
					if (attr_font_color[0] >= 0) {
						textgeode->setTextColor(attr_font_color, i->second.code_point);
					}
                    else
                        textgeode->setTextColor(textgeode->getTextColor(), i->second.code_point);
				}
				
				{
					float attr_line_spacing = df->getPreferencesFor(key+"/"+i->first+"/LineSpacing", -1.0f);
					if (attr_line_spacing < 0)
						attr_line_spacing = df->getPreferencesFor(key+"/"+i->first+"LineSpacing", -1.0f);
					if (attr_line_spacing >= 0) {
						textgeode->setLineSpacing(attr_line_spacing, i->second.code_point);
					}
                    else
                        textgeode->setLineSpacing(textgeode->getLineSpacing(), i->second.code_point);
				}

				
			}
		}
    
        std::string prepareTextFromUTF8Html(const string& text) const {
            std::string txt = cefix::strReplaceAll<std::string>(text, "&shy;", "\u00AD");
            std::wstring cp(L" ");
            for(typename Mapping::const_iterator i = _attributeMap.begin(); i != _attributeMap.end(); ++i) {
                cp[0] = i->second.code_point;
                std::string utf8_cp = cefix::Unicode::wstringToUTF8(cp);
                txt = cefix::strReplaceAll<std::string>(txt, "<"+i->second.html_tag+">", utf8_cp);
                txt = cefix::strReplaceAll<std::string>(txt, "</"+i->second.html_tag+">", utf8_cp);
            }
            
            return txt;
        }
    
        std::wstring prepareTextFromHtml(const wstring& text) const {
            std::wstring txt = cefix::strReplaceAll<std::wstring>(text, L"&shy;", L"\u00AD");
            std::wstring cp(L" ");
            for(typename Mapping::const_iterator i = _attributeMap.begin(); i != _attributeMap.end(); ++i) {
                cp[0] = i->second.code_point;
                std::wstring html_tag = cefix::Unicode::UTF8toWstring(i->second.html_tag);
                txt = cefix::strReplaceAll<std::wstring>(txt, L"<"+html_tag+L">", cp);
                txt = cefix::strReplaceAll<std::wstring>(txt, L"</"+html_tag+L">", cp);
            }
            
            return txt;
        }
    
private:
   Mapping _attributeMap;
		
};

}

#endif