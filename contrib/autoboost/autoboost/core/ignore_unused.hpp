// Copyright (c) 2014 Adam Wulkiewicz, Lodz, Poland.
//
// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_CORE_IGNORE_UNUSED_HPP
#define AUTOBOOST_CORE_IGNORE_UNUSED_HPP

#include <autoboost/config.hpp>

namespace autoboost {

#ifndef AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES

template <typename... Ts>
AUTOBOOST_FORCEINLINE AUTOBOOST_CXX14_CONSTEXPR void ignore_unused(Ts const& ...)
{}

template <typename... Ts>
AUTOBOOST_FORCEINLINE AUTOBOOST_CXX14_CONSTEXPR void ignore_unused()
{}

#else

template <typename T1>
AUTOBOOST_FORCEINLINE AUTOBOOST_CXX14_CONSTEXPR void ignore_unused(T1 const&)
{}

template <typename T1, typename T2>
AUTOBOOST_FORCEINLINE AUTOBOOST_CXX14_CONSTEXPR void ignore_unused(T1 const&, T2 const&)
{}

template <typename T1, typename T2, typename T3>
AUTOBOOST_FORCEINLINE AUTOBOOST_CXX14_CONSTEXPR void ignore_unused(T1 const&, T2 const&, T3 const&)
{}

template <typename T1, typename T2, typename T3, typename T4>
AUTOBOOST_FORCEINLINE AUTOBOOST_CXX14_CONSTEXPR void ignore_unused(T1 const&, T2 const&, T3 const&, T4 const&)
{}

template <typename T1, typename T2, typename T3, typename T4, typename T5>
AUTOBOOST_FORCEINLINE AUTOBOOST_CXX14_CONSTEXPR void ignore_unused(T1 const&, T2 const&, T3 const&, T4 const&, T5 const&)
{}

template <typename T1>
AUTOBOOST_FORCEINLINE AUTOBOOST_CXX14_CONSTEXPR void ignore_unused()
{}

template <typename T1, typename T2>
AUTOBOOST_FORCEINLINE AUTOBOOST_CXX14_CONSTEXPR void ignore_unused()
{}

template <typename T1, typename T2, typename T3>
AUTOBOOST_FORCEINLINE AUTOBOOST_CXX14_CONSTEXPR void ignore_unused()
{}

template <typename T1, typename T2, typename T3, typename T4>
AUTOBOOST_FORCEINLINE AUTOBOOST_CXX14_CONSTEXPR void ignore_unused()
{}

template <typename T1, typename T2, typename T3, typename T4, typename T5>
AUTOBOOST_FORCEINLINE AUTOBOOST_CXX14_CONSTEXPR void ignore_unused()
{}

#endif

} // namespace autoboost

#endif // AUTOBOOST_CORE_IGNORE_UNUSED_HPP
