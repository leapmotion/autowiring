// Copyright 2005 Alexander Nasonov.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef FILE_autoboost_type_traits_promote_hpp_INCLUDED
#define FILE_autoboost_type_traits_promote_hpp_INCLUDED

#include <autoboost/config.hpp>
#include <autoboost/type_traits/integral_promotion.hpp>
#include <autoboost/type_traits/floating_point_promotion.hpp>

// Should be the last #include
#include <autoboost/type_traits/detail/type_trait_def.hpp>

namespace autoboost {

namespace detail {

template<class T>
struct promote_impl
  : public integral_promotion<
        AUTOBOOST_DEDUCED_TYPENAME floating_point_promotion<T>::type
      >
{
};

}

AUTOBOOST_TT_AUX_TYPE_TRAIT_DEF1(
      promote
    , T
    , AUTOBOOST_DEDUCED_TYPENAME autoboost::detail::promote_impl<T>::type
    )
}

#include <autoboost/type_traits/detail/type_trait_undef.hpp>

#endif // #ifndef FILE_autoboost_type_traits_promote_hpp_INCLUDED

