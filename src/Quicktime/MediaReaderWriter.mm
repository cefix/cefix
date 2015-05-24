//
//  MediaReaderWriter.cpp
//  cefix
//
//  Created by Stephan Huber on 02.03.13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include <cefix/Export.h>


#if CEFIX_AV_FOUNDATION_AVAILABLE
    #include "ReaderWriterAVFoundation.cpp"
    #include "OSXAVFoundationVideo.mm"
    #include "OSXAvFoundationCoreVideoTexture.cpp"
#endif

#if CEFIX_QT_KIT_AVAILABLE
    #include "ReaderWriterQTKit.cpp"
    #include "OSXQTKitVideo.mm"
    #include "OSXCoreVideoAdapter.mm"
    #include "OSXCoreVideoTexture.cpp"

#endif

#if !CEFIX_QUICKTIME_AVAILABLE
    #include "ReaderWriterImageIO.cpp"
#endif