//  (C) Copyright 2009-2011 Frederic Bron.
//
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_HAS_PRE_INCREMENT_HPP_INCLUDED
#define AUTOBOOST_TT_HAS_PRE_INCREMENT_HPP_INCLUDED

#include <autoboost/type_traits/is_array.hpp>

#define AUTOBOOST_TT_TRAIT_NAME has_pre_increment
#define AUTOBOOST_TT_TRAIT_OP ++
#define AUTOBOOST_TT_FORBIDDEN_IF\
   ::autoboost::type_traits::ice_or<\
      /* bool */\
      ::autoboost::is_same< bool, Rhs_nocv >::value,\
      /* void* */\
      ::autoboost::type_traits::ice_and<\
         ::autoboost::is_pointer< Rhs_noref >::value,\
         ::autoboost::is_void< Rhs_noptr >::value\
      >::value,\
      /* (fundamental or pointer) and const */\
      ::autoboost::type_traits::ice_and<\
         ::autoboost::type_traits::ice_or<\
            ::autoboost::is_fundamental< Rhs_nocv >::value,\
            ::autoboost::is_pointer< Rhs_noref >::value\
         >::value,\
         ::autoboost::is_const< Rhs_noref >::value\
      >::value,\
      /* Arrays */ \
      ::autoboost::is_array<Rhs_noref>::value\
   >::value


#include <autoboost/type_traits/detail/has_prefix_operator.hpp>

#undef AUTOBOOST_TT_TRAIT_NAME
#undef AUTOBOOST_TT_TRAIT_OP
#undef AUTOBOOST_TT_FORBIDDEN_IF

#endif
