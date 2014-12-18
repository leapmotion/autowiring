//  (C) Copyright 2009-2011 Frederic Bron.
//
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef BOOST_TT_HAS_PLUS_ASSIGN_HPP_INCLUDED
#define BOOST_TT_HAS_PLUS_ASSIGN_HPP_INCLUDED

#define BOOST_TT_TRAIT_NAME has_plus_assign
#define BOOST_TT_TRAIT_OP +=
#define BOOST_TT_FORBIDDEN_IF\
   ::autoboost::type_traits::ice_or<\
      /* Lhs==pointer and Rhs==pointer */\
      ::autoboost::type_traits::ice_and<\
         ::autoboost::is_pointer< Lhs_noref >::value,\
         ::autoboost::is_pointer< Rhs_noref >::value\
      >::value,\
      /* Lhs==void* and Rhs==fundamental */\
      ::autoboost::type_traits::ice_and<\
         ::autoboost::is_pointer< Lhs_noref >::value,\
         ::autoboost::is_void< Lhs_noptr >::value,\
         ::autoboost::is_fundamental< Rhs_nocv >::value\
      >::value,\
      /* Rhs==void* and Lhs==fundamental */\
      ::autoboost::type_traits::ice_and<\
         ::autoboost::is_pointer< Rhs_noref >::value,\
         ::autoboost::is_void< Rhs_noptr >::value,\
         ::autoboost::is_fundamental< Lhs_nocv >::value\
      >::value,\
      /* Lhs==pointer and Rhs==fundamental and Rhs!=integral */\
      ::autoboost::type_traits::ice_and<\
         ::autoboost::is_pointer< Lhs_noref >::value,\
         ::autoboost::is_fundamental< Rhs_nocv >::value,\
         ::autoboost::type_traits::ice_not< ::autoboost::is_integral< Rhs_noref >::value >::value\
      >::value,\
      /* Rhs==pointer and Lhs==fundamental and Lhs!=bool */\
      ::autoboost::type_traits::ice_and<\
         ::autoboost::is_pointer< Rhs_noref >::value,\
         ::autoboost::is_fundamental< Lhs_nocv >::value,\
         ::autoboost::type_traits::ice_not< ::autoboost::is_same< Lhs_nocv, bool >::value >::value\
      >::value,\
      /* (Lhs==fundamental or Lhs==pointer) and (Rhs==fundamental or Rhs==pointer) and (Lhs==const) */\
      ::autoboost::type_traits::ice_and<\
         ::autoboost::type_traits::ice_or<\
            ::autoboost::is_fundamental< Lhs_nocv >::value,\
            ::autoboost::is_pointer< Lhs_noref >::value\
         >::value,\
         ::autoboost::type_traits::ice_or<\
            ::autoboost::is_fundamental< Rhs_nocv >::value,\
            ::autoboost::is_pointer< Rhs_noref >::value\
         >::value,\
         ::autoboost::is_const< Lhs_noref >::value\
      >::value\
   >::value


#include <boost/type_traits/detail/has_binary_operator.hpp>

#undef BOOST_TT_TRAIT_NAME
#undef BOOST_TT_TRAIT_OP
#undef BOOST_TT_FORBIDDEN_IF

#endif
