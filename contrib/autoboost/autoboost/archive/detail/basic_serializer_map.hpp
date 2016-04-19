#ifndef  AUTOBOOST_SERIALIZER_MAP_HPP
#define AUTOBOOST_SERIALIZER_MAP_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// basic_serializer_map.hpp: extenstion of type_info required for serialization.

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <set>

#include <autoboost/config.hpp>
#include <autoboost/noncopyable.hpp>
#include <autoboost/archive/detail/auto_link_archive.hpp>

#include <autoboost/archive/detail/abi_prefix.hpp> // must be the last header

namespace autoboost {
namespace serialization {
    class extended_type_info;
}

namespace archive {
namespace detail {

class basic_serializer;

class AUTOBOOST_SYMBOL_VISIBLE
basic_serializer_map : public
    autoboost::noncopyable
{
    struct type_info_pointer_compare
    {
        bool operator()(
            const basic_serializer * lhs, const basic_serializer * rhs
        ) const ;
    };
    typedef std::set<
        const basic_serializer *,
        type_info_pointer_compare
    > map_type;
    map_type m_map;
public:
    AUTOBOOST_ARCHIVE_DECL bool insert(const basic_serializer * bs);
    AUTOBOOST_ARCHIVE_DECL void erase(const basic_serializer * bs);
    AUTOBOOST_ARCHIVE_DECL const basic_serializer * find(
        const autoboost::serialization::extended_type_info & type_
    ) const;
private:
    // cw 8.3 requires this
    basic_serializer_map& operator=(basic_serializer_map const&);
};

} // namespace detail
} // namespace archive
} // namespace autoboost

#include <autoboost/archive/detail/abi_suffix.hpp> // must be the last header

#endif // AUTOBOOST_SERIALIZER_MAP_HPP
