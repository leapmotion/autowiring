/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// basic_oserializer.cpp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <cstddef> // NULL

#define AUTOBOOST_ARCHIVE_SOURCE
#include <autoboost/archive/detail/basic_oserializer.hpp>

namespace autoboost {
namespace archive {
namespace detail {

AUTOBOOST_ARCHIVE_DECL(AUTOBOOST_PP_EMPTY())
basic_oserializer::basic_oserializer(
        const autoboost::serialization::extended_type_info & eti
) :
    basic_serializer(eti),
    m_bpos(NULL)
{}

AUTOBOOST_ARCHIVE_DECL(AUTOBOOST_PP_EMPTY())
basic_oserializer::~basic_oserializer(){}

} // namespace detail
} // namespace archive
} // namespace autoboost
