//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2008-2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_INTERPROCESS_DETAIL_VARIADIC_TEMPLATES_TOOLS_HPP
#define AUTOBOOST_INTERPROCESS_DETAIL_VARIADIC_TEMPLATES_TOOLS_HPP

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif
#
#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <autoboost/container/detail/variadic_templates_tools.hpp>

namespace autoboost {
namespace interprocess {
namespace ipcdetail {

using autoboost::container::container_detail::tuple;
using autoboost::container::container_detail::build_number_seq;
using autoboost::container::container_detail::index_tuple;
using autoboost::container::container_detail::get;

}}}   //namespace autoboost { namespace interprocess { namespace ipcdetail {

#endif   //#ifndef AUTOBOOST_INTERPROCESS_DETAIL_VARIADIC_TEMPLATES_TOOLS_HPP
