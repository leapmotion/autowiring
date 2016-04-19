// Copyright (C) 2005 Arkadiy Vertleyb
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_TYPEOF_TYPE_TEMPLATE_PARAM_HPP_INCLUDED
#define AUTOBOOST_TYPEOF_TYPE_TEMPLATE_PARAM_HPP_INCLUDED

#define AUTOBOOST_TYPEOF_class_AUTOBOOST_TYPEOF (class)
#define AUTOBOOST_TYPEOF_typename_AUTOBOOST_TYPEOF (typename)

#define AUTOBOOST_TYPEOF_MAKE_OBJ_class AUTOBOOST_TYPEOF_TYPE_PARAM
#define AUTOBOOST_TYPEOF_MAKE_OBJ_typename AUTOBOOST_TYPEOF_TYPE_PARAM

#define AUTOBOOST_TYPEOF_TYPE_PARAM\
    (TYPE_PARAM)

#define AUTOBOOST_TYPEOF_TYPE_PARAM_EXPANDTYPE(Param) class

// TYPE_PARAM "virtual functions" implementation

#define AUTOBOOST_TYPEOF_TYPE_PARAM_ENCODE(This, n)\
    typedef typename autoboost::type_of::encode_type<\
        AUTOBOOST_PP_CAT(V, n),\
        AUTOBOOST_PP_CAT(P, n)\
    >::type AUTOBOOST_PP_CAT(V, AUTOBOOST_PP_INC(n));

#define AUTOBOOST_TYPEOF_TYPE_PARAM_DECODE(This, n)\
    typedef autoboost::type_of::decode_type< AUTOBOOST_PP_CAT(iter, n) > AUTOBOOST_PP_CAT(d, n);\
    typedef typename AUTOBOOST_PP_CAT(d, n)::type AUTOBOOST_PP_CAT(P, n);\
    typedef typename AUTOBOOST_PP_CAT(d, n)::iter AUTOBOOST_PP_CAT(iter, AUTOBOOST_PP_INC(n));

#define AUTOBOOST_TYPEOF_TYPE_PARAM_PLACEHOLDER(Param) int
#define AUTOBOOST_TYPEOF_TYPE_PARAM_DECLARATION_TYPE(Param) class
#define AUTOBOOST_TYPEOF_TYPE_PARAM_PLACEHOLDER_TYPES(Param, n) AUTOBOOST_PP_CAT(T,n)
#define AUTOBOOST_TYPEOF_TYPE_PARAM_ISTEMPLATE 0

#endif//AUTOBOOST_TYPEOF_TYPE_TEMPLATE_PARAM_HPP_INCLUDED
