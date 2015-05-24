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

#ifndef CEFIX_VERSION_HEADER
#define CEFIX_VERSION_HEADER

#include <cefix/Export.h>
#include <string>

extern "C" {

#define CEFIX_MAJOR_VERSION    0
#define CEFIX_MINOR_VERSION    8
#define CEFIX_PATCH_VERSION    7

/* Convenience macro that can be used to decide whether a feature is present or not i.e.
 * #if CEFIX_MIN_VERSION_REQUIRED(0,8,0)
 *    your code here
 * #endif
 */
#define CEFIX_MIN_VERSION_REQUIRED(MAJOR, MINOR, PATCH) ((CEFIX_MAJOR_VERSION>MAJOR) || (CEFIX_MAJOR_VERSION==MAJOR && (CEFIX_MINOR_VERSION>MINOR || (CEFIX_MINOR_VERSION==MINOR && CEFIX_PATCH_VERSION>=PATCH))))
#define CEFIX_VERSION_LESS_THAN(MAJOR, MINOR, PATCH) ((CEFIX_MAJOR_VERSION<MAJOR) || (CEFIX_MAJOR_VERSION==MAJOR && (CEFIX_MINOR_VERSION<MINOR || (CEFIX_MINOR_VERSION==MINOR && CEFIX_PATCH_VERSION<PATCH))))
#define CEFIX_VERSION_LESS_OR_EQUAL(MAJOR, MINOR, PATCH) ((CEFIX_MAJOR_VERSION<MAJOR) || (CEFIX_MAJOR_VERSION==MAJOR && (CEFIX_MINOR_VERSION<MINOR || (CEFIX_MINOR_VERSION==MINOR && CEFIX_PATCH_VERSION<=PATCH))))
#define CEFIX_VERSION_GREATER_THAN(MAJOR, MINOR, PATCH) ((CEFIX_MAJOR_VERSION>MAJOR) || (CEFIX_MAJOR_VERSION==MAJOR && (CEFIX_MINOR_VERSION>MINOR || (CEFIX_MINOR_VERSION==MINOR && CEFIX_PATCH_VERSION>PATCH))))
#define CEFIX_VERSION_GREATER_OR_EQUAL(MAJOR, MINOR, PATCH) ((CEFIX_MAJOR_VERSION>MAJOR) || (CEFIX_MAJOR_VERSION==MAJOR && (CEFIX_MINOR_VERSION>MINOR || (CEFIX_MINOR_VERSION==MINOR && CEFIX_PATCH_VERSION>=PATCH))))


/**
  * cefixGetVersion() returns the library version number.
  * Numbering convention : cefix-1.0 will return 1.0 from cefixGetVersion.
  */
extern CEFIX_EXPORT const char* cefixGetVersion();
}

extern CEFIX_EXPORT std::string cefixGetVersionDetailed();



#endif