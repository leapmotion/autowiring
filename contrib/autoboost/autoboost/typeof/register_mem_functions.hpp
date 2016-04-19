// Copyright (C) 2004 Arkadiy Vertleyb
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (http://www.boost.org/LICENSE_1_0.txt)

#include <autoboost/typeof/encode_decode_params.hpp>

// member functions

template<class V, class T, class R AUTOBOOST_PP_ENUM_TRAILING_PARAMS(n, class P)>
struct encode_type_impl<V, R(T::*)(AUTOBOOST_PP_ENUM_PARAMS(n, P)) AUTOBOOST_TYPEOF_qualifier>
{
    typedef R AUTOBOOST_PP_CAT(P, n);
    typedef T AUTOBOOST_PP_CAT(P, AUTOBOOST_PP_INC(n));
    typedef AUTOBOOST_TYPEOF_ENCODE_PARAMS(AUTOBOOST_PP_ADD(n, 2), AUTOBOOST_TYPEOF_id + n) type;
};

template<class Iter>
struct decode_type_impl<autoboost::mpl::size_t<AUTOBOOST_TYPEOF_id + n>, Iter>
{
    typedef Iter iter0;
    AUTOBOOST_TYPEOF_DECODE_PARAMS(AUTOBOOST_PP_ADD(n, 2))
    template<class T> struct workaround{
        typedef AUTOBOOST_PP_CAT(p, n)(T::*type)(AUTOBOOST_PP_ENUM_PARAMS(n, p)) AUTOBOOST_TYPEOF_qualifier;
    };
    typedef typename workaround<AUTOBOOST_PP_CAT(p, AUTOBOOST_PP_INC(n))>::type type;
    typedef AUTOBOOST_PP_CAT(iter, AUTOBOOST_PP_ADD(n, 2)) iter;
};

// undef parameters

#undef AUTOBOOST_TYPEOF_id
#undef AUTOBOOST_TYPEOF_qualifier
