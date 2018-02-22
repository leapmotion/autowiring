// Copyright Daniel Wallin 2006. Use, modification and distribution is
// subject to the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_PARAMETER_PREPROCESSOR_060206_HPP
# define AUTOBOOST_PARAMETER_PREPROCESSOR_060206_HPP

# include <autoboost/parameter/parameters.hpp>
# include <autoboost/parameter/binding.hpp>
# include <autoboost/parameter/match.hpp>

# include <autoboost/parameter/aux_/parenthesized_type.hpp>
# include <autoboost/parameter/aux_/cast.hpp>
# include <autoboost/parameter/aux_/preprocessor/flatten.hpp>

# include <autoboost/preprocessor/repetition/repeat_from_to.hpp>
# include <autoboost/preprocessor/comparison/equal.hpp>
# include <autoboost/preprocessor/control/if.hpp>
# include <autoboost/preprocessor/control/iif.hpp>
# include <autoboost/preprocessor/control/expr_if.hpp>
# include <autoboost/preprocessor/repetition/enum_params.hpp>
# include <autoboost/preprocessor/repetition/enum_binary_params.hpp>
# include <autoboost/preprocessor/repetition/enum_trailing.hpp>
# include <autoboost/preprocessor/seq/first_n.hpp>
# include <autoboost/preprocessor/seq/for_each_product.hpp>
# include <autoboost/preprocessor/seq/for_each_i.hpp>
# include <autoboost/preprocessor/tuple/elem.hpp>
# include <autoboost/preprocessor/tuple/eat.hpp>
# include <autoboost/preprocessor/seq/fold_left.hpp>
# include <autoboost/preprocessor/seq/push_back.hpp>
# include <autoboost/preprocessor/seq/size.hpp>
# include <autoboost/preprocessor/seq/enum.hpp>
# include <autoboost/preprocessor/seq/push_back.hpp>

# include <autoboost/preprocessor/detail/is_nullary.hpp>

# include <autoboost/mpl/always.hpp>
# include <autoboost/mpl/apply_wrap.hpp>

namespace autoboost { namespace parameter { namespace aux {

#  if ! defined(AUTOBOOST_NO_SFINAE) && ! AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x592))

// Given Match, which is "void x" where x is an argument matching
// criterion, extract a corresponding MPL predicate.
template <class Match>
struct unwrap_predicate;

// Match anything
template <>
struct unwrap_predicate<void*>
{
    typedef mpl::always<mpl::true_> type;
};

#if AUTOBOOST_WORKAROUND(__SUNPRO_CC, AUTOBOOST_TESTED_AT(0x580))

typedef void* voidstar;

// A matching predicate is explicitly specified
template <class Predicate>
struct unwrap_predicate<voidstar (Predicate)>
{
    typedef Predicate type;
};

#else

// A matching predicate is explicitly specified
template <class Predicate>
struct unwrap_predicate<void *(Predicate)>
{
    typedef Predicate type;
};

#endif


// A type to which the argument is supposed to be convertible is
// specified
template <class Target>
struct unwrap_predicate<void (Target)>
{
    typedef is_convertible<mpl::_, Target> type;
};

// Recast the ParameterSpec's nested match metafunction as a free metafunction
template <
    class Parameters
  , AUTOBOOST_PP_ENUM_BINARY_PARAMS(
        AUTOBOOST_PARAMETER_MAX_ARITY, class A, = autoboost::parameter::void_ AUTOBOOST_PP_INTERCEPT
    )
>
struct match
  : Parameters::template match<
        AUTOBOOST_PP_ENUM_PARAMS(AUTOBOOST_PARAMETER_MAX_ARITY, A)
    >
{};
# endif

# undef false_

template <
    class Parameters
  , AUTOBOOST_PP_ENUM_BINARY_PARAMS(
        AUTOBOOST_PARAMETER_MAX_ARITY, class A, = autoboost::parameter::void_ AUTOBOOST_PP_INTERCEPT
    )
>
struct argument_pack
{
    typedef typename make_arg_list<
        typename AUTOBOOST_PARAMETER_build_arg_list(
            AUTOBOOST_PARAMETER_MAX_ARITY, make_items, typename Parameters::parameter_spec, A
        )::type
      , typename Parameters::deduced_list
      , tag_keyword_arg
      , mpl::false_
    >::type result;
    typedef typename mpl::first<result>::type type;
};

// Works around VC6 problem where it won't accept rvalues.
template <class T>
T& as_lvalue(T& value, long)
{
    return value;
}

template <class T>
T const& as_lvalue(T const& value, int)
{
    return value;
}


# if AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x564))

template <class Predicate, class T, class Args>
struct apply_predicate
{
    AUTOBOOST_MPL_ASSERT((
        mpl::and_<mpl::false_,T>
    ));

    typedef typename mpl::if_<
        typename mpl::apply2<Predicate,T,Args>::type
      , char
      , int
    >::type type;
};

template <class P>
struct funptr_predicate
{
    static P p;

    template <class T, class Args, class P0>
    static typename apply_predicate<P0,T,Args>::type
    check_predicate(type<T>, Args*, void**(*)(P0));

    template <class T, class Args, class P0>
    static typename mpl::if_<
        is_convertible<T,P0>
      , char
      , int
     >::type check_predicate(type<T>, Args*, void*(*)(P0));

    template <class T, class Args>
    struct apply
    {
        AUTOBOOST_STATIC_CONSTANT(bool, result =
            sizeof(check_predicate(autoboost::type<T>(), (Args*)0, &p)) == 1
        );

        typedef mpl::bool_<apply<T,Args>::result> type;
    };
};

template <>
struct funptr_predicate<void**>
  : mpl::always<mpl::true_>
{};

# endif

}}} // namespace autoboost::parameter::aux

# if AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x564))
// From Paul Mensonides
#  define AUTOBOOST_PARAMETER_IS_NULLARY(x) \
    AUTOBOOST_PP_SPLIT(1, AUTOBOOST_PARAMETER_IS_NULLARY_C x AUTOBOOST_PP_COMMA() 0) \
    /**/
#  define AUTOBOOST_PARAMETER_IS_NULLARY_C() \
    ~, 1 AUTOBOOST_PP_RPAREN() \
    AUTOBOOST_PP_TUPLE_EAT(2) AUTOBOOST_PP_LPAREN() ~ \
    /**/
# else
#  define AUTOBOOST_PARAMETER_IS_NULLARY(x) AUTOBOOST_PP_IS_NULLARY(x)
# endif

