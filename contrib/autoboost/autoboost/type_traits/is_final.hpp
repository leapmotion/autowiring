
//  Copyright (c) 2014 Agustin Berge
//
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.


#ifndef AUTOBOOST_TT_IS_FINAL_HPP_INCLUDED
#define AUTOBOOST_TT_IS_FINAL_HPP_INCLUDED

#include <autoboost/type_traits/intrinsics.hpp>
#include <autoboost/type_traits/integral_constant.hpp>
#ifdef AUTOBOOST_IS_FINAL
#include <autoboost/type_traits/remove_cv.hpp>
#endif

namespace autoboost {

#ifdef AUTOBOOST_IS_FINAL
template <class T> struct is_final : public integral_constant<bool, AUTOBOOST_IS_FINAL(typename remove_cv<T>::type)> {};
#else
template <class T> struct is_final : public integral_constant<bool, false> {};
#endif

} // namespace autoboost

#endif // AUTOBOOST_TT_IS_FINAL_HPP_INCLUDED
