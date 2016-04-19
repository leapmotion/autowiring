/*
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * Copyright (c) 2014 Andrey Semashev
 */
/*!
 * \file   atomic/detail/operations_fwd.hpp
 *
 * This header contains forward declaration of the \c operations template.
 */

#ifndef AUTOBOOST_ATOMIC_DETAIL_OPERATIONS_FWD_HPP_INCLUDED_
#define AUTOBOOST_ATOMIC_DETAIL_OPERATIONS_FWD_HPP_INCLUDED_

#include <cstddef>
#include <autoboost/atomic/detail/config.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace autoboost {
namespace atomics {
namespace detail {

template< std::size_t Size, bool Signed >
struct operations;

} // namespace detail
} // namespace atomics
} // namespace autoboost

#endif // AUTOBOOST_ATOMIC_DETAIL_OPERATIONS_FWD_HPP_INCLUDED_
