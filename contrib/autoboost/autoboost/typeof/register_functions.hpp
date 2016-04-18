// Copyright (C) 2004 Arkadiy Vertleyb
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_TYPEOF_REGISTER_FUNCTIONS_HPP_INCLUDED
#define AUTOBOOST_TYPEOF_REGISTER_FUNCTIONS_HPP_INCLUDED

#include <autoboost/preprocessor/repetition/enum.hpp>
#include <autoboost/preprocessor/repetition/enum_params.hpp>
#include <autoboost/preprocessor/repetition/enum_trailing_params.hpp>
#include <autoboost/preprocessor/cat.hpp>
#include <autoboost/preprocessor/inc.hpp>
#include <autoboost/preprocessor/dec.hpp>
#include <autoboost/preprocessor/if.hpp>
#include <autoboost/preprocessor/arithmetic/add.hpp>
#include <autoboost/preprocessor/iteration/iterate.hpp>

#include AUTOBOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()

#ifndef AUTOBOOST_TYPEOF_LIMIT_FUNCTION_ARITY
#define AUTOBOOST_TYPEOF_LIMIT_FUNCTION_ARITY 10
#endif

enum
{
    FUN_ID                          = AUTOBOOST_TYPEOF_UNIQUE_ID(),
    FUN_PTR_ID                      = FUN_ID +  1 * AUTOBOOST_PP_INC(AUTOBOOST_TYPEOF_LIMIT_FUNCTION_ARITY),
    FUN_REF_ID                      = FUN_ID +  2 * AUTOBOOST_PP_INC(AUTOBOOST_TYPEOF_LIMIT_FUNCTION_ARITY),
    MEM_FUN_ID                      = FUN_ID +  3 * AUTOBOOST_PP_INC(AUTOBOOST_TYPEOF_LIMIT_FUNCTION_ARITY),
    CONST_MEM_FUN_ID                = FUN_ID +  4 * AUTOBOOST_PP_INC(AUTOBOOST_TYPEOF_LIMIT_FUNCTION_ARITY),
    VOLATILE_MEM_FUN_ID             = FUN_ID +  5 * AUTOBOOST_PP_INC(AUTOBOOST_TYPEOF_LIMIT_FUNCTION_ARITY),
    VOLATILE_CONST_MEM_FUN_ID       = FUN_ID +  6 * AUTOBOOST_PP_INC(AUTOBOOST_TYPEOF_LIMIT_FUNCTION_ARITY),
    FUN_VAR_ID                      = FUN_ID +  7 * AUTOBOOST_PP_INC(AUTOBOOST_TYPEOF_LIMIT_FUNCTION_ARITY),
    FUN_VAR_PTR_ID                  = FUN_ID +  8 * AUTOBOOST_PP_INC(AUTOBOOST_TYPEOF_LIMIT_FUNCTION_ARITY),
    FUN_VAR_REF_ID                  = FUN_ID +  9 * AUTOBOOST_PP_INC(AUTOBOOST_TYPEOF_LIMIT_FUNCTION_ARITY),
    MEM_FUN_VAR_ID                  = FUN_ID + 10 * AUTOBOOST_PP_INC(AUTOBOOST_TYPEOF_LIMIT_FUNCTION_ARITY),
    CONST_MEM_FUN_VAR_ID            = FUN_ID + 11 * AUTOBOOST_PP_INC(AUTOBOOST_TYPEOF_LIMIT_FUNCTION_ARITY),
    VOLATILE_MEM_FUN_VAR_ID         = FUN_ID + 12 * AUTOBOOST_PP_INC(AUTOBOOST_TYPEOF_LIMIT_FUNCTION_ARITY),
    VOLATILE_CONST_MEM_FUN_VAR_ID   = FUN_ID + 13 * AUTOBOOST_PP_INC(AUTOBOOST_TYPEOF_LIMIT_FUNCTION_ARITY)
};

AUTOBOOST_TYPEOF_BEGIN_ENCODE_NS

# define AUTOBOOST_PP_ITERATION_LIMITS (0, AUTOBOOST_TYPEOF_LIMIT_FUNCTION_ARITY)
# define AUTOBOOST_PP_FILENAME_1 <autoboost/typeof/register_functions_iterate.hpp>
# include AUTOBOOST_PP_ITERATE()

AUTOBOOST_TYPEOF_END_ENCODE_NS

#endif//AUTOBOOST_TYPEOF_REGISTER_FUNCTIONS_HPP_INCLUDED
