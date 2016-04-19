//  negate.hpp
//
//  (C) Copyright 2011 Vicente J. Botet Escriba
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//

#ifndef AUTOBOOST_RATIO_MPL_NEGATE_HPP
#define AUTOBOOST_RATIO_MPL_NEGATE_HPP

#include <autoboost/ratio/ratio.hpp>
#include <autoboost/ratio/mpl/numeric_cast.hpp>
#include <autoboost/mpl/negate.hpp>

namespace autoboost {
namespace mpl {

template<>
struct negate_impl< rational_c_tag >
{
    template< typename R > struct apply
        : ratio_negate<R>
    {
    };
};
}
}

#endif  // AUTOBOOST_RATIO_MPL_NEGATE_HPP