# define AUTOBOOST_PARAMETER_MEMBER_FUNCTION_CHECK_STATIC_static ()
# define AUTOBOOST_PARAMETER_MEMBER_FUNCTION_IS_STATIC(name) \
    AUTOBOOST_PARAMETER_IS_NULLARY( \
        AUTOBOOST_PP_CAT(AUTOBOOST_PARAMETER_MEMBER_FUNCTION_CHECK_STATIC_,name) \
    )

# if !defined(AUTOBOOST_MSVC)
#  define AUTOBOOST_PARAMETER_MEMBER_FUNCTION_STRIP_STATIC_static
#  define AUTOBOOST_PARAMETER_MEMBER_FUNCTION_STRIP_STATIC(name) \
    AUTOBOOST_PP_CAT(AUTOBOOST_PARAMETER_MEMBER_FUNCTION_STRIP_STATIC_, name)
# else
// Workaround for MSVC preprocessor.
//
// When stripping static from "static f", msvc will produce
// " f". The leading whitespace doesn't go away when pasting
// the token with something else, so this thing is a hack to
// strip the whitespace.
#  define AUTOBOOST_PARAMETER_MEMBER_FUNCTION_STRIP_STATIC_static (
#  define AUTOBOOST_PARAMETER_MEMBER_FUNCTION_STRIP_STATIC_AUX(name) \
    AUTOBOOST_PP_CAT(AUTOBOOST_PARAMETER_MEMBER_FUNCTION_STRIP_STATIC_, name))
#  define AUTOBOOST_PARAMETER_MEMBER_FUNCTION_STRIP_STATIC(name) \
    AUTOBOOST_PP_SEQ_HEAD( \
        AUTOBOOST_PARAMETER_MEMBER_FUNCTION_STRIP_STATIC_AUX(name) \
    )
# endif

# define AUTOBOOST_PARAMETER_MEMBER_FUNCTION_STATIC(name) \
    AUTOBOOST_PP_EXPR_IF( \
        AUTOBOOST_PARAMETER_MEMBER_FUNCTION_IS_STATIC(name) \
      , static \
    )

# define AUTOBOOST_PARAMETER_MEMBER_FUNCTION_NAME(name) \
    AUTOBOOST_PP_IF( \
        AUTOBOOST_PARAMETER_MEMBER_FUNCTION_IS_STATIC(name) \
      , AUTOBOOST_PARAMETER_MEMBER_FUNCTION_STRIP_STATIC \
      , name AUTOBOOST_PP_TUPLE_EAT(1) \
    )(name)

// Calculates [begin, end) arity range.

# define AUTOBOOST_PARAMETER_ARITY_RANGE_M_optional(state) state
# define AUTOBOOST_PARAMETER_ARITY_RANGE_M_deduced_optional(state) state
# define AUTOBOOST_PARAMETER_ARITY_RANGE_M_required(state) AUTOBOOST_PP_INC(state)
# define AUTOBOOST_PARAMETER_ARITY_RANGE_M_deduced_required(state) AUTOBOOST_PP_INC(state)

# define AUTOBOOST_PARAMETER_ARITY_RANGE_M(s, state, x) \
    AUTOBOOST_PP_CAT( \
        AUTOBOOST_PARAMETER_ARITY_RANGE_M_ \
      , AUTOBOOST_PARAMETER_FN_ARG_QUALIFIER(x) \
    )(state)
/**/

# define AUTOBOOST_PARAMETER_ARITY_RANGE(args) \
    ( \
        AUTOBOOST_PP_SEQ_FOLD_LEFT(AUTOBOOST_PARAMETER_ARITY_RANGE_M, 0, args) \
      , AUTOBOOST_PP_INC(AUTOBOOST_PP_SEQ_SIZE(args)) \
    )
/**/

// Accessor macros for the argument specs tuple.
# define AUTOBOOST_PARAMETER_FN_ARG_QUALIFIER(x) \
    AUTOBOOST_PP_TUPLE_ELEM(4,0,x)
/**/

# define AUTOBOOST_PARAMETER_FN_ARG_NAME(x) \
    AUTOBOOST_PP_TUPLE_ELEM(4,1,x)
/**/

# define AUTOBOOST_PARAMETER_FN_ARG_PRED(x) \
    AUTOBOOST_PP_TUPLE_ELEM(4,2,x)
/**/

# define AUTOBOOST_PARAMETER_FN_ARG_DEFAULT(x) \
    AUTOBOOST_PP_TUPLE_ELEM(4,3,x)
/**/

# define AUTOBOOST_PARAMETETER_FUNCTION_EAT_KEYWORD_QUALIFIER_out(x)
# define AUTOBOOST_PARAMETETER_FUNCTION_EAT_KEYWORD_QUALIFIER_in_out(x)

// Returns 1 if x is either "out(k)" or "in_out(k)".
# define AUTOBOOST_PARAMETER_FUNCTION_IS_KEYWORD_QUALIFIER(x) \
    AUTOBOOST_PP_IS_EMPTY( \
        AUTOBOOST_PP_CAT(AUTOBOOST_PARAMETETER_FUNCTION_EAT_KEYWORD_QUALIFIER_, x) \
    ) \
/**/

# define AUTOBOOST_PARAMETETER_FUNCTION_GET_KEYWORD_QUALIFIER_out(x) x
# define AUTOBOOST_PARAMETETER_FUNCTION_GET_KEYWORD_QUALIFIER_in_out(x) x
# define AUTOBOOST_PARAMETER_FUNCTION_KEYWORD_GET(x) \
    AUTOBOOST_PP_CAT(AUTOBOOST_PARAMETETER_FUNCTION_GET_KEYWORD_QUALIFIER_, x)
/**/

// Returns the keyword of x, where x is either a keyword qualifier
// or a keyword.
//
//   k => k
//   out(k) => k
//   in_out(k) => k
//
# define AUTOBOOST_PARAMETER_FUNCTION_KEYWORD(x) \
    AUTOBOOST_PP_IF( \
        AUTOBOOST_PARAMETER_FUNCTION_IS_KEYWORD_QUALIFIER(x) \
      , AUTOBOOST_PARAMETER_FUNCTION_KEYWORD_GET \
      , x AUTOBOOST_PP_TUPLE_EAT(1) \
    )(x)
/**/

# define AUTOBOOST_PARAMETER_FN_ARG_KEYWORD(x) \
    AUTOBOOST_PARAMETER_FUNCTION_KEYWORD( \
        AUTOBOOST_PARAMETER_FN_ARG_NAME(x) \
    )

// Builds forwarding functions.

# define AUTOBOOST_PARAMETER_FUNCTION_FWD_FUNCTION_TEMPLATE_Z(z, n) \
    template<AUTOBOOST_PP_ENUM_PARAMS_Z(z, n, class ParameterArgumentType)>
