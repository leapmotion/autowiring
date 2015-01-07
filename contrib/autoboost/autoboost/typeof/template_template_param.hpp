// Copyright (C) 2005 Peder Holt
// Copyright (C) 2005 Arkadiy Vertleyb
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_TYPEOF_TEMPLATE_TEMPLATE_PARAM_HPP_INCLUDED
#define AUTOBOOST_TYPEOF_TEMPLATE_TEMPLATE_PARAM_HPP_INCLUDED

#include <autoboost/preprocessor/logical/or.hpp>
#include <autoboost/preprocessor/seq/fold_left.hpp>
#include <autoboost/preprocessor/seq/enum.hpp>

#define AUTOBOOST_TYPEOF_MAKE_OBJ_template(x)   AUTOBOOST_TYPEOF_TEMPLATE_PARAM(x)
#define AUTOBOOST_TYPEOF_TEMPLATE(X) template(X) AUTOBOOST_TYPEOF_EAT
#define AUTOBOOST_TYPEOF_template(X) (template(X))

#define AUTOBOOST_TYPEOF_TEMPLATE_PARAM(Params)\
    (TEMPLATE_PARAM)\
    (Params)

#define AUTOBOOST_TYPEOF_TEMPLATE_PARAM_GETPARAMS(This)\
    AUTOBOOST_TYPEOF_TOSEQ(AUTOBOOST_PP_SEQ_ELEM(1, This))

//Encode / decode this
#define AUTOBOOST_TYPEOF_TEMPLATE_PARAM_ENCODE(This, n)\
   typedef typename autoboost::type_of::encode_template<AUTOBOOST_PP_CAT(V, n),\
       AUTOBOOST_PP_CAT(P, n)<AUTOBOOST_TYPEOF_SEQ_ENUM(AUTOBOOST_TYPEOF_MAKE_OBJS(AUTOBOOST_TYPEOF_TEMPLATE_PARAM_GETPARAMS(This)),AUTOBOOST_TYPEOF_PLACEHOLDER) >\
   >::type AUTOBOOST_PP_CAT(V, AUTOBOOST_PP_INC(n));

#define AUTOBOOST_TYPEOF_TEMPLATE_PARAM_DECODE(This, n)\
   typedef autoboost::type_of::decode_template< AUTOBOOST_PP_CAT(iter, n) > AUTOBOOST_PP_CAT(d, n);\
   typedef typename AUTOBOOST_PP_CAT(d, n)::type AUTOBOOST_PP_CAT(P, n);\
   typedef typename AUTOBOOST_PP_CAT(d, n)::iter AUTOBOOST_PP_CAT(iter,AUTOBOOST_PP_INC(n));

// template<class, unsigned int, ...> class
#define AUTOBOOST_TYPEOF_TEMPLATE_PARAM_EXPANDTYPE(This) \
    template <AUTOBOOST_PP_SEQ_ENUM(AUTOBOOST_TYPEOF_TEMPLATE_PARAM_GETPARAMS(This)) > class

#define AUTOBOOST_TYPEOF_TEMPLATE_PARAM_PLACEHOLDER(Param)\
    Nested_Template_Template_Arguments_Not_Supported

//'template<class,int> class' is reduced to 'class'
#define AUTOBOOST_TYPEOF_TEMPLATE_PARAM_DECLARATION_TYPE(Param) class

// T3<int, (unsigned int)0, ...>
#define AUTOBOOST_TYPEOF_TEMPLATE_PARAM_PLACEHOLDER_TYPES(Param, n)\
    AUTOBOOST_PP_CAT(T,n)<AUTOBOOST_TYPEOF_SEQ_ENUM_1(AUTOBOOST_TYPEOF_MAKE_OBJS(AUTOBOOST_TYPEOF_TEMPLATE_PARAM_GETPARAMS(Param)),AUTOBOOST_TYPEOF_PLACEHOLDER) >

#define AUTOBOOST_TYPEOF_TEMPLATE_PARAM_ISTEMPLATE 1

////////////////////////////
// move to encode_decode?

AUTOBOOST_TYPEOF_BEGIN_ENCODE_NS

template<class V, class Type_Not_Registered_With_Typeof_System> struct encode_template_impl;
template<class T, class Iter> struct decode_template_impl;

AUTOBOOST_TYPEOF_END_ENCODE_NS

namespace autoboost { namespace type_of {

    template<class V, class T> struct encode_template
        : AUTOBOOST_TYPEOF_ENCODE_NS_QUALIFIER::encode_template_impl<V, T>
    {};

    template<class Iter> struct decode_template
        : AUTOBOOST_TYPEOF_ENCODE_NS_QUALIFIER::decode_template_impl<typename Iter::type, typename Iter::next>
    {};
}}

////////////////////////////
// move to template_encoding.hpp?

//Template template registration
#define AUTOBOOST_TYPEOF_REGISTER_TYPE_FOR_TEMPLATE_TEMPLATE(Name,Params,ID)\
    template<class V\
        AUTOBOOST_TYPEOF_SEQ_ENUM_TRAILING(Params,AUTOBOOST_TYPEOF_REGISTER_TEMPLATE_PARAM_PAIR)\
    >\
    struct encode_template_impl<V,Name<\
        AUTOBOOST_PP_ENUM_PARAMS(\
        AUTOBOOST_PP_SEQ_SIZE(Params),\
        P)> >\
        : autoboost::type_of::push_back<V, autoboost::mpl::size_t<ID> >\
    {\
    };\
    template<class Iter> struct decode_template_impl<autoboost::mpl::size_t<ID>, Iter>\
    {\
        AUTOBOOST_PP_REPEAT(AUTOBOOST_PP_SEQ_SIZE(Params),AUTOBOOST_TYPEOF_TYPEDEF_INT_PN,_)\
        typedef Name<AUTOBOOST_TYPEOF_SEQ_ENUM(Params,AUTOBOOST_TYPEOF_PLACEHOLDER) > type;\
        typedef Iter iter;\
    };

