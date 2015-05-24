/*
 *  Alterable.cpp
 *  cefix_alterable
 *
 *  Created by Stephan Huber on 02.09.11.
 *  Copyright 2011 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "Alterable.h"
#include <cefix/StringUtils.h>

namespace cefix {

std::string Alterable::createKey(const std::string& name)
{
    std::string s = strToLower(name);
    return strReplaceAll<std::string>(s, " ", "_");
}

};