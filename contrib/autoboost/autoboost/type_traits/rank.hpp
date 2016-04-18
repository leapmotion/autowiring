
//  (C) Copyright John Maddock 2005.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.


#ifndef AUTOBOOST_TT_RANK_HPP_INCLUDED
#define AUTOBOOST_TT_RANK_HPP_INCLUDED

// should be the last #include
#include <autoboost/type_traits/detail/size_t_trait_def.hpp>

namespace autoboost {

#if !defined( __CODEGEARC__ )

namespace detail{

template <class T, std::size_t N>
struct rank_imp
{
   AUTOBOOST_STATIC_CONSTANT(std::size_t, value = N);
};
#if !defined(AUTOBOOST_NO_ARRAY_TYPE_SPECIALIZATIONS)
template <class T, std::size_t R, std::size_t N>
struct rank_imp<T[R], N>
{
   AUTOBOOST_STATIC_CONSTANT(std::size_t, value = (::autoboost::detail::rank_imp<T, N+1>::value));
};

template <class T, std::size_t R, std::size_t N>
struct rank_imp<T const[R], N>
{
   AUTOBOOST_STATIC_CONSTANT(std::size_t, value = (::autoboost::detail::rank_imp<T, N+1>::value));
};

template <class T, std::size_t R, std::size_t N>
struct rank_imp<T volatile[R], N>
{
   AUTOBOOST_STATIC_CONSTANT(std::size_t, value = (::autoboost::detail::rank_imp<T, N+1>::value));
};

template <class T, std::size_t R, std::size_t N>
struct rank_imp<T const volatile[R], N>
{
   AUTOBOOST_STATIC_CONSTANT(std::size_t, value = (::autoboost::detail::rank_imp<T, N+1>::value));
};

#if !AUTOBOOST_WORKAROUND(__BORLANDC__, < 0x600) && !defined(__IBMCPP__) &&  !AUTOBOOST_WORKAROUND(__DMC__, AUTOBOOST_TESTED_AT(0x840))
template <class T, std::size_t N>
struct rank_imp<T[], N>
{
   AUTOBOOST_STATIC_CONSTANT(std::size_t, value = (::autoboost::detail::rank_imp<T, N+1>::value));
};
template <class T, std::size_t N>
struct rank_imp<T const[], N>
{
   AUTOBOOST_STATIC_CONSTANT(std::size_t, value = (::autoboost::detail::rank_imp<T, N+1>::value));
};
template <class T, std::size_t N>
struct rank_imp<T volatile[], N>
{
   AUTOBOOST_STATIC_CONSTANT(std::size_t, value = (::autoboost::detail::rank_imp<T, N+1>::value));
};
template <class T, std::size_t N>
struct rank_imp<T const volatile[], N>
{
   AUTOBOOST_STATIC_CONSTANT(std::size_t, value = (::autoboost::detail::rank_imp<T, N+1>::value));
};
#endif
#endif
}

#endif // !defined( __CODEGEARC__ )

#if defined( __CODEGEARC__ )
AUTOBOOST_TT_AUX_SIZE_T_TRAIT_DEF1(rank,T,__array_rank(T))
#else
AUTOBOOST_TT_AUX_SIZE_T_TRAIT_DEF1(rank,T,(::autoboost::detail::rank_imp<T,0>::value))
#endif

} // namespace autoboost

#include <autoboost/type_traits/detail/size_t_trait_undef.hpp>

#endif // AUTOBOOST_TT_IS_MEMBER_FUNCTION_POINTER_HPP_INCLUDED
