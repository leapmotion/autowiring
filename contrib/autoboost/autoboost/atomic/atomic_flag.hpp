/*
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * Copyright (c) 2011 Helge Bahmann
 * Copyright (c) 2013 Tim Blechmann
 * Copyright (c) 2014 Andrey Semashev
 */
/*!
 * \file   atomic/atomic_flag.hpp
 *
 * This header contains definition of \c atomic_flag.
 */

#ifndef AUTOBOOST_ATOMIC_ATOMIC_FLAG_HPP_INCLUDED_
#define AUTOBOOST_ATOMIC_ATOMIC_FLAG_HPP_INCLUDED_

#include <autoboost/atomic/capabilities.hpp>
#include <autoboost/atomic/detail/operations.hpp>
#include <autoboost/atomic/detail/atomic_flag.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace autoboost {

using atomics::atomic_flag;

} // namespace autoboost

#endif // AUTOBOOST_ATOMIC_ATOMIC_FLAG_HPP_INCLUDED_