/**/

# if ! defined(AUTOBOOST_NO_SFINAE) && ! AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x592))
#  define AUTOBOOST_PARAMETER_FUNCTION_FWD_MATCH_Z(z, name, parameters, n) \
    , typename autoboost::parameter::aux::match< \
          parameters, AUTOBOOST_PP_ENUM_PARAMS(n, ParameterArgumentType) \
      >::type = parameters()
# else
#  define AUTOBOOST_PARAMETER_FUNCTION_FWD_MATCH_Z(z, name, parameters, n)
# endif
/**/

# define AUTOBOOST_PARAMETER_FUNCTION_PARAMETERS_NAME(base) \
    AUTOBOOST_PP_CAT( \
        autoboost_param_parameters_ \
      , AUTOBOOST_PP_CAT(__LINE__, AUTOBOOST_PARAMETER_MEMBER_FUNCTION_NAME(base)) \
    )

// Produce a name for a result type metafunction for the function
// named base
# define AUTOBOOST_PARAMETER_FUNCTION_RESULT_NAME(base) \
    AUTOBOOST_PP_CAT( \
        autoboost_param_result_ \
      , AUTOBOOST_PP_CAT(__LINE__,AUTOBOOST_PARAMETER_MEMBER_FUNCTION_NAME(base)) \
    )

// Can't do autoboost_param_impl_ ## basee because base might start with an underscore
// daniel: what? how is that relevant? the reason for using CAT() is to make sure
// base is expanded. i'm not sure we need to here, but it's more stable to do it.
# define AUTOBOOST_PARAMETER_IMPL(base) \
    AUTOBOOST_PP_CAT(autoboost_param_impl,AUTOBOOST_PARAMETER_MEMBER_FUNCTION_NAME(base))

# define AUTOBOOST_PARAMETER_FUNCTION_FWD_FUNCTION00(z, n, r, data, elem) \
    AUTOBOOST_PP_IF( \
        n \
      , AUTOBOOST_PARAMETER_FUNCTION_FWD_FUNCTION_TEMPLATE_Z, AUTOBOOST_PP_TUPLE_EAT(2) \
    )(z,n) \
    AUTOBOOST_PARAMETER_MEMBER_FUNCTION_STATIC(AUTOBOOST_PP_TUPLE_ELEM(7,3,data)) \
    inline \
    AUTOBOOST_PP_EXPR_IF(n, typename) \
        AUTOBOOST_PARAMETER_FUNCTION_RESULT_NAME(AUTOBOOST_PP_TUPLE_ELEM(7,3,data))<   \
        AUTOBOOST_PP_EXPR_IF(n, typename) \
        autoboost::parameter::aux::argument_pack< \
            AUTOBOOST_PARAMETER_FUNCTION_PARAMETERS_NAME(AUTOBOOST_PP_TUPLE_ELEM(7,3,data)) \
            AUTOBOOST_PP_COMMA_IF(n) \
            AUTOBOOST_PP_IF( \
                n, AUTOBOOST_PP_SEQ_ENUM, AUTOBOOST_PP_TUPLE_EAT(1) \
            )(elem) \
        >::type \
    >::type \
    AUTOBOOST_PARAMETER_MEMBER_FUNCTION_NAME(AUTOBOOST_PP_TUPLE_ELEM(7,3,data))( \
        AUTOBOOST_PP_IF( \
            n \
          , AUTOBOOST_PP_SEQ_FOR_EACH_I_R \
          , AUTOBOOST_PP_TUPLE_EAT(4) \
        )( \
            r \
          , AUTOBOOST_PARAMETER_FUNCTION_ARGUMENT \
          , ~ \
          , elem \
        ) \
        AUTOBOOST_PP_IF(n, AUTOBOOST_PARAMETER_FUNCTION_FWD_MATCH_Z, AUTOBOOST_PP_TUPLE_EAT(4))( \
            z \
          , AUTOBOOST_PP_TUPLE_ELEM(7,3,data) \
          , AUTOBOOST_PARAMETER_FUNCTION_PARAMETERS_NAME(AUTOBOOST_PP_TUPLE_ELEM(7,3,data)) \
          , n \
        ) \
    ) AUTOBOOST_PP_EXPR_IF(AUTOBOOST_PP_TUPLE_ELEM(7,4,data), const) \
    { \
        return AUTOBOOST_PARAMETER_IMPL(AUTOBOOST_PP_TUPLE_ELEM(7,3,data))( \
            AUTOBOOST_PARAMETER_FUNCTION_PARAMETERS_NAME(AUTOBOOST_PP_TUPLE_ELEM(7,3,data))()( \
                AUTOBOOST_PP_ENUM_PARAMS_Z(z, n, a) \
            ) \
        ); \
    }
/**/

# define AUTOBOOST_PARAMETER_FUNCTION_FWD_FUNCTION0(r, data, elem) \
    AUTOBOOST_PARAMETER_FUNCTION_FWD_FUNCTION00( \
        AUTOBOOST_PP_TUPLE_ELEM(7,0,data) \
      , AUTOBOOST_PP_TUPLE_ELEM(7,1,data) \
      , r \
      , data \
      , elem \
    )
/**/

# define AUTOBOOST_PARAMETER_FUNCTION_FWD_FUNCTION_ARITY_0(z, n, data) \
    AUTOBOOST_PARAMETER_FUNCTION_FWD_FUNCTION00( \
        z, n, AUTOBOOST_PP_DEDUCE_R() \
      , (z, n, AUTOBOOST_PP_TUPLE_REM(5) data) \
      , ~ \
    )
/**/

# define AUTOBOOST_PARAMETER_FUNCTION_FWD_FUNCTION_ARITY_N(z, n, data) \
    AUTOBOOST_PP_SEQ_FOR_EACH( \
        AUTOBOOST_PARAMETER_FUNCTION_FWD_FUNCTION0 \
      , (z, n, AUTOBOOST_PP_TUPLE_REM(5) data) \
      , AUTOBOOST_PP_SEQ_FOR_EACH_PRODUCT( \
            AUTOBOOST_PARAMETER_FUNCTION_FWD_PRODUCT \
          , AUTOBOOST_PP_SEQ_FIRST_N( \
                n, AUTOBOOST_PP_TUPLE_ELEM(5,3,data) \
            ) \
        ) \
    )
/**/

# define AUTOBOOST_PARAMETER_FUNCTION_FWD_FUNCTION(z, n, data) \
    AUTOBOOST_PP_IF( \
        n \
      , AUTOBOOST_PARAMETER_FUNCTION_FWD_FUNCTION_ARITY_N \
      , AUTOBOOST_PARAMETER_FUNCTION_FWD_FUNCTION_ARITY_0 \
    )(z,n,data) \
