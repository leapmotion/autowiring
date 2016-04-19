/*
(c) 2014 Glen Joseph Fernandes
<glenjofe -at- gmail.com>

Distributed under the Boost Software
License, Version 1.0.
http://autoboost.org/LICENSE_1_0.txt
*/
#ifndef AUTOBOOST_ALIGN_DETAIL_ADDRESSOF_HPP
#define AUTOBOOST_ALIGN_DETAIL_ADDRESSOF_HPP

#include <autoboost/config.hpp>

#if !defined(AUTOBOOST_NO_CXX11_ADDRESSOF)
#include <memory>
#else
#include <autoboost/core/addressof.hpp>
#endif

namespace autoboost {
namespace alignment {
namespace detail {

#if !defined(AUTOBOOST_NO_CXX11_ADDRESSOF)
using std::addressof;
#else
using autoboost::addressof;
#endif

} /* .detail */
} /* .alignment */
} /* .boost */

#endif
