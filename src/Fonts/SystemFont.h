#ifndef SYSTEM_FONT_HEADER
#define SYSTEM_FONT_HEADER

#include <cefix/Font.h>

namespace cefix 
{
	class SystemFont {
	public:
		static Font* create();
	private:
		SystemFont();
	};
}
#endif