/**/

# define AUTOBOOST_PARAMETER_FUNCTION_FWD_FUNCTIONS0( \
    result,name,args,const_,combinations,range \
) \
    AUTOBOOST_PP_REPEAT_FROM_TO( \
        AUTOBOOST_PP_TUPLE_ELEM(2,0,range), AUTOBOOST_PP_TUPLE_ELEM(2,1,range) \
      , AUTOBOOST_PARAMETER_FUNCTION_FWD_FUNCTION \
      , (result,name,const_,combinations,AUTOBOOST_PP_TUPLE_ELEM(2,1,range)) \
    )
/**/

# define AUTOBOOST_PARAMETER_FUNCTION_FWD_FUNCTIONS(result,name,args, const_, combinations) \
    AUTOBOOST_PARAMETER_FUNCTION_FWD_FUNCTIONS0( \
        result, name, args, const_, combinations, AUTOBOOST_PARAMETER_ARITY_RANGE(args) \
    )
/**/

// Builds autoboost::parameter::parameters<> specialization
#  define AUTOBOOST_PARAMETER_FUNCTION_PARAMETERS_QUALIFIER_optional(tag) \
    optional<tag

#  define AUTOBOOST_PARAMETER_FUNCTION_PARAMETERS_QUALIFIER_required(tag) \
    required<tag

#  define AUTOBOOST_PARAMETER_FUNCTION_PARAMETERS_QUALIFIER_deduced_optional(tag) \
    optional<autoboost::parameter::deduced<tag>

#  define AUTOBOOST_PARAMETER_FUNCTION_PARAMETERS_QUALIFIER_deduced_required(tag) \
    required<autoboost::parameter::deduced<tag>

# if !AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x564))

#  define AUTOBOOST_PARAMETER_FUNCTION_PARAMETERS_M(r,tag_namespace,i,elem) \
    AUTOBOOST_PP_COMMA_IF(i) \
    autoboost::parameter::AUTOBOOST_PP_CAT( \
        AUTOBOOST_PARAMETER_FUNCTION_PARAMETERS_QUALIFIER_ \
      , AUTOBOOST_PARAMETER_FN_ARG_QUALIFIER(elem) \
    )( \
        tag_namespace::AUTOBOOST_PARAMETER_FUNCTION_KEYWORD( \
            AUTOBOOST_PARAMETER_FN_ARG_KEYWORD(elem) \
        ) \
    ) \
      , typename autoboost::parameter::aux::unwrap_predicate< \
            void AUTOBOOST_PARAMETER_FN_ARG_PRED(elem) \
        >::type \
    >
# elif AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x564))
#  define AUTOBOOST_PARAMETER_FUNCTION_PARAMETERS_M(r,tag_namespace,i,elem) \
    AUTOBOOST_PP_COMMA_IF(i) \
    autoboost::parameter::AUTOBOOST_PP_CAT( \
        AUTOBOOST_PARAMETER_FUNCTION_PARAMETERS_QUALIFIER_ \
      , AUTOBOOST_PARAMETER_FN_ARG_QUALIFIER(elem) \
    )( \
        tag_namespace::AUTOBOOST_PARAMETER_FUNCTION_KEYWORD( \
            AUTOBOOST_PARAMETER_FN_ARG_KEYWORD(elem) \
        ) \
    ) \
      , autoboost::mpl::always<autoboost::mpl::true_> \
    >
# endif

# define AUTOBOOST_PARAMETER_FUNCTION_PARAMETERS(tag_namespace, base, args)             \
    template <class BoostParameterDummy>                                            \
    struct AUTOBOOST_PP_CAT(                                                            \
            AUTOBOOST_PP_CAT(autoboost_param_params_, __LINE__)                             \
          , AUTOBOOST_PARAMETER_MEMBER_FUNCTION_NAME(base)                              \
    ) : autoboost::parameter::parameters<                                               \
            AUTOBOOST_PP_SEQ_FOR_EACH_I(                                                \
                AUTOBOOST_PARAMETER_FUNCTION_PARAMETERS_M, tag_namespace, args          \
            )                                                                       \
        >                                                                           \
    {};                                                                             \
                                                                                    \
    typedef AUTOBOOST_PP_CAT( \
            AUTOBOOST_PP_CAT(autoboost_param_params_, __LINE__) \
          , AUTOBOOST_PARAMETER_MEMBER_FUNCTION_NAME(base) \
    )<int>

// Defines result type metafunction
# define AUTOBOOST_PARAMETER_FUNCTION_RESULT_ARG(z, _, i, x) \
    AUTOBOOST_PP_COMMA_IF(i) class AUTOBOOST_PP_TUPLE_ELEM(3,1,x)
/**/

# define AUTOBOOST_PARAMETER_FUNCTION_RESULT_(result, name, args)                                   \
    template <class Args>                                                                       \
    struct AUTOBOOST_PARAMETER_FUNCTION_RESULT_NAME(name)                                           \
    {                                                                                           \
        typedef typename AUTOBOOST_PARAMETER_PARENTHESIZED_TYPE(result) type;                       \
    };

// Defines implementation function
# define AUTOBOOST_PARAMETER_FUNCTION_IMPL_HEAD(name)           \
    template <class Args>                                   \
    typename AUTOBOOST_PARAMETER_FUNCTION_RESULT_NAME(name)<    \
       Args                                                 \
    >::type AUTOBOOST_PARAMETER_IMPL(name)(Args const& args)

# define AUTOBOOST_PARAMETER_FUNCTION_IMPL_FWD(name) \
    AUTOBOOST_PARAMETER_FUNCTION_IMPL_HEAD(name);
/**/

# define AUTOBOOST_PARAMETER_FUNCTION_SPLIT_ARG_required(state, arg) \
    ( \
        AUTOBOOST_PP_INC(AUTOBOOST_PP_TUPLE_ELEM(4, 0, state)) \
      , AUTOBOOST_PP_SEQ_PUSH_BACK(AUTOBOOST_PP_TUPLE_ELEM(4, 1, state), arg) \
      , AUTOBOOST_PP_TUPLE_ELEM(4, 2, state) \
      , AUTOBOOST_PP_TUPLE_ELEM(4, 3, state) \
    )

# define AUTOBOOST_PARAMETER_FUNCTION_SPLIT_ARG_deduced_required(state, arg) \
    AUTOBOOST_PARAMETER_FUNCTION_SPLIT_ARG_required(state, arg)

