// Copyright David Abrahams 2006. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// #include guards intentionally disabled.
// #ifndef AUTOBOOST_DETAIL_FUNCTION_N_DWA2006514_HPP
// # define AUTOBOOST_DETAIL_FUNCTION_N_DWA2006514_HPP

#include <autoboost/mpl/void.hpp>
#include <autoboost/mpl/apply.hpp>

#include <autoboost/preprocessor/control/if.hpp>
#include <autoboost/preprocessor/cat.hpp>
#include <autoboost/preprocessor/punctuation/comma_if.hpp>
#include <autoboost/preprocessor/repetition/enum_params.hpp>
#include <autoboost/preprocessor/repetition/enum_trailing_params.hpp>
#include <autoboost/preprocessor/repetition/repeat.hpp>
#include <autoboost/preprocessor/seq/fold_left.hpp>
#include <autoboost/preprocessor/seq/seq.hpp>
#include <autoboost/preprocessor/seq/for_each.hpp>
#include <autoboost/preprocessor/seq/for_each_i.hpp>
#include <autoboost/preprocessor/seq/for_each_product.hpp>
#include <autoboost/preprocessor/seq/size.hpp>
#include <autoboost/type_traits/add_const.hpp>
#include <autoboost/type_traits/remove_reference.hpp>

namespace autoboost { namespace detail {

# define AUTOBOOST_DETAIL_default_arg(z, n, _)                                      \
    typedef mpl::void_ AUTOBOOST_PP_CAT(arg, n);

# define AUTOBOOST_DETAIL_function_arg(z, n, _)                                     \
    typedef typename remove_reference<                                          \
        typename add_const< AUTOBOOST_PP_CAT(A, n) >::type                          \
    >::type AUTOBOOST_PP_CAT(arg, n);

#define AUTOBOOST_DETAIL_cat_arg_counts(s, state, n)                                \
    AUTOBOOST_PP_IF(                                                                \
        n                                                                       \
      , AUTOBOOST_PP_CAT(state, AUTOBOOST_PP_CAT(_, n))                                 \
      , state                                                                   \
    )                                                                           \
    /**/

#define function_name                                                           \
    AUTOBOOST_PP_SEQ_FOLD_LEFT(                                                     \
        AUTOBOOST_DETAIL_cat_arg_counts                                             \
      , AUTOBOOST_PP_CAT(function, AUTOBOOST_PP_SEQ_HEAD(args))                         \
      , AUTOBOOST_PP_SEQ_TAIL(args)(0)                                              \
    )                                                                           \
    /**/

template<typename F>
struct function_name
{
    AUTOBOOST_PP_REPEAT(
        AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY
      , AUTOBOOST_DETAIL_default_arg
      , ~
    )

    template<typename Signature>
    struct result {};

#define AUTOBOOST_DETAIL_function_result(r, _, n)                                   \
    template<typename This AUTOBOOST_PP_ENUM_TRAILING_PARAMS(n, typename A)>        \
    struct result<This(AUTOBOOST_PP_ENUM_PARAMS(n, A))>                             \
    {                                                                           \
        AUTOBOOST_PP_REPEAT(n, AUTOBOOST_DETAIL_function_arg, ~)                        \
        typedef                                                                 \
            typename AUTOBOOST_PP_CAT(mpl::apply, AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY)<\
                F                                                               \
                AUTOBOOST_PP_ENUM_TRAILING_PARAMS(                                  \
                    AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY                          \
                  , arg                                                         \
                )                                                               \
            >::type                                                             \
        impl;                                                                   \
        typedef typename impl::result_type type;                                \
    };                                                                          \
    /**/

    AUTOBOOST_PP_SEQ_FOR_EACH(AUTOBOOST_DETAIL_function_result, _, args)

# define arg_type(r, _, i, is_const)                                            \
    AUTOBOOST_PP_COMMA_IF(i) AUTOBOOST_PP_CAT(A, i) AUTOBOOST_PP_CAT(const_if, is_const) &

# define result_(r, n, constness)                                               \
    typename result<                                                            \
        function_name(                                                          \
            AUTOBOOST_PP_SEQ_FOR_EACH_I_R(r, arg_type, ~, constness)                \
        )                                                                       \
    >                                                                           \
    /**/

# define param(r, _, i, is_const) AUTOBOOST_PP_COMMA_IF(i)                          \
    AUTOBOOST_PP_CAT(A, i) AUTOBOOST_PP_CAT(const_if, is_const) & AUTOBOOST_PP_CAT(x, i)

# define param_list(r, n, constness)                                            \
    AUTOBOOST_PP_SEQ_FOR_EACH_I_R(r, param, ~, constness)

# define call_operator(r, constness)                                            \
    template<AUTOBOOST_PP_ENUM_PARAMS(AUTOBOOST_PP_SEQ_SIZE(constness), typename A)>    \
        result_(r, AUTOBOOST_PP_SEQ_SIZE(constness), constness)::type               \
    operator ()( param_list(r, AUTOBOOST_PP_SEQ_SIZE(constness), constness) ) const \
    {                                                                           \
        typedef result_(r, AUTOBOOST_PP_SEQ_SIZE(constness), constness)::impl impl; \
        return impl()(AUTOBOOST_PP_ENUM_PARAMS(AUTOBOOST_PP_SEQ_SIZE(constness), x));   \
    }                                                                           \
    /**/

# define const_if0
# define const_if1 const

# define bits(z, n, _) ((0)(1))

# define gen_operator(r, _, n)                                                  \
    AUTOBOOST_PP_SEQ_FOR_EACH_PRODUCT_R(                                            \
        r                                                                       \
      , call_operator                                                           \
      , AUTOBOOST_PP_REPEAT(n, bits, ~)                                             \
    )                                                                           \
    /**/

    AUTOBOOST_PP_SEQ_FOR_EACH(
        gen_operator
      , ~
      , args
    )

# undef bits
# undef const_if1
# undef const_if0
# undef call_operator
# undef param_list
# undef param
# undef result_
# undef default_
# undef arg_type
# undef gen_operator
# undef function_name

# undef args
};

}} // namespace autoboost::detail

//#endif // AUTOBOOST_DETAIL_FUNCTION_N_DWA2006514_HPP
