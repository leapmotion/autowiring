// Copyright (C) 2013 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// 2013/10 Vicente J. Botet Escriba
//   Creation.

#ifndef AUTOBOOST_CSBL_DEQUE_HPP
#define AUTOBOOST_CSBL_DEQUE_HPP

#include <autoboost/config.hpp>
// MSVC has some trouble instantiating a non_copyable type
//C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\INCLUDE\xmemory0(606) : error C2248: 'non_copyable::non_copyable' : cannot access private member declared in class 'non_copyable'
//        ..\libs\thread\test\sync\mutual_exclusion\queue_views\single_thread_pass.cpp(24) : see declaration of 'non_copyable::non_copyable'
//        ..\libs\thread\test\sync\mutual_exclusion\queue_views\single_thread_pass.cpp(23) : see declaration of 'non_copyable'
//        C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\INCLUDE\xmemory0(605) : while compiling class template member function 'void std::allocator<_Ty>::construct(_Ty *,const _Ty &)'
//        with
//        [
//            _Ty=non_copyable
//        ]
#if defined AUTOBOOST_THREAD_USES_AUTOBOOST_DEQUE || defined AUTOBOOST_NO_CXX11_RVALUE_REFERENCES || (defined _MSC_VER && _MSC_FULL_VER < 180020827)
#ifndef AUTOBOOST_THREAD_USES_AUTOBOOST_DEQUE
#define AUTOBOOST_THREAD_USES_AUTOBOOST_DEQUE
#endif
#include <autoboost/container/deque.hpp>
#else
#include <deque>
#endif

namespace autoboost
{
  namespace csbl
  {
#if defined AUTOBOOST_THREAD_USES_AUTOBOOST_DEQUE
    using ::autoboost::container::deque;

#else
    using ::std::deque;

#endif

  }
}
#endif // header
