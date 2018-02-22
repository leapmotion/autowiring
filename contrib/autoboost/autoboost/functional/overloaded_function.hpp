
// Copyright (C) 2009-2012 Lorenzo Caminiti
// Distributed under the Boost Software License, Version 1.0
// (see accompanying file LICENSE_1_0.txt or a copy at
// http://www.boost.org/LICENSE_1_0.txt)
// Home at http://www.boost.org/libs/functional/overloaded_function

#ifndef DOXYGEN // Doxygen documentation only.

#if !AUTOBOOST_PP_IS_ITERATING
#   ifndef AUTOBOOST_FUNCTIONAL_OVERLOADED_FUNCTION_HPP_
#       define AUTOBOOST_FUNCTIONAL_OVERLOADED_FUNCTION_HPP_

#       include <autoboost/functional/overloaded_function/detail/base.hpp>
#       include <autoboost/functional/overloaded_function/detail/function_type.hpp>
#       include <autoboost/functional/overloaded_function/config.hpp>
#       include <autoboost/typeof/typeof.hpp>
#       include <autoboost/preprocessor/iteration/iterate.hpp>
#       include <autoboost/preprocessor/repetition/enum.hpp>
#       include <autoboost/preprocessor/repetition/repeat.hpp>
#       include <autoboost/preprocessor/control/expr_iif.hpp>
#       include <autoboost/preprocessor/control/expr_if.hpp>
#       include <autoboost/preprocessor/comparison/greater.hpp>
#       include <autoboost/preprocessor/comparison/less.hpp>
#       include <autoboost/preprocessor/cat.hpp>
#       include <autoboost/preprocessor/arithmetic/add.hpp>
#       include <autoboost/preprocessor/arithmetic/sub.hpp>
#       include <autoboost/preprocessor/tuple/eat.hpp>
#       include <autoboost/preprocessor/logical/and.hpp>
#       include <autoboost/preprocessor/logical/not.hpp>
#       include <autoboost/preprocessor/facilities/expand.hpp>

#define AUTOBOOST_FUNCTIONAL_f_type(z, n, unused) \
    AUTOBOOST_PP_CAT(F, n)

#define AUTOBOOST_FUNCTIONAL_f_arg(z, n, unused) \
    AUTOBOOST_PP_CAT(f, n)

#define AUTOBOOST_FUNCTIONAL_f_tparam(z, n, unused) \
    typename AUTOBOOST_FUNCTIONAL_f_type(z, n, ~) \

#define AUTOBOOST_FUNCTIONAL_f_tparam_dflt(z, n, is_tspec) \
    AUTOBOOST_FUNCTIONAL_f_tparam(z, n, ~) \
    /* overload requires at least 2 functors so F0 and F1 not optional */ \
    AUTOBOOST_PP_EXPR_IIF(AUTOBOOST_PP_AND(AUTOBOOST_PP_NOT(is_tspec), \
            AUTOBOOST_PP_GREATER(n, 1)), \
        = void \
    )

#define AUTOBOOST_FUNCTIONAL_f_arg_decl(z, n, unused) \
    AUTOBOOST_FUNCTIONAL_f_type(z, n, ~) /* no qualifier to deduce tparam */ \
    AUTOBOOST_FUNCTIONAL_f_arg(z, n, ~)

#define AUTOBOOST_FUNCTIONAL_g_type(z, n, unused) \
    AUTOBOOST_PP_CAT(G, n)

#define AUTOBOOST_FUNCTIONAL_g_arg(z, n, unused) \
    AUTOBOOST_PP_CAT(g, n)

#define AUTOBOOST_FUNCTIONAL_g_tparam(z, n, unused) \
    typename AUTOBOOST_FUNCTIONAL_g_type(z, n, ~)

#define AUTOBOOST_FUNCTIONAL_g_arg_decl(z, n, unused) \
    AUTOBOOST_FUNCTIONAL_g_type(z, n, ~) /* no qualifier to deduce tparam */ \
    AUTOBOOST_FUNCTIONAL_g_arg(z, n, ~)

