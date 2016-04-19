/*
 *          Copyright Andrey Semashev 2007 - 2013.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */

/*!
 * \file   empty_deleter.hpp
 * \author Andrey Semashev
 * \date   22.04.2007
 *
 * This header contains an \c empty_deleter implementation. This is an empty
 * function object that receives a pointer and does nothing with it.
 * Such empty deletion strategy may be convenient, for example, when
 * constructing <tt>shared_ptr</tt>s that point to some object that should not be
 * deleted (i.e. a variable on the stack or some global singleton, like <tt>std::cout</tt>).
 */

#ifndef AUTOBOOST_UTILITY_EMPTY_DELETER_HPP
#define AUTOBOOST_UTILITY_EMPTY_DELETER_HPP

#include <autoboost/config.hpp>
#include <autoboost/core/null_deleter.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if defined(__GNUC__)
#pragma message "This header is deprecated, use autoboost/core/null_deleter.hpp instead."
#elif defined(_MSC_VER)
#pragma message("This header is deprecated, use autoboost/core/null_deleter.hpp instead.")
#endif

namespace autoboost {

//! A deprecated name for \c null_deleter
typedef null_deleter empty_deleter;

} // namespace autoboost

#endif // AUTOBOOST_UTILITY_EMPTY_DELETER_HPP
