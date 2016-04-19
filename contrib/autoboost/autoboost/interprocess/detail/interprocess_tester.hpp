//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2007-2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_INTERPROCESS_DETAIL_INTERPROCESS_TESTER_HPP
#define AUTOBOOST_INTERPROCESS_DETAIL_INTERPROCESS_TESTER_HPP

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif
#
#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

namespace autoboost{
namespace interprocess{
namespace ipcdetail{

class interprocess_tester
{
   public:
   template<class T>
   static void dont_close_on_destruction(T &t)
   {  t.dont_close_on_destruction(); }
};

}  //namespace ipcdetail{
}  //namespace interprocess{
}  //namespace autoboost{

#endif   //#ifndef AUTOBOOST_INTERPROCESS_DETAIL_INTERPROCESS_TESTER_HPP

