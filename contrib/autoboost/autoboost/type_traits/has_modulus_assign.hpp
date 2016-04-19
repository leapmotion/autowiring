//  (C) Copyright 2009-2011 Frederic Bron.
//
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_HAS_MODULUS_ASSIGN_HPP_INCLUDED
#define AUTOBOOST_TT_HAS_MODULUS_ASSIGN_HPP_INCLUDED

#define AUTOBOOST_TT_TRAIT_NAME has_modulus_assign
#define AUTOBOOST_TT_TRAIT_OP %=
#define AUTOBOOST_TT_FORBIDDEN_IF\
   (\
      /* Lhs==fundamental and Rhs==fundamental and (Lhs!=integral or Rhs!=integral) */\
      (\
         ::autoboost::is_fundamental< Lhs_nocv >::value && \
         ::autoboost::is_fundamental< Rhs_nocv >::value && \
         ( \
            (!  ::autoboost::is_integral< Lhs_noref >::value ) || \
            (!  ::autoboost::is_integral< Rhs_noref >::value )\
         )\
      )||\
      /* Lhs==fundamental and Rhs==pointer */\
      (\
         ::autoboost::is_fundamental< Lhs_nocv >::value && \
         ::autoboost::is_pointer< Rhs_noref >::value\
      )||\
      /* Rhs==fundamental and Lhs==pointer */\
      (\
         ::autoboost::is_fundamental< Rhs_nocv >::value && \
         ::autoboost::is_pointer< Lhs_noref >::value\
      )||\
      /* Lhs==pointer and Rhs==pointer */\
      (\
         ::autoboost::is_pointer< Lhs_noref >::value && \
         ::autoboost::is_pointer< Rhs_noref >::value\
      )||\
      /* Lhs==fundamental and Rhs==fundamental and Lhs==const */\
      (\
         ::autoboost::is_fundamental< Lhs_nocv >::value && \
         ::autoboost::is_fundamental< Rhs_nocv >::value && \
         ::autoboost::is_const< Lhs_noref >::value\
      )\
      )


#include <autoboost/type_traits/detail/has_binary_operator.hpp>

#undef AUTOBOOST_TT_TRAIT_NAME
#undef AUTOBOOST_TT_TRAIT_OP
#undef AUTOBOOST_TT_FORBIDDEN_IF

#endif
