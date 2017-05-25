//  minus.hpp
//
//  (C) Copyright 2011 Vicente J. Botet Escriba
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//

#ifndef AUTOBOOST_RATIO_MPL_MINUS_HPP
#define AUTOBOOST_RATIO_MPL_MINUS_HPP

#include <autoboost/ratio/ratio.hpp>
#include <autoboost/ratio/mpl/numeric_cast.hpp>
#include <autoboost/mpl/minus.hpp>

namespace autoboost {
namespace mpl {

template<>
struct minus_impl< rational_c_tag,rational_c_tag >
{
    template< typename R1, typename R2 > struct apply
        : ratio_subtract<R1, R2>
    {
    };
};
}
}

#endif  // AUTOBOOST_RATIO_MPL_MINUS_HPP
