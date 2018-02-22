#ifndef AUTOBOOST_ARCHIVE_SERIALIZER_MAP_HPP
#define AUTOBOOST_ARCHIVE_SERIALIZER_MAP_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// archive_serializer_map.hpp: extenstion of type_info required for
// serialization.

// (C) Copyright 2009 Robert Ramey - http://www.rrsd.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

// note: this is nothing more than the thinest of wrappers around
// basic_serializer_map so we can have a one map / archive type.

#include <autoboost/config.hpp>
#include <autoboost/archive/detail/auto_link_archive.hpp>
#include <autoboost/archive/detail/abi_prefix.hpp> // must be the last header

namespace autoboost {

namespace serialization {
    class extended_type_info;
} // namespace serialization

namespace archive {
namespace detail {

class basic_serializer;

template<class Archive>
class AUTOBOOST_SYMBOL_VISIBLE archive_serializer_map {
public:
    static AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL bool insert(const basic_serializer * bs);
    static AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL void erase(const basic_serializer * bs);
    static AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL const basic_serializer * find(
        const autoboost::serialization::extended_type_info & type_
    );
};

} // namespace detail
} // namespace archive
} // namespace autoboost

#include <autoboost/archive/detail/abi_suffix.hpp> // must be the last header

#endif //AUTOBOOST_ARCHIVE_SERIALIZER_MAP_HPP
