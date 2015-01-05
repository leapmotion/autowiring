/*
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * Copyright (c) 2014 Andrey Semashev
 */
/*!
 * \file   atomic/detail/operations_lockfree.hpp
 *
 * This header defines lockfree atomic operations.
 */

#ifndef AUTOBOOST_ATOMIC_DETAIL_OPERATIONS_LOCKFREE_HPP_INCLUDED_
#define AUTOBOOST_ATOMIC_DETAIL_OPERATIONS_LOCKFREE_HPP_INCLUDED_

#include <autoboost/atomic/detail/config.hpp>
#include <autoboost/atomic/detail/platform.hpp>

#if !defined(AUTOBOOST_ATOMIC_EMULATED)
#include AUTOBOOST_ATOMIC_DETAIL_HEADER(autoboost/atomic/detail/ops_)
#else
#include <autoboost/atomic/detail/operations_fwd.hpp>
#endif

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#endif // AUTOBOOST_ATOMIC_DETAIL_OPERATIONS_LOCKFREE_HPP_INCLUDED_
