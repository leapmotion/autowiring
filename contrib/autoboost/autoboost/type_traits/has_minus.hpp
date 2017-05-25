//  (C) Copyright 2009-2011 Frederic Bron.
//
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_HAS_MINUS_HPP_INCLUDED
#define AUTOBOOST_TT_HAS_MINUS_HPP_INCLUDED

#define AUTOBOOST_TT_TRAIT_NAME has_minus
#define AUTOBOOST_TT_TRAIT_OP -
#define AUTOBOOST_TT_FORBIDDEN_IF\
   (\
      /* Lhs==pointer and Rhs==fundamental and Rhs!=integral */\
      (\
         ::autoboost::is_pointer< Lhs_noref >::value && \
         ::autoboost::is_fundamental< Rhs_nocv >::value && \
         (!  ::autoboost::is_integral< Rhs_noref >::value )\
      ) || \
      /* Lhs==void* and (Rhs==fundamental or Rhs==pointer) */\
      (\
         ::autoboost::is_pointer< Lhs_noref >::value && \
         ::autoboost::is_void< Lhs_noptr >::value && \
         ( \
            ::autoboost::is_fundamental< Rhs_nocv >::value || \
            ::autoboost::is_pointer< Rhs_noref >::value\
          )\
      ) || \
      /* Rhs==void* and (Lhs==fundamental or Lhs==pointer) */\
      (\
         ::autoboost::is_pointer< Rhs_noref >::value && \
         ::autoboost::is_void< Rhs_noptr >::value && \
         (\
            ::autoboost::is_fundamental< Lhs_nocv >::value || \
            ::autoboost::is_pointer< Lhs_noref >::value\
          )\
      ) ||\
      /* Lhs=fundamental and Rhs=pointer */\
      (\
         ::autoboost::is_fundamental< Lhs_nocv >::value && \
         ::autoboost::is_pointer< Rhs_noref >::value\
      ) ||\
      /* two different pointers */\
      (\
         ::autoboost::is_pointer< Lhs_noref >::value && \
         ::autoboost::is_pointer< Rhs_noref >::value && \
         (!  ::autoboost::is_same< Lhs_nocv, Rhs_nocv >::value )\
      )\
      )


#include <autoboost/type_traits/detail/has_binary_operator.hpp>

#undef AUTOBOOST_TT_TRAIT_NAME
#undef AUTOBOOST_TT_TRAIT_OP
#undef AUTOBOOST_TT_FORBIDDEN_IF

#endif