# define AUTOBOOST_PARAMETER_FUNCTION_SPLIT_ARG_optional(state, arg) \
    ( \
        AUTOBOOST_PP_TUPLE_ELEM(4, 0, state) \
      , AUTOBOOST_PP_TUPLE_ELEM(4, 1, state) \
      , AUTOBOOST_PP_INC(AUTOBOOST_PP_TUPLE_ELEM(4, 2, state)) \
      , AUTOBOOST_PP_SEQ_PUSH_BACK(AUTOBOOST_PP_TUPLE_ELEM(4, 3, state), arg) \
    )

# define AUTOBOOST_PARAMETER_FUNCTION_SPLIT_ARG_deduced_optional(state, arg) \
    AUTOBOOST_PARAMETER_FUNCTION_SPLIT_ARG_optional(state, arg)

# define AUTOBOOST_PARAMETER_FUNCTION_SPLIT_ARG(s, state, arg) \
    AUTOBOOST_PP_CAT( \
        AUTOBOOST_PARAMETER_FUNCTION_SPLIT_ARG_ \
      , AUTOBOOST_PARAMETER_FN_ARG_QUALIFIER(arg) \
    )(state, arg)

// Returns (required_count, required, optional_count, optionals) tuple
# define AUTOBOOST_PARAMETER_FUNCTION_SPLIT_ARGS(args) \
    AUTOBOOST_PP_SEQ_FOLD_LEFT( \
        AUTOBOOST_PARAMETER_FUNCTION_SPLIT_ARG \
      , (0,AUTOBOOST_PP_SEQ_NIL, 0,AUTOBOOST_PP_SEQ_NIL) \
      , args \
    )

# define AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION_ARG_NAME(keyword) \
    AUTOBOOST_PP_CAT(AUTOBOOST_PP_CAT(keyword,_),type)

// Helpers used as parameters to AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_ARGUMENTS.
# define AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION_TEMPLATE_ARG(r, _, arg) \
    , class AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION_ARG_NAME( \
              AUTOBOOST_PARAMETER_FN_ARG_KEYWORD(arg) \
      )

# define AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION_ARG(r, _, arg) \
    , AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION_ARG_NAME( \
              AUTOBOOST_PARAMETER_FN_ARG_KEYWORD(arg) \
      )& AUTOBOOST_PARAMETER_FN_ARG_KEYWORD(arg)

# define AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION_PARAMETER(r, _, arg) \
    , AUTOBOOST_PARAMETER_FN_ARG_KEYWORD(arg)

// Produces a name for the dispatch functions.
# define AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_NAME(name) \
    AUTOBOOST_PP_CAT( \
        autoboost_param_default_ \
      , AUTOBOOST_PP_CAT(__LINE__, AUTOBOOST_PARAMETER_MEMBER_FUNCTION_NAME(name)) \
    )

// Helper macro used below to produce lists based on the keyword argument
// names. macro is applied to every element. n is the number of
// optional arguments that should be included.
# define AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_ARGUMENTS(macro, n, split_args) \
    AUTOBOOST_PP_SEQ_FOR_EACH( \
        macro \
      , ~ \
      , AUTOBOOST_PP_TUPLE_ELEM(4,1,split_args) \
    ) \
    AUTOBOOST_PP_SEQ_FOR_EACH( \
        macro \
      , ~ \
      , AUTOBOOST_PP_SEQ_FIRST_N( \
          AUTOBOOST_PP_SUB(AUTOBOOST_PP_TUPLE_ELEM(4,2,split_args), n) \
        , AUTOBOOST_PP_TUPLE_ELEM(4,3,split_args) \
        ) \
    )

// Generates a keyword | default expression.
# define AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_EVAL_DEFAULT(arg, tag_namespace) \
    autoboost::parameter::keyword< \
        tag_namespace::AUTOBOOST_PARAMETER_FN_ARG_KEYWORD(arg) \
    >::instance | autoboost::parameter::aux::use_default_tag()

# define AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION_GET_ARG(arg, tag_ns) \
    AUTOBOOST_PARAMETER_FUNCTION_CAST( \
        args[ \
            AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_EVAL_DEFAULT( \
                arg, tag_ns \
            ) \
        ] \
      , AUTOBOOST_PARAMETER_FN_ARG_PRED(arg) \
      , Args \
    )

# define AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION_BODY(name, n, split_args, tag_namespace) \
    { \
        return AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_NAME(name)( \
            (ResultType(*)())0 \
          , args \
          , 0L \
            AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_ARGUMENTS( \
                AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION_PARAMETER \
              , n \
              , split_args \
            ) \
          , AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION_GET_ARG( \
                AUTOBOOST_PP_SEQ_ELEM( \
                    AUTOBOOST_PP_SUB(AUTOBOOST_PP_TUPLE_ELEM(4,2,split_args), n) \
                  , AUTOBOOST_PP_TUPLE_ELEM(4,3,split_args) \
                ) \
              , tag_namespace \
            ) \
        ); \
    }

# define AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_EVAL_ACTUAL_DEFAULT(arg) \
    AUTOBOOST_PARAMETER_FUNCTION_CAST( \
        autoboost::parameter::aux::as_lvalue(AUTOBOOST_PARAMETER_FN_ARG_DEFAULT(arg), 0L) \
      , AUTOBOOST_PARAMETER_FN_ARG_PRED(arg) \
      , Args \
    )

# define AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_EVAL_DEFAULT_BODY(name, n, split_args, tag_ns, const_) \
    template < \
        class ResultType \
      , class Args \
        AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_ARGUMENTS( \
            AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION_TEMPLATE_ARG \
          , AUTOBOOST_PP_INC(n) \
          , split_args \
        ) \
    > \
    AUTOBOOST_PARAMETER_MEMBER_FUNCTION_STATIC(name) \
    ResultType AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_NAME(name)( \
        ResultType(*)() \
      , Args const& args \
      , long \
        AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_ARGUMENTS( \
            AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION_ARG \
          , AUTOBOOST_PP_INC(n) \
          , split_args \
        ) \
      , autoboost::parameter::aux::use_default_tag \
    ) AUTOBOOST_PP_EXPR_IF(const_, const) \
    { \
        return AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_NAME(name)( \
            (ResultType(*)())0 \
          , args \
          , 0L \
            AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_ARGUMENTS( \
                AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION_PARAMETER \
              , AUTOBOOST_PP_INC(n) \
              , split_args \
            ) \
          , AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_EVAL_ACTUAL_DEFAULT( \
                AUTOBOOST_PP_SEQ_ELEM( \
                    AUTOBOOST_PP_SUB(AUTOBOOST_PP_TUPLE_ELEM(4,2,split_args), AUTOBOOST_PP_INC(n)) \
                  , AUTOBOOST_PP_TUPLE_ELEM(4,3,split_args) \
                ) \
            ) \
        ); \
    }

