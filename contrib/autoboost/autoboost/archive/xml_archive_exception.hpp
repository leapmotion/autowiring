#ifndef AUTOBOOST_ARCHIVE_XML_ARCHIVE_EXCEPTION_HPP
#define AUTOBOOST_ARCHIVE_XML_ARCHIVE_EXCEPTION_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// xml_archive_exception.hpp:

// (C) Copyright 2007 Robert Ramey - http://www.rrsd.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <exception>
#include <autoboost/assert.hpp>

#include <autoboost/config.hpp>
#include <autoboost/archive/detail/decl.hpp>
#include <autoboost/archive/archive_exception.hpp>

#include <autoboost/archive/detail/abi_prefix.hpp> // must be the last header

namespace autoboost {
namespace archive {

//////////////////////////////////////////////////////////////////////
// exceptions thrown by xml archives
//
class AUTOBOOST_SYMBOL_VISIBLE xml_archive_exception :
    public virtual autoboost::archive::archive_exception
{
public:
    typedef enum {
        xml_archive_parsing_error,    // see save_register
        xml_archive_tag_mismatch,
        xml_archive_tag_name_error
    } exception_code;
    AUTOBOOST_ARCHIVE_DECL xml_archive_exception(
        exception_code c,
        const char * e1 = NULL,
        const char * e2 = NULL
    );
    AUTOBOOST_ARCHIVE_DECL xml_archive_exception(xml_archive_exception const &) ;
    virtual AUTOBOOST_ARCHIVE_DECL ~xml_archive_exception() AUTOBOOST_NOEXCEPT_OR_NOTHROW ;
};

}// namespace archive
}// namespace autoboost

#include <autoboost/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

#endif //AUTOBOOST_XML_ARCHIVE_ARCHIVE_EXCEPTION_HPP
