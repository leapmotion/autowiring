//  abs.hpp
//
//  (C) Copyright 2011 Vicente J. Botet Escriba
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//

#ifndef AUTOBOOST_RATIO_MPL_RATIONAL_C_TAG_HPP
#define AUTOBOOST_RATIO_MPL_RATIONAL_C_TAG_HPP

#ifdef AUTOBOOST_RATIO_EXTENSIONS

#include <autoboost/mpl/int.hpp>

namespace autoboost {
namespace mpl {

struct rational_c_tag : int_<10> {};

}
}

#endif // AUTOBOOST_RATIO_EXTENSIONS
#endif  // AUTOBOOST_RATIO_MPL_RATIONAL_C_TAG_HPP