// Produces a forwarding layer in the default evaluation machine.
//
// data is a tuple:
//
//   (name, split_args)
//
// Where name is the base name of the function, and split_args is a tuple:
//
//   (required_count, required_args, optional_count, required_args)
//


// defines the actual function body for AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION below.
# define AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION0(z, n, data) \
    template < \
        class ResultType \
      , class Args \
        AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_ARGUMENTS( \
            AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION_TEMPLATE_ARG \
          , n \
          , AUTOBOOST_PP_TUPLE_ELEM(5,1,data) \
        ) \
    > \
    AUTOBOOST_PARAMETER_MEMBER_FUNCTION_STATIC(AUTOBOOST_PP_TUPLE_ELEM(5,0,data)) \
    ResultType AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_NAME(AUTOBOOST_PP_TUPLE_ELEM(5,0,data))( \
        ResultType(*)() \
      , Args const& args \
      , int \
        AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_ARGUMENTS( \
            AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION_ARG \
          , n \
          , AUTOBOOST_PP_TUPLE_ELEM(5,1,data) \
        ) \
    ) AUTOBOOST_PP_EXPR_IF(AUTOBOOST_PP_TUPLE_ELEM(5,2,data), const) \
    AUTOBOOST_PP_IF( \
        n \
      , AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION_BODY \
      , ; AUTOBOOST_PP_TUPLE_EAT(4) \
    )( \
        AUTOBOOST_PP_TUPLE_ELEM(5,0,data) \
      , n \
      , AUTOBOOST_PP_TUPLE_ELEM(5,1,data) \
      , AUTOBOOST_PP_TUPLE_ELEM(5,3,data) \
    )

# define AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION(z, n, data) \
    AUTOBOOST_PP_IF( \
        AUTOBOOST_PP_AND( \
            AUTOBOOST_PP_NOT(n) \
          , AUTOBOOST_PP_TUPLE_ELEM(5,4,data) \
        ) \
      , AUTOBOOST_PP_TUPLE_EAT(3) \
      , AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION0 \
    )(z, n, data) \
    AUTOBOOST_PP_IF( \
        AUTOBOOST_PP_EQUAL(n, AUTOBOOST_PP_TUPLE_ELEM(4,2,AUTOBOOST_PP_TUPLE_ELEM(5,1,data))) \
      , AUTOBOOST_PP_TUPLE_EAT(5) \
      , AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_EVAL_DEFAULT_BODY \
    )( \
        AUTOBOOST_PP_TUPLE_ELEM(5,0,data) \
      , n \
      , AUTOBOOST_PP_TUPLE_ELEM(5,1,data) \
      , AUTOBOOST_PP_TUPLE_ELEM(5,3,data) \
      , AUTOBOOST_PP_TUPLE_ELEM(5,2,data) \
    )

# define AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_GET_ARG(r, tag_ns, arg) \
    , AUTOBOOST_PARAMETER_FUNCTION_CAST( \
          args[ \
              autoboost::parameter::keyword<tag_ns::AUTOBOOST_PARAMETER_FN_ARG_KEYWORD(arg)>::instance \
          ] \
        , AUTOBOOST_PARAMETER_FN_ARG_PRED(arg) \
        , Args \
      )

// Generates the function template that recives a ArgumentPack, and then
// goes on to call the layers of overloads generated by
// AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_LAYER.
# define AUTOBOOST_PARAMETER_FUNCTION_INITIAL_DISPATCH_FUNCTION(name, split_args, const_, tag_ns) \
    template <class Args> \
    typename AUTOBOOST_PARAMETER_FUNCTION_RESULT_NAME(name)<Args>::type \
    AUTOBOOST_PARAMETER_MEMBER_FUNCTION_STATIC(name) \
    AUTOBOOST_PARAMETER_IMPL(name)(Args const& args) AUTOBOOST_PP_EXPR_IF(const_, const) \
    { \
        return AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_NAME(name)( \
            (typename AUTOBOOST_PARAMETER_FUNCTION_RESULT_NAME(name)<Args>::type(*)())0 \
          , args \
          , 0L \
 \
            AUTOBOOST_PP_SEQ_FOR_EACH( \
                AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_GET_ARG \
              , tag_ns \
              , AUTOBOOST_PP_TUPLE_ELEM(4,1,split_args) \
            ) \
 \
        ); \
    }

// Helper for AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_LAYER below.
# define AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_LAYER_AUX( \
    name, split_args, skip_fwd_decl, const_, tag_namespace \
  ) \
    AUTOBOOST_PP_REPEAT_FROM_TO( \
        0 \
      , AUTOBOOST_PP_INC(AUTOBOOST_PP_TUPLE_ELEM(4, 2, split_args)) \
      , AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION \
      , (name, split_args, const_, tag_namespace, skip_fwd_decl) \
    ) \
 \
    AUTOBOOST_PARAMETER_FUNCTION_INITIAL_DISPATCH_FUNCTION(name, split_args, const_, tag_namespace) \
\
    template < \
        class ResultType \
      , class Args \
        AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_ARGUMENTS( \
            AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION_TEMPLATE_ARG \
          , 0 \
          , split_args \
        ) \
    > \
    AUTOBOOST_PARAMETER_MEMBER_FUNCTION_STATIC(name) \
    ResultType AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_NAME(name)( \
        ResultType(*)() \
      , Args const& \
      , int \
        AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_ARGUMENTS( \
            AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_FUNCTION_ARG \
          , 0 \
          , split_args \
        ) \
    ) AUTOBOOST_PP_EXPR_IF(const_, const)

// Generates a bunch of forwarding functions that each extract
// one more argument.
# define AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_LAYER(name, args, skip_fwd_decl, const_, tag_ns) \
    AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_LAYER_AUX( \
        name, AUTOBOOST_PARAMETER_FUNCTION_SPLIT_ARGS(args), skip_fwd_decl, const_, tag_ns \
    )
/**/

// Defines the result metafunction and the parameters specialization.
# define AUTOBOOST_PARAMETER_FUNCTION_HEAD(result, name, tag_namespace, args)   \
      AUTOBOOST_PARAMETER_FUNCTION_RESULT_(result, name, args)                   \
                                                                            \
          AUTOBOOST_PARAMETER_FUNCTION_PARAMETERS(tag_namespace, name, args)    \
          AUTOBOOST_PARAMETER_FUNCTION_PARAMETERS_NAME(name);                   \