#define AUTOBOOST_FUNCTIONAL_base(z, n, unused) \
    ::autoboost::overloaded_function_detail::base< \
        AUTOBOOST_FUNCTIONAL_f_type(z, n, ~) \
    >

#define AUTOBOOST_FUNCTIONAL_inherit(z, n, unused) \
    public AUTOBOOST_FUNCTIONAL_base(z, n, ~)

#define AUTOBOOST_FUNCTIONAL_base_init(z, n, unused) \
    AUTOBOOST_FUNCTIONAL_base(z, n, ~)(AUTOBOOST_FUNCTIONAL_g_arg(z, n, ~))

#define AUTOBOOST_FUNCTIONAL_using_operator_call(z, n, unused) \
    using AUTOBOOST_FUNCTIONAL_base(z, n, ~)::operator();

#define AUTOBOOST_FUNCTIONAL_function_type(z, n, unused) \
    typename ::autoboost::overloaded_function_detail::function_type< \
        AUTOBOOST_FUNCTIONAL_f_type(z, n, ~) \
    >::type

#       define AUTOBOOST_PP_ITERATION_PARAMS_1 \
            /* at least 2 func to overload so start from 2 to MAX */ \
            /* (cannot iterate [0, MAX-2) because error on Sun) */ \
            (3, (2, AUTOBOOST_FUNCTIONAL_OVERLOADED_FUNCTION_CONFIG_OVERLOAD_MAX, \
            "autoboost/functional/overloaded_function.hpp"))
#       include AUTOBOOST_PP_ITERATE() // Iterate over function arity.

#undef AUTOBOOST_FUNCTIONAL_f_type
#undef AUTOBOOST_FUNCTIONAL_f_arg
#undef AUTOBOOST_FUNCTIONAL_f_tparam
#undef AUTOBOOST_FUNCTIONAL_f_arg_decl
#undef AUTOBOOST_FUNCTIONAL_f_tparam_dflt
#undef AUTOBOOST_FUNCTIONAL_g_type
#undef AUTOBOOST_FUNCTIONAL_g_arg
#undef AUTOBOOST_FUNCTIONAL_g_tparam
#undef AUTOBOOST_FUNCTIONAL_g_arg_decl
#undef AUTOBOOST_FUNCTIONAL_base
#undef AUTOBOOST_FUNCTIONAL_inherit
#undef AUTOBOOST_FUNCTIONAL_base_init
#undef AUTOBOOST_FUNCTIONAL_using_operator_call
#undef AUTOBOOST_FUNCTIONAL_function_type

#   endif // #include guard

#elif AUTOBOOST_PP_ITERATION_DEPTH() == 1
#   define AUTOBOOST_FUNCTIONAL_overloads \
        /* iterate as OVERLOADS, OVERLOADS-1, OVERLOADS-2, ... */ \
        /* (add 2 because iteration started from 2 to MAX) */ \
        AUTOBOOST_PP_ADD(2, AUTOBOOST_PP_SUB( \
                AUTOBOOST_FUNCTIONAL_OVERLOADED_FUNCTION_CONFIG_OVERLOAD_MAX, \
                AUTOBOOST_PP_FRAME_ITERATION(1)))
#   define AUTOBOOST_FUNCTIONAL_is_tspec \
        /* if template specialization */ \
        AUTOBOOST_PP_LESS(AUTOBOOST_FUNCTIONAL_overloads, \
                AUTOBOOST_FUNCTIONAL_OVERLOADED_FUNCTION_CONFIG_OVERLOAD_MAX)

// For type-of emulation: This must be included at this pp iteration level.
#   include AUTOBOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()

