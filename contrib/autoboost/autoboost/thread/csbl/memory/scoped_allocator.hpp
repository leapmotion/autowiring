// Copyright (C) 2013 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// 2013/10 Vicente J. Botet Escriba
//   Creation.

#ifndef AUTOBOOST_CSBL_MEMORY_SCOPED_ALLOCATOR_HPP
#define AUTOBOOST_CSBL_MEMORY_SCOPED_ALLOCATOR_HPP

#include <autoboost/thread/csbl/memory/config.hpp>

// 20.7.7, uses_allocator
#if defined AUTOBOOST_NO_CXX11_ALLOCATOR
#include <autoboost/container/scoped_allocator.hpp>

namespace autoboost
{
  namespace csbl
  {
    using ::autoboost::container::uses_allocator;
  }
}
#else
namespace autoboost
{
  namespace csbl
  {
    using ::std::uses_allocator;
  }
}
#endif // AUTOBOOST_NO_CXX11_POINTER_TRAITS

#endif // header