// Helper for AUTOBOOST_PARAMETER_FUNCTION below.
# define AUTOBOOST_PARAMETER_FUNCTION_AUX(result, name, tag_namespace, args)    \
    AUTOBOOST_PARAMETER_FUNCTION_HEAD(result, name, tag_namespace, args)         \
    AUTOBOOST_PARAMETER_FUNCTION_IMPL_HEAD(name); \
\
    AUTOBOOST_PARAMETER_FUNCTION_FWD_FUNCTIONS(                                  \
        result, name, args, 0                                                \
      , AUTOBOOST_PARAMETER_FUNCTION_FWD_COMBINATIONS(args)                      \
    )                                                                        \
                                                                             \
    AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_LAYER(name, args, 0, 0, tag_namespace)

// Defines a Boost.Parameter enabled function with the new syntax.
# define AUTOBOOST_PARAMETER_FUNCTION(result, name, tag_namespace, args)    \
    AUTOBOOST_PARAMETER_FUNCTION_AUX(                                       \
        result, name, tag_namespace                                      \
      , AUTOBOOST_PARAMETER_FLATTEN(3, 2, 3, args)                           \
    )                                                                    \
/**/

// Defines a Boost.Parameter enabled function.
# define AUTOBOOST_PARAMETER_BASIC_FUNCTION_AUX(result, name, tag_namespace, args)    \
    AUTOBOOST_PARAMETER_FUNCTION_HEAD(result, name, tag_namespace, args)        \
                                                                            \
    AUTOBOOST_PARAMETER_FUNCTION_IMPL_FWD(name)                                 \
                                                                            \
    AUTOBOOST_PARAMETER_FUNCTION_FWD_FUNCTIONS(                                 \
        result, name, args, 0                                               \
      , AUTOBOOST_PARAMETER_FUNCTION_FWD_COMBINATIONS(args)                     \
    )                                                                       \
                                                                            \
    AUTOBOOST_PARAMETER_FUNCTION_IMPL_HEAD(name)

# define AUTOBOOST_PARAMETER_BASIC_FUNCTION(result, name, tag_namespace, args)  \
    AUTOBOOST_PARAMETER_BASIC_FUNCTION_AUX(                                     \
        result, name, tag_namespace                                     \
      , AUTOBOOST_PARAMETER_FLATTEN(2, 2, 3, args)                          \
    )                                                                   \
/**/

// Defines a Boost.Parameter enabled member function.
# define AUTOBOOST_PARAMETER_BASIC_MEMBER_FUNCTION_AUX(result, name, tag_namespace, args, const_) \
    AUTOBOOST_PARAMETER_FUNCTION_HEAD(result, name, tag_namespace, args)                    \
                                                                                        \
    AUTOBOOST_PARAMETER_FUNCTION_FWD_FUNCTIONS(                                             \
        result, name, args, const_                                                      \
      , AUTOBOOST_PARAMETER_FUNCTION_FWD_COMBINATIONS(args)                                 \
    )                                                                                   \
                                                                                        \
    AUTOBOOST_PARAMETER_FUNCTION_IMPL_HEAD(name) AUTOBOOST_PP_EXPR_IF(const_, const)            \
/**/

# define AUTOBOOST_PARAMETER_BASIC_MEMBER_FUNCTION(result, name, tag_namespace, args) \
    AUTOBOOST_PARAMETER_BASIC_MEMBER_FUNCTION_AUX( \
        result, name, tag_namespace \
      , AUTOBOOST_PARAMETER_FLATTEN(2, 2, 3, args) \
      , 0 \
    )
/**/

# define AUTOBOOST_PARAMETER_BASIC_CONST_MEMBER_FUNCTION(result, name, tag_namespace, args) \
    AUTOBOOST_PARAMETER_BASIC_MEMBER_FUNCTION_AUX( \
        result, name, tag_namespace \
      , AUTOBOOST_PARAMETER_FLATTEN(2, 2, 3, args) \
      , 1 \
    )
/**/



# define AUTOBOOST_PARAMETER_MEMBER_FUNCTION_AUX(result, name, tag_namespace, const_, args)    \
    AUTOBOOST_PARAMETER_FUNCTION_HEAD(result, name, tag_namespace, args)         \
\
    AUTOBOOST_PARAMETER_FUNCTION_FWD_FUNCTIONS(                                  \
        result, name, args, const_                                           \
      , AUTOBOOST_PARAMETER_FUNCTION_FWD_COMBINATIONS(args)                      \
    )                                                                        \
                                                                             \
    AUTOBOOST_PARAMETER_FUNCTION_DEFAULT_LAYER(name, args, 1, const_, tag_namespace)

// Defines a Boost.Parameter enabled function with the new syntax.
# define AUTOBOOST_PARAMETER_MEMBER_FUNCTION(result, name, tag_namespace, args)    \
    AUTOBOOST_PARAMETER_MEMBER_FUNCTION_AUX(                                       \
        result, name, tag_namespace, 0                                     \
      , AUTOBOOST_PARAMETER_FLATTEN(3, 2, 3, args)                           \
    )                                                                    \
/**/

# define AUTOBOOST_PARAMETER_CONST_MEMBER_FUNCTION(result, name, tag_namespace, args)    \
    AUTOBOOST_PARAMETER_MEMBER_FUNCTION_AUX(                                       \
        result, name, tag_namespace, 1                                     \
      , AUTOBOOST_PARAMETER_FLATTEN(3, 2, 3, args)                           \
    )                                                                    \
/**/

// Defines a Boost.Parameter enabled constructor.

# define AUTOBOOST_PARAMETER_FUNCTION_ARGUMENT(r, _, i, elem) \
    AUTOBOOST_PP_COMMA_IF(i) elem& AUTOBOOST_PP_CAT(a, i)
/**/

