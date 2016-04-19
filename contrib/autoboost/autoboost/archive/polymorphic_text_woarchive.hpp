#ifndef AUTOBOOST_ARCHIVE_POLYMORPHIC_TEXT_WOARCHIVE_HPP
#define AUTOBOOST_ARCHIVE_POLYMORPHIC_TEXT_WOARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// polymorphic_text_oarchive.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <autoboost/config.hpp>
#ifdef AUTOBOOST_NO_STD_WSTREAMBUF
#error "wide char i/o not supported on this platform"
#else

#include <autoboost/archive/text_woarchive.hpp>
#include <autoboost/archive/detail/polymorphic_oarchive_route.hpp>

namespace autoboost {
namespace archive {

typedef detail::polymorphic_oarchive_route<
    text_woarchive_impl<text_woarchive>
> polymorphic_text_woarchive;

} // namespace archive
} // namespace autoboost

// required by export
AUTOBOOST_SERIALIZATION_REGISTER_ARCHIVE(
    autoboost::archive::polymorphic_text_woarchive
)

#endif // AUTOBOOST_NO_STD_WSTREAMBUF
#endif // AUTOBOOST_ARCHIVE_POLYMORPHIC_TEXT_WOARCHIVE_HPP

