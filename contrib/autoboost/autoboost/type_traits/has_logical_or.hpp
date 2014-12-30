//  (C) Copyright 2009-2011 Frederic Bron.
//
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_HAS_LOGICAL_OR_HPP_INCLUDED
#define AUTOBOOST_TT_HAS_LOGICAL_OR_HPP_INCLUDED

#define AUTOBOOST_TT_TRAIT_NAME has_logical_or
#define AUTOBOOST_TT_TRAIT_OP ||
#define AUTOBOOST_TT_FORBIDDEN_IF\
   /* pointer with fundamental non convertible to bool */\
   ::autoboost::type_traits::ice_or<\
      ::autoboost::type_traits::ice_and<\
         ::autoboost::is_pointer< Lhs_noref >::value,\
         ::autoboost::type_traits::ice_and<\
            ::autoboost::is_fundamental< Rhs_nocv >::value,\
            ::autoboost::type_traits::ice_not< ::autoboost::is_convertible< Rhs_nocv, bool >::value >::value\
         >::value\
      >::value,\
      ::autoboost::type_traits::ice_and<\
         ::autoboost::is_pointer< Rhs_noref >::value,\
         ::autoboost::type_traits::ice_and<\
            ::autoboost::is_fundamental< Lhs_nocv >::value,\
            ::autoboost::type_traits::ice_not< ::autoboost::is_convertible< Lhs_nocv, bool >::value >::value\
         >::value\
      >::value\
   >::value


#include <autoboost/type_traits/detail/has_binary_operator.hpp>

#undef AUTOBOOST_TT_TRAIT_NAME
#undef AUTOBOOST_TT_TRAIT_OP
#undef AUTOBOOST_TT_FORBIDDEN_IF

#endif
