//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright David Abrahams, Vicente Botet 2009.
// (C) Copyright Ion Gaztanaga 2009-2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/move for documentation.
//
//////////////////////////////////////////////////////////////////////////////

//! \file
//! A general library header that includes
//! the rest of top-level headers.

#ifndef AUTOBOOST_MOVE_MOVE_HPP
#define AUTOBOOST_MOVE_MOVE_HPP

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif
#
#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <autoboost/move/detail/config_begin.hpp>
#include <autoboost/move/utility.hpp>
#include <autoboost/move/iterator.hpp>
#include <autoboost/move/traits.hpp>
#include <autoboost/move/algorithm.hpp>
#include <autoboost/move/detail/config_end.hpp>

#endif //#ifndef AUTOBOOST_MOVE_MOVE_HPP
