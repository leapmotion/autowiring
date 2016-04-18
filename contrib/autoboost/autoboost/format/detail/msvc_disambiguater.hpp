// ----------------------------------------------------------------------------
// msvc_disambiguater.hpp : msvc workarounds. (for put_{head|last} overloads)
//               the trick was described in autoboost's list  by Aleksey Gurtovoy
// ----------------------------------------------------------------------------

//  Copyright Samuel Krempp 2003. Use, modification, and distribution are
//  subject to the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// see http://www.boost.org/libs/format for library home page

// ----------------------------------------------------------------------------

#ifndef AUTOBOOST_MSVC_DISAMBIGUATER_HPP
#define AUTOBOOST_MSVC_DISAMBIGUATER_HPP

#if AUTOBOOST_WORKAROUND(__DECCXX_VER, AUTOBOOST_TESTED_AT(60590042))

#include <autoboost/format/group.hpp>
#include <ostream>

namespace autoboost {
namespace io {
namespace detail {

template< class Ch, class Tr, class T >
struct disambiguater
{
   template< typename U >
   static void put_head(AUTOBOOST_IO_STD basic_ostream<Ch, Tr>& os, group1<U> const& x, long)
   {
       os << group_head(x.a1_);
   }
   static void put_head(AUTOBOOST_IO_STD basic_ostream<Ch, Tr>& os, T const& x, int)
   {
   }
   template< typename U >
   static void put_last(AUTOBOOST_IO_STD basic_ostream<Ch, Tr>& os, group1<U> const& x, long)
   {
       os << group_last(x.a1_);
   }
   static void put_last(AUTOBOOST_IO_STD basic_ostream<Ch, Tr>& os, T const& x, int)
   {
     os << x;
   }
};

} // namespace detail
} // namespace io
} // namespace autoboost

#endif // -__DECCXX_VER

#endif // -AUTOBOOST_MSVC_DISAMBIGUATER_HPP
