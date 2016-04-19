//  numeric_cast.hpp
//
//  (C) Copyright 2011 Vicente J. Botet Escriba
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//

#ifndef AUTOBOOST_RATIO_MPL_NUMERIC_CAST_HPP
#define AUTOBOOST_RATIO_MPL_NUMERIC_CAST_HPP

#include <autoboost/ratio/ratio.hpp>
#include <autoboost/mpl/integral_c.hpp>
#include <autoboost/ratio/mpl/rational_c_tag.hpp>
#include <autoboost/mpl/numeric_cast.hpp>

namespace autoboost {
namespace mpl {

template<> struct numeric_cast< integral_c_tag,rational_c_tag >
{
    template< typename N > struct apply
        : ratio< N::value, 1 >
    {
    };
};

}
}

#endif  // AUTOBOOST_RATIO_MPL_NUMERIC_CAST_HPP
