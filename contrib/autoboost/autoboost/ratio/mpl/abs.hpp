//  abs.hpp
//
//  (C) Copyright 2011 Vicente J. Botet Escriba
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//

#ifndef AUTOBOOST_RATIO_MPL_ABS_HPP
#define AUTOBOOST_RATIO_MPL_ABS_HPP

#include <autoboost/ratio/ratio.hpp>
#include <autoboost/ratio/mpl/numeric_cast.hpp>
#include <autoboost/ratio/detail/mpl/abs.hpp>

namespace autoboost {
namespace mpl {

template<>
struct abs_impl< rational_c_tag >
{
    template< typename R > struct apply
        : ratio_abs<R>
    {
    };
};
}
}

#endif  // AUTOBOOST_RATIO_MPL_ABS_HPP
