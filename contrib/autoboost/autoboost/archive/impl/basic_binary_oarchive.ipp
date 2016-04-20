/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// basic_binary_oarchive.ipp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.
#include <string>
#include <autoboost/assert.hpp>
#include <algorithm>
#include <cstring>

#include <autoboost/config.hpp>
#if defined(AUTOBOOST_NO_STDC_NAMESPACE)
namespace std{
    using ::memcpy;
}
#endif

#include <autoboost/archive/basic_binary_oarchive.hpp>

namespace autoboost {
namespace archive {

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// implementation of binary_binary_oarchive

template<class Archive>
#if !defined(__BORLANDC__)
AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL(void)
#else
void
#endif
basic_binary_oarchive<Archive>::init(){
    // write signature in an archive version independent manner
    const std::string file_signature(AUTOBOOST_ARCHIVE_SIGNATURE());
    * this->This() << file_signature;
    // write library version
    const library_version_type v(AUTOBOOST_ARCHIVE_VERSION());
    * this->This() << v;
}

} // namespace archive
} // namespace autoboost
