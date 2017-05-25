//
// detail/type_traits.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_DETAIL_TYPE_TRAITS_HPP
#define AUTOBOOST_ASIO_DETAIL_TYPE_TRAITS_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>

#if defined(AUTOBOOST_ASIO_HAS_STD_TYPE_TRAITS)
# include <type_traits>
#else // defined(AUTOBOOST_ASIO_HAS_TYPE_TRAITS)
# include <autoboost/type_traits/add_const.hpp>
# include <autoboost/type_traits/is_const.hpp>
# include <autoboost/type_traits/is_convertible.hpp>
# include <autoboost/type_traits/is_function.hpp>
# include <autoboost/type_traits/is_same.hpp>
# include <autoboost/type_traits/remove_pointer.hpp>
# include <autoboost/type_traits/remove_reference.hpp>
# include <autoboost/utility/enable_if.hpp>
#endif // defined(AUTOBOOST_ASIO_HAS_TYPE_TRAITS)

namespace autoboost {
namespace asio {

#if defined(AUTOBOOST_ASIO_HAS_STD_TYPE_TRAITS)
using std::add_const;
using std::enable_if;
using std::is_const;
using std::is_convertible;
using std::is_function;
using std::is_same;
using std::remove_pointer;
using std::remove_reference;
#else // defined(AUTOBOOST_ASIO_HAS_STD_TYPE_TRAITS)
using autoboost::add_const;
template <bool Condition, typename Type = void>
struct enable_if : autoboost::enable_if_c<Condition, Type> {};
using autoboost::is_const;
using autoboost::is_convertible;
using autoboost::is_function;
using autoboost::is_same;
using autoboost::remove_pointer;
using autoboost::remove_reference;
#endif // defined(AUTOBOOST_ASIO_HAS_STD_TYPE_TRAITS)

} // namespace asio
} // namespace autoboost

#endif // AUTOBOOST_ASIO_DETAIL_TYPE_TRAITS_HPP
