//  (C) Copyright 2009-2011 Frederic Bron.
//
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_HAS_COMPLEMENT_HPP_INCLUDED
#define AUTOBOOST_TT_HAS_COMPLEMENT_HPP_INCLUDED

#define AUTOBOOST_TT_TRAIT_NAME has_complement
#define AUTOBOOST_TT_TRAIT_OP ~
#define AUTOBOOST_TT_FORBIDDEN_IF\
   ::autoboost::type_traits::ice_or<\
      /* pointer */\
      ::autoboost::is_pointer< Rhs_noref >::value,\
      /* fundamental non integral */\
      ::autoboost::type_traits::ice_and<\
         ::autoboost::is_fundamental< Rhs_noref >::value,\
         ::autoboost::type_traits::ice_not< ::autoboost::is_integral< Rhs_noref >::value >::value\
      >::value\
   >::value


#include <autoboost/type_traits/detail/has_prefix_operator.hpp>

#undef AUTOBOOST_TT_TRAIT_NAME
#undef AUTOBOOST_TT_TRAIT_OP
#undef AUTOBOOST_TT_FORBIDDEN_IF

#endif
