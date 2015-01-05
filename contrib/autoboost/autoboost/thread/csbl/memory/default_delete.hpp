// Copyright (C) 2013 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// 2013/10 Vicente J. Botet Escriba
//   Creation.

#ifndef AUTOBOOST_CSBL_MEMORY_DEFAULT_DELETE_HPP
#define AUTOBOOST_CSBL_MEMORY_DEFAULT_DELETE_HPP

#include <autoboost/thread/csbl/memory/config.hpp>

// 20.8.1 class template unique_ptr:
// default_delete

#if defined AUTOBOOST_NO_CXX11_SMART_PTR
#include <autoboost/move/unique_ptr.hpp>

namespace autoboost
{
  namespace csbl
  {
    using ::autoboost::movelib::default_delete;
  }
}
#else
namespace autoboost
{
  namespace csbl
  {
    using ::std::default_delete;
  }
}
#endif // defined  AUTOBOOST_NO_CXX11_SMART_PTR

namespace autoboost
{
  using ::autoboost::csbl::default_delete;
}
#endif // header
