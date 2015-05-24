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

#ifndef CEFIX_UTF8_TEXT_RENDERER_HEADER
#define CEFIX_UTF8_TEXT_RENDERER_HEADER

#include <cefix/StringUtils.h>
#include <cefix/TextRenderer.h>

namespace cefix {

template <class IteratorClass> class Utf8TextRendererT : public cefix::TextRendererT<IteratorClass> {
	
	public:
		typedef IteratorClass iterator;
		typedef typename iterator::TextAlignment TextAlignment;


		template<class T> void render(const T &text, cefix::TextRendererBase::Container* container = NULL, iterator* itr = NULL) {

			if ((itr) && (!this->_iterator.valid()))
				this->setIterator(itr);
			
			if (!this->_iterator.valid()) {
				log::error("PUTF8TextRendererT::render") << "no iterator specified! using standard "<< std::endl;                    
				this->setIterator(new iterator());
			}
			
			if ((container) && (!this->_iterator->hasContainer()))
				this->_iterator->setContainer(container);
			
			unsigned int m = text.length();
			unsigned int ch,nextch;
			
			this->_iterator->begin();
			// check, if this iterator needs to preflight the rendered data, e.g. for measuring the widths
			unsigned int j = 0;
            if (this->_iterator->needsPreflight()) {
				for (unsigned int i=0; i < m; ) {
                    this->_iterator->setTextPosition(j);
					ch = Unicode::getCodePointFromUTF8(text, i);
					unsigned int k(i);
					nextch = (i<m) ? Unicode::getCodePointFromUTF8(text, k) : 0;
					this->_iterator->preflight(ch, nextch);
				}
			}
			//finally, start the rendering
			j = 0;
			for (unsigned int i=0; i < m; ++j) {
				this->_iterator->setTextPosition(j);
				ch = Unicode::getCodePointFromUTF8(text, i);
                this->_iterator->prerender(ch);
				this->_iterator->render(ch);
				unsigned int k(i);
				nextch = (i<m) ? Unicode::getCodePointFromUTF8(text, k) : 0;
				this->_iterator->advance(ch,nextch);
			   
			}
			
			this->_iterator->finish();
		}

};

}


#endif