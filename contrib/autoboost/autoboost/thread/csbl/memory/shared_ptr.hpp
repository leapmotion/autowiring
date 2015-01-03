// Copyright (C) 2014 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// 2014/10 Vicente J. Botet Escriba
//   Creation.

#ifndef AUTOBOOST_CSBL_MEMORY_SHARED_PTR_HPP
#define AUTOBOOST_CSBL_MEMORY_SHARED_PTR_HPP

#include <autoboost/thread/csbl/memory/config.hpp>

#if defined AUTOBOOST_NO_CXX11_SMART_PTR

#include <autoboost/smart_ptr/shared_ptr.hpp>
#include <autoboost/smart_ptr/make_shared.hpp>

namespace autoboost
{
  namespace csbl
  {
    using ::autoboost::shared_ptr;
    using ::autoboost::make_shared;
  }
}

#else

#include <autoboost/shared_ptr.hpp>

namespace autoboost
{
  namespace csbl
  {
    using std::shared_ptr;
    using std::make_shared;
  }
}

#endif
#endif // header