# define AUTOBOOST_PARAMETER_FUNCTION_FWD_CONSTRUCTOR00(z, n, r, data, elem) \
    AUTOBOOST_PP_IF( \
        n \
      , AUTOBOOST_PARAMETER_FUNCTION_FWD_FUNCTION_TEMPLATE_Z, AUTOBOOST_PP_TUPLE_EAT(2) \
    )(z, n) \
    AUTOBOOST_PP_EXPR_IF(AUTOBOOST_PP_EQUAL(n,1), explicit) \
    AUTOBOOST_PP_TUPLE_ELEM(6,2,data)( \
        AUTOBOOST_PP_IF( \
            n \
          , AUTOBOOST_PP_SEQ_FOR_EACH_I_R \
          , AUTOBOOST_PP_TUPLE_EAT(4) \
        )( \
            r \
          , AUTOBOOST_PARAMETER_FUNCTION_ARGUMENT \
          , ~ \
          , elem \
        ) \
        AUTOBOOST_PP_IF(n, AUTOBOOST_PARAMETER_FUNCTION_FWD_MATCH_Z, AUTOBOOST_PP_TUPLE_EAT(4))( \
            z \
          , AUTOBOOST_PP_TUPLE_ELEM(6,3,data) \
          , AUTOBOOST_PP_CAT(constructor_parameters, __LINE__) \
          , n \
        ) \
    ) \
      : AUTOBOOST_PARAMETER_PARENTHESIZED_TYPE(AUTOBOOST_PP_TUPLE_ELEM(6,3,data)) ( \
            AUTOBOOST_PP_CAT(constructor_parameters, __LINE__)()( \
                AUTOBOOST_PP_ENUM_PARAMS_Z(z, n, a) \
            ) \
        ) \
    {}
/**/

# define AUTOBOOST_PARAMETER_FUNCTION_FWD_CONSTRUCTOR0(r, data, elem) \
    AUTOBOOST_PARAMETER_FUNCTION_FWD_CONSTRUCTOR00( \
        AUTOBOOST_PP_TUPLE_ELEM(6,0,data) \
      , AUTOBOOST_PP_TUPLE_ELEM(6,1,data) \
      , r \
      , data \
      , elem \
    )
/**/

# define AUTOBOOST_PARAMETER_FUNCTION_FWD_PRODUCT(r, product) \
    (product)
/**/

# define AUTOBOOST_PARAMETER_FUNCTION_FWD_CONSTRUCTOR_ARITY_0(z, n, data) \
    AUTOBOOST_PARAMETER_FUNCTION_FWD_CONSTRUCTOR00( \
        z, n, AUTOBOOST_PP_DEDUCE_R() \
      , (z, n, AUTOBOOST_PP_TUPLE_REM(4) data) \
      , ~ \
    )
/**/

# define AUTOBOOST_PARAMETER_FUNCTION_FWD_CONSTRUCTOR_ARITY_N(z, n, data) \
    AUTOBOOST_PP_SEQ_FOR_EACH( \
        AUTOBOOST_PARAMETER_FUNCTION_FWD_CONSTRUCTOR0 \
      , (z, n, AUTOBOOST_PP_TUPLE_REM(4) data) \
      , AUTOBOOST_PP_SEQ_FOR_EACH_PRODUCT( \
            AUTOBOOST_PARAMETER_FUNCTION_FWD_PRODUCT \
          , AUTOBOOST_PP_SEQ_FIRST_N( \
                n, AUTOBOOST_PP_TUPLE_ELEM(4,2,data) \
            ) \
        ) \
    )
/**/

# define AUTOBOOST_PARAMETER_FUNCTION_FWD_CONSTRUCTOR(z, n, data) \
    AUTOBOOST_PP_IF( \
        n \
      , AUTOBOOST_PARAMETER_FUNCTION_FWD_CONSTRUCTOR_ARITY_N \
      , AUTOBOOST_PARAMETER_FUNCTION_FWD_CONSTRUCTOR_ARITY_0 \
    )(z,n,data) \
/**/

# define AUTOBOOST_PARAMETER_FUNCTION_FWD_CONSTRUCTORS0(class_,base,args,combinations,range) \
    AUTOBOOST_PP_REPEAT_FROM_TO( \
        AUTOBOOST_PP_TUPLE_ELEM(2,0,range), AUTOBOOST_PP_TUPLE_ELEM(2,1,range) \
      , AUTOBOOST_PARAMETER_FUNCTION_FWD_CONSTRUCTOR \
      , (class_,base,combinations,AUTOBOOST_PP_TUPLE_ELEM(2,1,range)) \
    )
/**/

# define AUTOBOOST_PARAMETER_FUNCTION_FWD_CONSTRUCTORS(class_,base,args,combinations) \
    AUTOBOOST_PARAMETER_FUNCTION_FWD_CONSTRUCTORS0( \
        class_, base, args, combinations, AUTOBOOST_PARAMETER_ARITY_RANGE(args) \
    )
/**/

# define AUTOBOOST_PARAMETER_CONSTRUCTOR_AUX(class_, base, tag_namespace, args) \
    AUTOBOOST_PARAMETER_FUNCTION_PARAMETERS(tag_namespace, ctor, args)          \
        AUTOBOOST_PP_CAT(constructor_parameters, __LINE__); \
\
    AUTOBOOST_PARAMETER_FUNCTION_FWD_CONSTRUCTORS( \
        class_, base, args \
      , AUTOBOOST_PARAMETER_FUNCTION_FWD_COMBINATIONS(args) \
    ) \
/**/

# define AUTOBOOST_PARAMETER_CONSTRUCTOR(class_, base, tag_namespace, args) \
    AUTOBOOST_PARAMETER_CONSTRUCTOR_AUX( \
        class_, base, tag_namespace \
      , AUTOBOOST_PARAMETER_FLATTEN(2, 2, 3, args) \
    )
/**/

# ifndef AUTOBOOST_NO_FUNCTION_TEMPLATE_ORDERING
#  define AUTOBOOST_PARAMETER_FUNCTION_FWD_COMBINATION(r, _, i, elem) \
    (AUTOBOOST_PP_IF( \
        AUTOBOOST_PARAMETER_FUNCTION_IS_KEYWORD_QUALIFIER( \
            AUTOBOOST_PARAMETER_FN_ARG_NAME(elem) \
        ) \
      , (const ParameterArgumentType ## i)(ParameterArgumentType ## i) \
      , (const ParameterArgumentType ## i) \
    ))
// No partial ordering. This feature doesn't work.
# else
#  define AUTOBOOST_PARAMETER_FUNCTION_FWD_COMBINATION(r, _, i, elem) \
    (AUTOBOOST_PP_IF( \
        AUTOBOOST_PARAMETER_FUNCTION_IS_KEYWORD_QUALIFIER( \
            AUTOBOOST_PARAMETER_FN_ARG_NAME(elem) \
        ) \
      , (ParameterArgumentType ## i) \
      , (const ParameterArgumentType ## i) \
    ))
# endif

# define AUTOBOOST_PARAMETER_FUNCTION_FWD_COMBINATIONS(args) \
    AUTOBOOST_PP_SEQ_FOR_EACH_I(AUTOBOOST_PARAMETER_FUNCTION_FWD_COMBINATION, ~, args)

#endif // AUTOBOOST_PARAMETER_PREPROCESSOR_060206_HPP

