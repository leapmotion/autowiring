// Copyright David Abrahams, Daniel Wallin 2003. Use, modification and
// distribution is subject to the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_PARAMETER_MACROS_050412_HPP
#define AUTOBOOST_PARAMETER_MACROS_050412_HPP

#include <autoboost/preprocessor/tuple/elem.hpp>
#include <autoboost/preprocessor/repetition/repeat_from_to.hpp>
#include <autoboost/preprocessor/arithmetic/inc.hpp>
#include <autoboost/preprocessor/logical/bool.hpp>
#include <autoboost/preprocessor/punctuation/comma_if.hpp>
#include <autoboost/preprocessor/control/expr_if.hpp>
#include <autoboost/preprocessor/repetition/enum_params.hpp>
#include <autoboost/preprocessor/repetition/enum_binary_params.hpp>
#include <autoboost/preprocessor/cat.hpp>
#include <autoboost/detail/workaround.hpp>

#define AUTOBOOST_PARAMETER_FUN_TEMPLATE_HEAD1(n) \
    template<AUTOBOOST_PP_ENUM_PARAMS(n, class T)>

#define AUTOBOOST_PARAMETER_FUN_TEMPLATE_HEAD0(n)

#if ! defined(AUTOBOOST_NO_SFINAE) && ! AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x592))

# define AUTOBOOST_PARAMETER_MATCH_TYPE(n, param)           \
            AUTOBOOST_PP_EXPR_IF(n, typename) param::match  \
            <                                           \
                AUTOBOOST_PP_ENUM_PARAMS(n, T)              \
            >::type

#else

# define AUTOBOOST_PARAMETER_MATCH_TYPE(n, param) param

#endif

#define AUTOBOOST_PARAMETER_FUN_DECL(z, n, params)                                      \
                                                                                    \
    AUTOBOOST_PP_CAT(AUTOBOOST_PARAMETER_FUN_TEMPLATE_HEAD, AUTOBOOST_PP_BOOL(n))(n)            \
                                                                                    \
    AUTOBOOST_PP_TUPLE_ELEM(3, 0, params)                                               \
        AUTOBOOST_PP_TUPLE_ELEM(3, 1, params)(                                          \
            AUTOBOOST_PP_ENUM_BINARY_PARAMS(n, T, const& p)                             \
            AUTOBOOST_PP_COMMA_IF(n)                                                    \
            AUTOBOOST_PARAMETER_MATCH_TYPE(n,AUTOBOOST_PP_TUPLE_ELEM(3, 2, params))         \
            kw = AUTOBOOST_PP_TUPLE_ELEM(3, 2, params)()                                \
        )                                                                           \
    {                                                                               \
        return AUTOBOOST_PP_CAT(AUTOBOOST_PP_TUPLE_ELEM(3, 1, params), _with_named_params)( \
            kw(AUTOBOOST_PP_ENUM_PARAMS(n, p))                                          \
        );                                                                          \
    }

// Generates:
//
// template<class Params>
// ret name ## _with_named_params(Params const&);
//
// template<class T0>
// ret name(T0 const& p0, typename parameters::match<T0>::type kw = parameters())
// {
//     return name ## _with_named_params(kw(p0));
// }
//
// template<class T0, ..., class TN>
// ret name(T0 const& p0, ..., TN const& PN
//    , typename parameters::match<T0, ..., TN>::type kw = parameters())
// {
//     return name ## _with_named_params(kw(p0, ..., pN));
// }
//
// template<class Params>
// ret name ## _with_named_params(Params const&)
//
// lo and hi determines the min and max arity of the generated functions.

#define AUTOBOOST_PARAMETER_FUN(ret, name, lo, hi, parameters)                          \
                                                                                    \
    template<class Params>                                                          \
    ret AUTOBOOST_PP_CAT(name, _with_named_params)(Params const& p);                    \
                                                                                    \
    AUTOBOOST_PP_REPEAT_FROM_TO(                                                        \
        lo, AUTOBOOST_PP_INC(hi), AUTOBOOST_PARAMETER_FUN_DECL, (ret, name, parameters))    \
                                                                                    \
    template<class Params>                                                          \
    ret AUTOBOOST_PP_CAT(name, _with_named_params)(Params const& p)

#define AUTOBOOST_PARAMETER_MEMFUN(ret, name, lo, hi, parameters)                       \
                                                                                    \
    AUTOBOOST_PP_REPEAT_FROM_TO(                                                        \
        lo, AUTOBOOST_PP_INC(hi), AUTOBOOST_PARAMETER_FUN_DECL, (ret, name, parameters))    \
                                                                                    \
    template<class Params>                                                          \
    ret AUTOBOOST_PP_CAT(name, _with_named_params)(Params const& p)

#endif // AUTOBOOST_PARAMETER_MACROS_050412_HPP

