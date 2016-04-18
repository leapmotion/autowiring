
// NO INCLUDE GUARDS, THE HEADER IS INTENDED FOR MULTIPLE INCLUSION

// Copyright Aleksey Gurtovoy 2000-2004
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/preprocessor/expr_if.hpp>
#include <autoboost/preprocessor/inc.hpp>
#include <autoboost/preprocessor/cat.hpp>

#if !defined(AUX778076_COUNT_ARGS_PARAM_NAME)
#   define AUX778076_COUNT_ARGS_PARAM_NAME T
#endif

#if !defined(AUX778076_COUNT_ARGS_TEMPLATE_PARAM)
#   define AUX778076_COUNT_ARGS_TEMPLATE_PARAM typename AUX778076_COUNT_ARGS_PARAM_NAME
#endif

// local macros, #undef-ined at the end of the header

#if !defined(AUX778076_COUNT_ARGS_USE_STANDARD_PP_PRIMITIVES)

#   include <autoboost/mpl/aux_/preprocessor/repeat.hpp>
#   include <autoboost/mpl/aux_/preprocessor/params.hpp>

#   define AUX778076_COUNT_ARGS_REPEAT AUTOBOOST_MPL_PP_REPEAT
#   define AUX778076_COUNT_ARGS_PARAMS(param) \
    AUTOBOOST_MPL_PP_PARAMS( \
          AUX778076_COUNT_ARGS_ARITY \
        , param \
        ) \
    /**/

#else

#   include <autoboost/preprocessor/enum_shifted_params.hpp>
#   include <autoboost/preprocessor/repeat.hpp>
#   include <autoboost/preprocessor/inc.hpp>

#   define AUX778076_COUNT_ARGS_REPEAT AUTOBOOST_PP_REPEAT
#   define AUX778076_COUNT_ARGS_PARAMS(param) \
    AUTOBOOST_PP_ENUM_SHIFTED_PARAMS( \
          AUTOBOOST_PP_INC(AUX778076_COUNT_ARGS_ARITY) \
        , param \
        ) \
    /**/

#endif // AUX778076_COUNT_ARGS_USE_STANDARD_PP_PRIMITIVES


#define AUX778076_IS_ARG_TEMPLATE_NAME \
    AUTOBOOST_PP_CAT(is_,AUTOBOOST_PP_CAT(AUX778076_COUNT_ARGS_PREFIX,_arg)) \
/**/

#define AUX778076_COUNT_ARGS_FUNC(unused, i, param) \
    AUTOBOOST_PP_EXPR_IF(i, +) \
    AUX778076_IS_ARG_TEMPLATE_NAME<AUTOBOOST_PP_CAT(param,AUTOBOOST_PP_INC(i))>::value \
/**/

// is_<xxx>_arg
template< AUX778076_COUNT_ARGS_TEMPLATE_PARAM >
struct AUX778076_IS_ARG_TEMPLATE_NAME
{
    AUTOBOOST_STATIC_CONSTANT(bool, value = true);
};

template<>
struct AUX778076_IS_ARG_TEMPLATE_NAME<AUX778076_COUNT_ARGS_DEFAULT>
{
    AUTOBOOST_STATIC_CONSTANT(bool, value = false);
};

// <xxx>_count_args
template<
      AUX778076_COUNT_ARGS_PARAMS(AUX778076_COUNT_ARGS_TEMPLATE_PARAM)
    >
struct AUTOBOOST_PP_CAT(AUX778076_COUNT_ARGS_PREFIX,_count_args)
{
    AUTOBOOST_STATIC_CONSTANT(int, value = AUX778076_COUNT_ARGS_REPEAT(
          AUX778076_COUNT_ARGS_ARITY
        , AUX778076_COUNT_ARGS_FUNC
        , AUX778076_COUNT_ARGS_PARAM_NAME
        ));
};

#undef AUX778076_COUNT_ARGS_FUNC
#undef AUX778076_IS_ARG_TEMPLATE_NAME
#undef AUX778076_COUNT_ARGS_PARAMS
#undef AUX778076_COUNT_ARGS_REPEAT

#undef AUX778076_COUNT_ARGS_ARITY
#undef AUX778076_COUNT_ARGS_DEFAULT
#undef AUX778076_COUNT_ARGS_PREFIX
#undef AUX778076_COUNT_ARGS_USE_STANDARD_PP_PRIMITIVES
#undef AUX778076_COUNT_ARGS_TEMPLATE_PARAM
#undef AUX778076_COUNT_ARGS_PARAM_NAME
