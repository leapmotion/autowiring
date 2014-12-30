// Copyright (C) 2004 Arkadiy Vertleyb
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <autoboost/typeof/encode_decode_params.hpp>

#define n AUTOBOOST_PP_ITERATION()

// function pointers

template<class V, class R AUTOBOOST_PP_ENUM_TRAILING_PARAMS(n, class P)>
struct encode_type_impl<V, R(*)(AUTOBOOST_PP_ENUM_PARAMS(n, P))>
{
    typedef R AUTOBOOST_PP_CAT(P, n);
    typedef AUTOBOOST_TYPEOF_ENCODE_PARAMS(AUTOBOOST_PP_INC(n), FUN_PTR_ID + n) type;
};

template<class V, class R AUTOBOOST_PP_ENUM_TRAILING_PARAMS(n, class P)>
struct encode_type_impl<V, R(*)(AUTOBOOST_PP_ENUM_PARAMS(n, P) ...)>
{
    typedef R AUTOBOOST_PP_CAT(P, n);
    typedef AUTOBOOST_TYPEOF_ENCODE_PARAMS(AUTOBOOST_PP_INC(n), FUN_VAR_PTR_ID + n) type;
};

template<class Iter>
struct decode_type_impl<autoboost::mpl::size_t<FUN_PTR_ID + n>, Iter>
{
    typedef Iter iter0;
    AUTOBOOST_TYPEOF_DECODE_PARAMS(AUTOBOOST_PP_INC(n))
    typedef AUTOBOOST_PP_CAT(p, n)(*type)(AUTOBOOST_PP_ENUM_PARAMS(n, p));
    typedef AUTOBOOST_PP_CAT(iter, AUTOBOOST_PP_INC(n)) iter;
};

template<class Iter>
struct decode_type_impl<autoboost::mpl::size_t<FUN_VAR_PTR_ID + n>, Iter>
{
    typedef Iter iter0;
    AUTOBOOST_TYPEOF_DECODE_PARAMS(AUTOBOOST_PP_INC(n))
    typedef AUTOBOOST_PP_CAT(p, n)(*type)(AUTOBOOST_PP_ENUM_PARAMS(n, p) ...);
    typedef AUTOBOOST_PP_CAT(iter, AUTOBOOST_PP_INC(n)) iter;
};

#ifndef AUTOBOOST_TYPEOF_NO_FUNCTION_TYPES

    // function references

    template<class V, class R AUTOBOOST_PP_ENUM_TRAILING_PARAMS(n, class P)>
    struct encode_type_impl<V, R(&)(AUTOBOOST_PP_ENUM_PARAMS(n, P))>
    {
        typedef R AUTOBOOST_PP_CAT(P, n);
        typedef AUTOBOOST_TYPEOF_ENCODE_PARAMS(AUTOBOOST_PP_INC(n), FUN_REF_ID + n) type;
    };

    template<class V, class R AUTOBOOST_PP_ENUM_TRAILING_PARAMS(n, class P)>
    struct encode_type_impl<V, R(&)(AUTOBOOST_PP_ENUM_PARAMS(n, P) ...)>
    {
        typedef R AUTOBOOST_PP_CAT(P, n);
        typedef AUTOBOOST_TYPEOF_ENCODE_PARAMS(AUTOBOOST_PP_INC(n), FUN_VAR_REF_ID + n) type;
    };

    template<class Iter>
    struct decode_type_impl<autoboost::mpl::size_t<FUN_REF_ID + n>, Iter>
    {
        typedef Iter iter0;
        AUTOBOOST_TYPEOF_DECODE_PARAMS(AUTOBOOST_PP_INC(n))
        typedef AUTOBOOST_PP_CAT(p, n)(&type)(AUTOBOOST_PP_ENUM_PARAMS(n, p));
        typedef AUTOBOOST_PP_CAT(iter, AUTOBOOST_PP_INC(n)) iter;
    };

    template<class Iter>
    struct decode_type_impl<autoboost::mpl::size_t<FUN_VAR_REF_ID + n>, Iter>
    {
        typedef Iter iter0;
        AUTOBOOST_TYPEOF_DECODE_PARAMS(AUTOBOOST_PP_INC(n))
        typedef AUTOBOOST_PP_CAT(p, n)(&type)(AUTOBOOST_PP_ENUM_PARAMS(n, p) ...);
        typedef AUTOBOOST_PP_CAT(iter, AUTOBOOST_PP_INC(n)) iter;
    };

    // functions

    template<class V, class R AUTOBOOST_PP_ENUM_TRAILING_PARAMS(n, class P)>
    struct encode_type_impl<V, R(AUTOBOOST_PP_ENUM_PARAMS(n, P))>
    {
        typedef R AUTOBOOST_PP_CAT(P, n);
        typedef AUTOBOOST_TYPEOF_ENCODE_PARAMS(AUTOBOOST_PP_INC(n), FUN_ID + n) type;
    };

    template<class V, class R AUTOBOOST_PP_ENUM_TRAILING_PARAMS(n, class P)>
    struct encode_type_impl<V, R(AUTOBOOST_PP_ENUM_PARAMS(n, P) ...)>
    {
        typedef R AUTOBOOST_PP_CAT(P, n);
        typedef AUTOBOOST_TYPEOF_ENCODE_PARAMS(AUTOBOOST_PP_INC(n), FUN_VAR_ID + n) type;
    };

    template<class Iter>
    struct decode_type_impl<autoboost::mpl::size_t<FUN_ID + n>, Iter>
    {
        typedef Iter iter0;
        AUTOBOOST_TYPEOF_DECODE_PARAMS(AUTOBOOST_PP_INC(n))
        typedef AUTOBOOST_PP_CAT(p, n)(type)(AUTOBOOST_PP_ENUM_PARAMS(n, p));
        typedef AUTOBOOST_PP_CAT(iter, AUTOBOOST_PP_INC(n)) iter;
    };

    template<class Iter>
    struct decode_type_impl<autoboost::mpl::size_t<FUN_VAR_ID + n>, Iter>
    {
        typedef Iter iter0;
        AUTOBOOST_TYPEOF_DECODE_PARAMS(AUTOBOOST_PP_INC(n))
        typedef AUTOBOOST_PP_CAT(p, n)(type)(AUTOBOOST_PP_ENUM_PARAMS(n, p) ...);
        typedef AUTOBOOST_PP_CAT(iter, AUTOBOOST_PP_INC(n)) iter;
    };

#endif//AUTOBOOST_TYPEOF_NO_FUNCTION_TYPES

#ifndef AUTOBOOST_TYPEOF_NO_MEMBER_FUNCTION_TYPES
// member functions

#define AUTOBOOST_TYPEOF_qualifier
#define AUTOBOOST_TYPEOF_id MEM_FUN_ID
#include <autoboost/typeof/register_mem_functions.hpp>

#define AUTOBOOST_TYPEOF_qualifier const
#define AUTOBOOST_TYPEOF_id CONST_MEM_FUN_ID
#include <autoboost/typeof/register_mem_functions.hpp>

#define AUTOBOOST_TYPEOF_qualifier volatile
#define AUTOBOOST_TYPEOF_id VOLATILE_MEM_FUN_ID
#include <autoboost/typeof/register_mem_functions.hpp>

#define AUTOBOOST_TYPEOF_qualifier volatile const
#define AUTOBOOST_TYPEOF_id VOLATILE_CONST_MEM_FUN_ID
#include <autoboost/typeof/register_mem_functions.hpp>

#undef n
#endif