namespace autoboost {

template<
    AUTOBOOST_PP_ENUM(AUTOBOOST_FUNCTIONAL_overloads, AUTOBOOST_FUNCTIONAL_f_tparam_dflt,
            AUTOBOOST_FUNCTIONAL_is_tspec)
>
class overloaded_function
    // Template specialization.
    AUTOBOOST_PP_EXPR_IIF(AUTOBOOST_PP_EXPAND(AUTOBOOST_FUNCTIONAL_is_tspec), <)
    AUTOBOOST_PP_IIF(AUTOBOOST_FUNCTIONAL_is_tspec,
        AUTOBOOST_PP_ENUM
    ,
        AUTOBOOST_PP_TUPLE_EAT(3)
    )(AUTOBOOST_FUNCTIONAL_overloads, AUTOBOOST_FUNCTIONAL_f_type, ~)
    AUTOBOOST_PP_EXPR_IIF(AUTOBOOST_PP_EXPAND(AUTOBOOST_FUNCTIONAL_is_tspec), >)
    // Bases (overloads >= 2 so always at least 2 bases).
    : AUTOBOOST_PP_ENUM(AUTOBOOST_FUNCTIONAL_overloads,
            AUTOBOOST_FUNCTIONAL_inherit, ~)
{
public:
    template<
        AUTOBOOST_PP_ENUM(AUTOBOOST_FUNCTIONAL_overloads, AUTOBOOST_FUNCTIONAL_g_tparam, ~)
    > /* implicit */ inline overloaded_function(
            AUTOBOOST_PP_ENUM(AUTOBOOST_FUNCTIONAL_overloads,
                    AUTOBOOST_FUNCTIONAL_g_arg_decl, ~))
            // Overloads >= 2 so always at least 2 bases to initialize.
            : AUTOBOOST_PP_ENUM(AUTOBOOST_FUNCTIONAL_overloads,
                    AUTOBOOST_FUNCTIONAL_base_init, ~)
    {}

    AUTOBOOST_PP_REPEAT(AUTOBOOST_FUNCTIONAL_overloads,
            AUTOBOOST_FUNCTIONAL_using_operator_call, ~)
};

template<
    AUTOBOOST_PP_ENUM(AUTOBOOST_FUNCTIONAL_overloads, AUTOBOOST_FUNCTIONAL_f_tparam, ~)
>
overloaded_function<
    AUTOBOOST_PP_ENUM(AUTOBOOST_FUNCTIONAL_overloads, AUTOBOOST_FUNCTIONAL_function_type, ~)
> make_overloaded_function(
    AUTOBOOST_PP_ENUM(AUTOBOOST_FUNCTIONAL_overloads, AUTOBOOST_FUNCTIONAL_f_arg_decl, ~)
) {
    return overloaded_function<
        AUTOBOOST_PP_ENUM(AUTOBOOST_FUNCTIONAL_overloads,
                AUTOBOOST_FUNCTIONAL_function_type, ~)
    >(AUTOBOOST_PP_ENUM(AUTOBOOST_FUNCTIONAL_overloads, AUTOBOOST_FUNCTIONAL_f_arg, ~));
}

} // namespace

// For type-of emulation: Register overloaded function type (for _AUTO, etc).
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(autoboost::overloaded_function,
    AUTOBOOST_FUNCTIONAL_overloads)

#   undef AUTOBOOST_FUNCTIONAL_overloads
#   undef AUTOBOOST_FUNCTIONAL_is_tspec
#endif // iteration

// DOCUMENTATION //

#else // DOXYGEN

/** @file
@brief Overload distinct function pointers, function references, and
monomorphic function objects into a single function object.
*/

