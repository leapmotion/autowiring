#ifndef AUTOBOOST_ARCHIVE_POLYMORPHIC_XML_IARCHIVE_HPP
#define AUTOBOOST_ARCHIVE_POLYMORPHIC_XML_IARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// polymorphic_xml_iarchive.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <autoboost/config.hpp>
#include <autoboost/archive/xml_iarchive.hpp>
#include <autoboost/archive/detail/polymorphic_iarchive_route.hpp>

#ifdef AUTOBOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4511 4512)
#endif

namespace autoboost {
namespace archive {

class polymorphic_xml_iarchive :
    public detail::polymorphic_iarchive_route<xml_iarchive>
{
public:
    polymorphic_xml_iarchive(std::istream & is, unsigned int flags = 0) :
        detail::polymorphic_iarchive_route<xml_iarchive>(is, flags)
    {}
    ~polymorphic_xml_iarchive(){}
};

} // namespace archive
} // namespace autoboost

#ifdef AUTOBOOST_MSVC
#pragma warning(pop)
#endif

// required by export
AUTOBOOST_SERIALIZATION_REGISTER_ARCHIVE(
    autoboost::archive::polymorphic_xml_iarchive
)

#endif // AUTOBOOST_ARCHIVE_POLYMORPHIC_XML_IARCHIVE_HPP

