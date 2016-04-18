//  (c) Copyright Fernando Luis Cacciola Carballal 2000-2004
//  Use, modification, and distribution is subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See library home page at http://www.boost.org/libs/numeric/conversion
//
// Contact the author at: fernando_cacciola@hotmail.com
//
#ifndef AUTOBOOST_NUMERIC_CONVERSION_BOUNDS_DETAIL_FLC_12NOV2002_HPP
#define AUTOBOOST_NUMERIC_CONVERSION_BOUNDS_DETAIL_FLC_12NOV2002_HPP

#include "autoboost/limits.hpp"
#include "autoboost/config.hpp"
#include "autoboost/mpl/if.hpp"

namespace autoboost { namespace numeric { namespace boundsdetail
{
  template<class N>
  class Integral
  {
      typedef std::numeric_limits<N> limits ;

    public :

      static N lowest  () { return limits::min AUTOBOOST_PREVENT_MACRO_SUBSTITUTION (); }
      static N highest () { return limits::max AUTOBOOST_PREVENT_MACRO_SUBSTITUTION (); }
      static N smallest() { return static_cast<N>(1); }
  } ;

  template<class N>
  class Float
  {
      typedef std::numeric_limits<N> limits ;

    public :

      static N lowest  () { return static_cast<N>(-limits::max AUTOBOOST_PREVENT_MACRO_SUBSTITUTION ()) ; }
      static N highest () { return limits::max AUTOBOOST_PREVENT_MACRO_SUBSTITUTION (); }
      static N smallest() { return limits::min AUTOBOOST_PREVENT_MACRO_SUBSTITUTION (); }
  } ;

  template<class N>
  struct get_impl
  {
    typedef mpl::bool_< ::std::numeric_limits<N>::is_integer > is_int ;

    typedef Integral<N> impl_int   ;
    typedef Float   <N> impl_float ;

    typedef typename mpl::if_<is_int,impl_int,impl_float>::type type ;
  } ;

} } } // namespace autoboost::numeric::boundsdetail.

#endif
//
///////////////////////////////////////////////////////////////////////////////////////////////