namespace autoboost {

/**
@brief Function object to overload functions with distinct signatures.

This function object aggregates together calls to functions of all the
specified function types <c>F1</c>, <c>F2</c>, etc which must have distinct
function signatures from one another.

@Params
@Param{F<em>i</em>,
Each function type must be specified using the following syntax (which is
Boost.Function's preferred syntax):
@code
    result_type (argument1_type\, argumgnet2_type\, ...)
@endcode
}
@EndParams

In some cases, the @RefFunc{make_overloaded_function} function template can be
useful to construct an overloaded function object without explicitly
specifying the function types.

At least two distinct function types must be specified (because there is
nothing to overload between one or zero functions).
The maximum number of functions to overload is given by the
@RefMacro{AUTOBOOST_FUNCTIONAL_OVERLOADED_FUNCTION_CONFIG_OVERLOAD_MAX}
configuration macro.
The maximum number of function parameters for each of the specified function
types is given by the
@RefMacro{AUTOBOOST_FUNCTIONAL_OVERLOADED_FUNCTION_CONFIG_ARITY_MAX}
configuration macro.

@See @RefSect{tutorial, Tutorial} section, @RefFunc{make_overloaded_function},
@RefMacro{AUTOBOOST_FUNCTIONAL_OVERLOADED_FUNCTION_CONFIG_OVERLOAD_MAX},
@RefMacro{AUTOBOOST_FUNCTIONAL_OVERLOADED_FUNCTION_CONFIG_ARITY_MAX},
Boost.Function.
*/
template<typename F1, typename F2, ...>
class overloaded_function {
public:
    /**
    @brief Construct the overloaded function object.

    Any function pointer, function reference, and monomorphic function object
    that can be converted to a <c>autoboost::function</c> function object can be
    specified as parameter.

    @Note Unfortunately, it is not possible to support polymorphic function
    objects (as explained <a
    href="http://lists.boost.org/Archives/autoboost/2012/03/191744.php">here</a>).
    */
    overloaded_function(const autoboost::function<F1>&,
            const autoboost::function<F2>&, ...);

    /**
    @brief Call operator matching the signature of the function type specified
    as 1st template parameter.

    This will in turn invoke the call operator of the 1st function passed to
    the constructor.
    */
    typename autoboost::function_traits<F1>::result_type operator()(
            typename autoboost::function_traits<F1>::arg1_type,
            typename autoboost::function_traits<F1>::arg2_type,
            ...) const;

    /**
    @brief Call operator matching the signature of the function type specified
    as 2nd template parameter.

    This will in turn invoke the call operator of the 2nd function passed to
    the constructor.

    @Note Similar call operators are present for all specified function types
    <c>F1</c>, <c>F2</c>, etc (even if not exhaustively listed by this
    documentation).
    */
    typename autoboost::function_traits<F2>::result_type operator()(
            typename autoboost::function_traits<F2>::arg1_type,
            typename autoboost::function_traits<F2>::arg2_type,
            ...) const;
};

/**
@brief Make an overloaded function object without explicitly specifying the
function types.

This function template creates and returns an @RefClass{overloaded_function}
object that overloads all the specified functions <c>f1</c>, <c>f2</c>, etc.

The function types are internally determined from the template parameter types
so they do not need to be explicitly specified.
Therefore, this function template usually has a more concise syntax when
compared with @RefClass{overloaded_function}.
This is especially useful when the explicit type of the returned
@RefClass{overloaded_function} object does not need to be known (e.g., when
used with Boost.Typeof's <c>AUTOBOOST_AUTO</c>, C++11 <c>auto</c>, or when the
overloaded function object is handled using a function template parameter, see
the @RefSect{tutorial, Tutorial} section).

The maximum number of functions to overload is given by the
@RefMacro{AUTOBOOST_FUNCTIONAL_OVERLOADED_FUNCTION_CONFIG_OVERLOAD_MAX}
configuration macro.

@Note In this documentation, <c>__function_type__</c> is a placeholder for a
symbol that is specific to the implementation of this library.

@See @RefSect{tutorial, Tutorial} section, @RefClass{overloaded_function},
@RefMacro{AUTOBOOST_FUNCTIONAL_OVERLOADED_FUNCTION_CONFIG_OVERLOAD_MAX}.
*/
template<typename F1, typename F2, ...>
overloaded_function<
    __function_type__<F1>, __function_type__<F2>, ...
> make_overloaded_function(F1 f1, F2 f2, ...);

} // namespace

#endif // DOXYGEN

