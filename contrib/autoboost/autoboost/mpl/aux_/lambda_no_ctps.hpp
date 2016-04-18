
#if !defined(AUTOBOOST_PP_IS_ITERATING)

///// header body

#ifndef AUTOBOOST_MPL_AUX_LAMBDA_NO_CTPS_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_LAMBDA_NO_CTPS_HPP_INCLUDED

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

#if !defined(AUTOBOOST_MPL_PREPROCESSING_MODE)
#   include <autoboost/mpl/lambda_fwd.hpp>
#   include <autoboost/mpl/bind_fwd.hpp>
#   include <autoboost/mpl/protect.hpp>
#   include <autoboost/mpl/is_placeholder.hpp>
#   include <autoboost/mpl/if.hpp>
#   include <autoboost/mpl/identity.hpp>
#   include <autoboost/mpl/bool.hpp>
#   include <autoboost/mpl/aux_/na_spec.hpp>
#   include <autoboost/mpl/aux_/lambda_support.hpp>
#   include <autoboost/mpl/aux_/template_arity.hpp>
#   include <autoboost/mpl/aux_/value_wknd.hpp>
#endif

#include <autoboost/mpl/aux_/config/use_preprocessed.hpp>

#if    !defined(AUTOBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS) \
    && !defined(AUTOBOOST_MPL_PREPROCESSING_MODE)

#   define AUTOBOOST_MPL_PREPROCESSED_HEADER lambda_no_ctps.hpp
#   include <autoboost/mpl/aux_/include_preprocessed.hpp>

#else

#   include <autoboost/mpl/limits/arity.hpp>
#   include <autoboost/mpl/aux_/preprocessor/params.hpp>
#   include <autoboost/mpl/aux_/preprocessor/default_params.hpp>
#   include <autoboost/mpl/aux_/preprocessor/repeat.hpp>
#   include <autoboost/mpl/aux_/preprocessor/enum.hpp>
#   include <autoboost/mpl/aux_/config/msvc.hpp>
#   include <autoboost/mpl/aux_/config/workaround.hpp>

#   include <autoboost/preprocessor/comma_if.hpp>
#   include <autoboost/preprocessor/iterate.hpp>
#   include <autoboost/preprocessor/inc.hpp>
#   include <autoboost/preprocessor/cat.hpp>

namespace autoboost { namespace mpl {

#   define AUX778076_LAMBDA_PARAMS(i_, param) \
    AUTOBOOST_MPL_PP_PARAMS(i_, param) \
    /**/

namespace aux {

#define n_ AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY
template<
      AUTOBOOST_MPL_PP_DEFAULT_PARAMS(n_,bool C,false)
    >
struct lambda_or
    : true_
{
};

template<>
struct lambda_or< AUTOBOOST_MPL_PP_ENUM(n_,false) >
    : false_
{
};
#undef n_

template< typename Arity > struct lambda_impl
{
    template< typename T, typename Tag, typename Protect > struct result_
    {
        typedef T type;
        typedef is_placeholder<T> is_le;
    };
};

#define AUTOBOOST_PP_ITERATION_PARAMS_1 \
    (3,(1, AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY, <autoboost/mpl/aux_/lambda_no_ctps.hpp>))
#include AUTOBOOST_PP_ITERATE()

} // namespace aux

template<
      typename T
    , typename Tag
    , typename Protect
    >
struct lambda
{
    /// Metafunction forwarding confuses MSVC 6.x
    typedef typename aux::template_arity<T>::type arity_;
    typedef typename aux::lambda_impl<arity_>
        ::template result_< T,Tag,Protect > l_;

    typedef typename l_::type type;
    typedef typename l_::is_le is_le;

    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(3, lambda, (T, Tag, Protect))
};

AUTOBOOST_MPL_AUX_NA_SPEC2(1, 3, lambda)

template<
      typename T
    >
struct is_lambda_expression
    : lambda<T>::is_le
{
};

#   undef AUX778076_LAMBDA_PARAMS

}}

#endif // AUTOBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#endif // AUTOBOOST_MPL_AUX_LAMBDA_NO_CTPS_HPP_INCLUDED

///// iteration, depth == 1

#else

#define i_ AUTOBOOST_PP_FRAME_ITERATION(1)

#   define AUX778076_LAMBDA_TYPEDEF(unused, i_, F) \
    typedef lambda< \
          typename F::AUTOBOOST_PP_CAT(arg,AUTOBOOST_PP_INC(i_)) \
        , Tag \
        , false_ \
        > AUTOBOOST_PP_CAT(l,AUTOBOOST_PP_INC(i_)); \
    /**/

#   define AUX778076_IS_LE_TYPEDEF(unused, i_, unused2) \
    typedef typename AUTOBOOST_PP_CAT(l,AUTOBOOST_PP_INC(i_))::is_le \
        AUTOBOOST_PP_CAT(is_le,AUTOBOOST_PP_INC(i_)); \
    /**/

#   define AUX778076_IS_LAMBDA_EXPR(unused, i_, unused2) \
    AUTOBOOST_PP_COMMA_IF(i_) \
    AUTOBOOST_MPL_AUX_MSVC_VALUE_WKND(AUTOBOOST_PP_CAT(is_le,AUTOBOOST_PP_INC(i_)))::value \
    /**/

#   define AUX778076_LAMBDA_RESULT(unused, i_, unused2) \
    , typename AUTOBOOST_PP_CAT(l,AUTOBOOST_PP_INC(i_))::type \
    /**/

template<> struct lambda_impl< int_<i_> >
{
    template< typename F, typename Tag, typename Protect > struct result_
    {
        AUTOBOOST_MPL_PP_REPEAT(i_, AUX778076_LAMBDA_TYPEDEF, F)
        AUTOBOOST_MPL_PP_REPEAT(i_, AUX778076_IS_LE_TYPEDEF, unused)

        typedef aux::lambda_or<
              AUTOBOOST_MPL_PP_REPEAT(i_, AUX778076_IS_LAMBDA_EXPR, unused)
            > is_le;

        typedef AUTOBOOST_PP_CAT(bind,i_)<
              typename F::rebind
            AUTOBOOST_MPL_PP_REPEAT(i_, AUX778076_LAMBDA_RESULT, unused)
            > bind_;

        typedef typename if_<
              is_le
            , if_< Protect, mpl::protect<bind_>, bind_ >
            , identity<F>
            >::type type_;

        typedef typename type_::type type;
    };
};

#   undef AUX778076_LAMBDA_RESULT
#   undef AUX778076_IS_LAMBDA_EXPR
#   undef AUX778076_IS_LE_TYPEDEF
#   undef AUX778076_LAMBDA_TYPEDEF

#undef i_

#endif // AUTOBOOST_PP_IS_ITERATING