#define AUTOBOOST_TYPEOF_TYPEDEF_INT_PN(z,n,Params) typedef int AUTOBOOST_PP_CAT(P,n);

#ifdef __BORLANDC__
#define AUTOBOOST_TYPEOF_DECODE_NESTED_TEMPLATE_HELPER_NAME AUTOBOOST_PP_CAT(\
        AUTOBOOST_PP_CAT(\
            AUTOBOOST_PP_CAT(\
                decode_nested_template_helper,\
                AUTOBOOST_TYPEOF_REGISTRATION_GROUP\
            ),0x10000\
        ),__LINE__\
    )
#define AUTOBOOST_TYPEOF_REGISTER_DECODE_NESTED_TEMPLATE_HELPER_IMPL(Name,Params,ID)\
    struct AUTOBOOST_TYPEOF_DECODE_NESTED_TEMPLATE_HELPER_NAME {\
        template<AUTOBOOST_TYPEOF_SEQ_ENUM(Params,AUTOBOOST_TYPEOF_REGISTER_DECLARE_DECODER_TYPE_PARAM_PAIR) >\
        struct decode_params;\
        template<AUTOBOOST_TYPEOF_SEQ_ENUM(Params,AUTOBOOST_TYPEOF_REGISTER_DECODER_TYPE_PARAM_PAIR) >\
        struct decode_params<AUTOBOOST_TYPEOF_SEQ_ENUM(Params,AUTOBOOST_TYPEOF_PLACEHOLDER_TYPES) >\
        {\
            typedef Name<AUTOBOOST_PP_ENUM_PARAMS(AUTOBOOST_PP_SEQ_SIZE(Params),T)> type;\
        };\
    };
//Template template param decoding
#define AUTOBOOST_TYPEOF_TYPEDEF_DECODED_TEMPLATE_TEMPLATE_TYPE(Name,Params)\
    typedef typename AUTOBOOST_TYPEOF_DECODE_NESTED_TEMPLATE_HELPER_NAME::decode_params<AUTOBOOST_PP_ENUM_PARAMS(AUTOBOOST_PP_SEQ_SIZE(Params),P)>::type type;

#else
#define AUTOBOOST_TYPEOF_REGISTER_DECODE_NESTED_TEMPLATE_HELPER_IMPL(Name,Params,ID)

//Template template param decoding
#define AUTOBOOST_TYPEOF_TYPEDEF_DECODED_TEMPLATE_TEMPLATE_TYPE(Name,Params)\
    template<AUTOBOOST_TYPEOF_SEQ_ENUM(Params,AUTOBOOST_TYPEOF_REGISTER_DECLARE_DECODER_TYPE_PARAM_PAIR) >\
    struct decode_params;\
    template<AUTOBOOST_TYPEOF_SEQ_ENUM(Params,AUTOBOOST_TYPEOF_REGISTER_DECODER_TYPE_PARAM_PAIR) >\
    struct decode_params<AUTOBOOST_TYPEOF_SEQ_ENUM(Params,AUTOBOOST_TYPEOF_PLACEHOLDER_TYPES) >\
    {\
        typedef Name<AUTOBOOST_PP_ENUM_PARAMS(AUTOBOOST_PP_SEQ_SIZE(Params),T)> type;\
    };\
    typedef typename decode_params<AUTOBOOST_PP_ENUM_PARAMS(AUTOBOOST_PP_SEQ_SIZE(Params),P)>::type type;
#endif
#define AUTOBOOST_TYPEOF_REGISTER_DECLARE_DECODER_TYPE_PARAM_PAIR(z,n,elem) \
    AUTOBOOST_TYPEOF_VIRTUAL(DECLARATION_TYPE, elem)(elem) AUTOBOOST_PP_CAT(T, n)

// AUTOBOOST_TYPEOF_HAS_TEMPLATES
#define AUTOBOOST_TYPEOF_HAS_TEMPLATES(Params)\
    AUTOBOOST_PP_SEQ_FOLD_LEFT(AUTOBOOST_TYPEOF_HAS_TEMPLATES_OP, 0, Params)

#define AUTOBOOST_TYPEOF_HAS_TEMPLATES_OP(s, state, elem)\
    AUTOBOOST_PP_OR(state, AUTOBOOST_TYPEOF_VIRTUAL(ISTEMPLATE, elem))

//Define template template arguments
#define AUTOBOOST_TYPEOF_REGISTER_TEMPLATE_TEMPLATE_IMPL(Name,Params,ID)\
    AUTOBOOST_PP_IF(AUTOBOOST_TYPEOF_HAS_TEMPLATES(Params),\
        AUTOBOOST_TYPEOF_REGISTER_DECODE_NESTED_TEMPLATE_HELPER_IMPL,\
        AUTOBOOST_TYPEOF_REGISTER_TYPE_FOR_TEMPLATE_TEMPLATE)(Name,Params,ID)

#endif //AUTOBOOST_TYPEOF_TEMPLATE_TEMPLATE_PARAM_HPP_INCLUDED
