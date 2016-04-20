/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// text_iarchive.cpp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#if (defined _MSC_VER) && (_MSC_VER == 1200)
#  pragma warning (disable : 4786) // too long name, harmless warning
#endif

#define AUTOBOOST_ARCHIVE_SOURCE
#include <autoboost/archive/text_iarchive.hpp>
#include <autoboost/archive/detail/archive_serializer_map.hpp>

// explicitly instantiate for this type of text stream
#include <autoboost/archive/impl/archive_serializer_map.ipp>
#include <autoboost/archive/impl/basic_text_iarchive.ipp>
#include <autoboost/archive/impl/text_iarchive_impl.ipp>

namespace autoboost {
namespace archive {

template class detail::archive_serializer_map<text_iarchive>;
template class basic_text_iarchive<text_iarchive> ;
template class text_iarchive_impl<text_iarchive> ;

} // namespace archive
} // namespace autoboost
