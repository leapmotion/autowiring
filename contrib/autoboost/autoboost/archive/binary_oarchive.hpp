#ifndef AUTOBOOST_ARCHIVE_BINARY_OARCHIVE_HPP
#define AUTOBOOST_ARCHIVE_BINARY_OARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// binary_oarchive.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <ostream>
#include <autoboost/config.hpp>
#include <autoboost/archive/binary_oarchive_impl.hpp>
#include <autoboost/archive/detail/register_archive.hpp>

#ifdef AUTOBOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4511 4512)
#endif

namespace autoboost {
namespace archive {

// do not derive from this class.  If you want to extend this functionality
// via inhertance, derived from binary_oarchive_impl instead.  This will
// preserve correct static polymorphism.
class AUTOBOOST_SYMBOL_VISIBLE binary_oarchive :
    public binary_oarchive_impl<
        binary_oarchive, std::ostream::char_type, std::ostream::traits_type
    >
{
public:
    binary_oarchive(std::ostream & os, unsigned int flags = 0) :
        binary_oarchive_impl<
            binary_oarchive, std::ostream::char_type, std::ostream::traits_type
        >(os, flags)
    {}
    binary_oarchive(std::streambuf & bsb, unsigned int flags = 0) :
        binary_oarchive_impl<
            binary_oarchive, std::ostream::char_type, std::ostream::traits_type
        >(bsb, flags)
    {}
};

} // namespace archive
} // namespace autoboost

// required by export
AUTOBOOST_SERIALIZATION_REGISTER_ARCHIVE(autoboost::archive::binary_oarchive)
AUTOBOOST_SERIALIZATION_USE_ARRAY_OPTIMIZATION(autoboost::archive::binary_oarchive)

#ifdef AUTOBOOST_MSVC
#pragma warning(pop)
#endif

#endif // AUTOBOOST_ARCHIVE_BINARY_OARCHIVE_HPP
