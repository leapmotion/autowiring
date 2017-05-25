// Copyright 2005 Alexander Nasonov.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef FILE_autoboost_type_traits_promote_hpp_INCLUDED
#define FILE_autoboost_type_traits_promote_hpp_INCLUDED

#include <autoboost/config.hpp>
#include <autoboost/type_traits/integral_promotion.hpp>
#include <autoboost/type_traits/floating_point_promotion.hpp>

namespace autoboost {

template<class T> struct promote : public integral_promotion<typename floating_point_promotion<T>::type>{};

}

#endif // #ifndef FILE_autoboost_type_traits_promote_hpp_INCLUDED

