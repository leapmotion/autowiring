//
// Copyright (c) Chris Glover, 2016.
//
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_TYPE_INDEX_RUNTIME_CAST_HPP
#define AUTOBOOST_TYPE_INDEX_RUNTIME_CAST_HPP

/// \file runtime_cast.hpp
/// \brief Contains the basic utilities necessary to fully emulate
/// dynamic_cast for language level constructs (raw pointers and references).
///
/// autoboost::typeindex::runtime_cast is a drop in replacement for dynamic_cast
/// that can be used in situations where traditional rtti is either unavailable
/// or undesirable.

#include <autoboost/type_index/runtime_cast/register_runtime_class.hpp>
#include <autoboost/type_index/runtime_cast/pointer_cast.hpp>
#include <autoboost/type_index/runtime_cast/reference_cast.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

#endif // AUTOBOOST_TYPE_INDEX_RUNTIME_CAST_HPP
