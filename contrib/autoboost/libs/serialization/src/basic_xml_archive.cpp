/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// basic_xml_archive.cpp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#define AUTOBOOST_ARCHIVE_SOURCE
#include <autoboost/serialization/config.hpp>
#include <autoboost/archive/basic_xml_archive.hpp>

namespace autoboost {
namespace archive {

AUTOBOOST_SYMBOL_VISIBLE const char *
AUTOBOOST_ARCHIVE_XML_OBJECT_ID(){
    return "object_id";
}
AUTOBOOST_SYMBOL_VISIBLE const char *
AUTOBOOST_ARCHIVE_XML_OBJECT_REFERENCE(){
    return "object_id_reference";
}
AUTOBOOST_SYMBOL_VISIBLE const char *
AUTOBOOST_ARCHIVE_XML_CLASS_ID(){
    return "class_id";
}
AUTOBOOST_SYMBOL_VISIBLE const char *
AUTOBOOST_ARCHIVE_XML_CLASS_ID_REFERENCE(){
    return "class_id_reference";
}
AUTOBOOST_SYMBOL_VISIBLE const char *
AUTOBOOST_ARCHIVE_XML_CLASS_NAME(){
    return "class_name";
}
AUTOBOOST_SYMBOL_VISIBLE const char *
AUTOBOOST_ARCHIVE_XML_TRACKING(){
    return "tracking_level";
}
AUTOBOOST_SYMBOL_VISIBLE const char *
AUTOBOOST_ARCHIVE_XML_VERSION(){
    return "version";
}
AUTOBOOST_SYMBOL_VISIBLE const char *
AUTOBOOST_ARCHIVE_XML_SIGNATURE(){
    return "signature";
}

}// namespace archive
}// namespace autoboost
