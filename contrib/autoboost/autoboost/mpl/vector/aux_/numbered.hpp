
// NO INCLUDE GUARDS, THE HEADER IS INTENDED FOR MULTIPLE INCLUSION

#if defined(AUTOBOOST_PP_IS_ITERATING)

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

#include <autoboost/preprocessor/enum_params.hpp>
#include <autoboost/preprocessor/enum_shifted_params.hpp>
#include <autoboost/preprocessor/comma_if.hpp>
#include <autoboost/preprocessor/repeat.hpp>
#include <autoboost/preprocessor/dec.hpp>
#include <autoboost/preprocessor/cat.hpp>

#define i_ AUTOBOOST_PP_FRAME_ITERATION(1)

#if defined(AUTOBOOST_MPL_CFG_TYPEOF_BASED_SEQUENCES)

#   define AUX778076_VECTOR_TAIL(vector, i_, T) \
    AUTOBOOST_PP_CAT(vector,i_)< \
          AUTOBOOST_PP_ENUM_PARAMS(i_, T) \
        > \
    /**/

#if i_ > 0
template<
      AUTOBOOST_PP_ENUM_PARAMS(i_, typename T)
    >
struct AUTOBOOST_PP_CAT(vector,i_)
    : v_item<
          AUTOBOOST_PP_CAT(T,AUTOBOOST_PP_DEC(i_))
        , AUX778076_VECTOR_TAIL(vector,AUTOBOOST_PP_DEC(i_),T)
        >
{
    typedef AUTOBOOST_PP_CAT(vector,i_) type;
};
#endif

#   undef AUX778076_VECTOR_TAIL

#else // "brute force" implementation

#   if i_ > 0

template<
      AUTOBOOST_PP_ENUM_PARAMS(i_, typename T)
    >
struct AUTOBOOST_PP_CAT(vector,i_)
{
    typedef aux::vector_tag<i_> tag;
    typedef AUTOBOOST_PP_CAT(vector,i_) type;

#   define AUX778076_VECTOR_ITEM(unused, i_, unused2) \
    typedef AUTOBOOST_PP_CAT(T,i_) AUTOBOOST_PP_CAT(item,i_); \
    /**/

    AUTOBOOST_PP_REPEAT(i_, AUX778076_VECTOR_ITEM, unused)
#   undef AUX778076_VECTOR_ITEM
    typedef void_ AUTOBOOST_PP_CAT(item,i_);
    typedef AUTOBOOST_PP_CAT(T,AUTOBOOST_PP_DEC(i_)) back;

    // Borland forces us to use 'type' here (instead of the class name)
    typedef v_iter<type,0> begin;
    typedef v_iter<type,i_> end;
};

template<>
struct push_front_impl< aux::vector_tag<AUTOBOOST_PP_DEC(i_)> >
{
    template< typename Vector, typename T > struct apply
    {
        typedef AUTOBOOST_PP_CAT(vector,i_)<
              T
              AUTOBOOST_PP_COMMA_IF(AUTOBOOST_PP_DEC(i_))
              AUTOBOOST_PP_ENUM_PARAMS(AUTOBOOST_PP_DEC(i_), typename Vector::item)
            > type;
    };
};

template<>
struct pop_front_impl< aux::vector_tag<i_> >
{
    template< typename Vector > struct apply
    {
        typedef AUTOBOOST_PP_CAT(vector,AUTOBOOST_PP_DEC(i_))<
              AUTOBOOST_PP_ENUM_SHIFTED_PARAMS(i_, typename Vector::item)
            > type;
    };
};


template<>
struct push_back_impl< aux::vector_tag<AUTOBOOST_PP_DEC(i_)> >
{
    template< typename Vector, typename T > struct apply
    {
        typedef AUTOBOOST_PP_CAT(vector,i_)<
              AUTOBOOST_PP_ENUM_PARAMS(AUTOBOOST_PP_DEC(i_), typename Vector::item)
              AUTOBOOST_PP_COMMA_IF(AUTOBOOST_PP_DEC(i_))
              T
            > type;
    };
};

template<>
struct pop_back_impl< aux::vector_tag<i_> >
{
    template< typename Vector > struct apply
    {
        typedef AUTOBOOST_PP_CAT(vector,AUTOBOOST_PP_DEC(i_))<
              AUTOBOOST_PP_ENUM_PARAMS(AUTOBOOST_PP_DEC(i_), typename Vector::item)
            > type;
    };
};

#   endif // i_ > 0

#   if !defined(AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) \
    && !defined(AUTOBOOST_MPL_CFG_NO_NONTYPE_TEMPLATE_PARTIAL_SPEC)

template< typename V >
struct v_at<V,i_>
{
    typedef typename V::AUTOBOOST_PP_CAT(item,i_) type;
};

#   else

namespace aux {
template<> struct v_at_impl<i_>
{
    template< typename V_ > struct result_
    {
        typedef typename V_::AUTOBOOST_PP_CAT(item,i_) type;
    };
};
}

template<>
struct at_impl< aux::vector_tag<i_> >
{
    template< typename V_, typename N > struct apply
    {
        typedef typename aux::v_at_impl<AUTOBOOST_MPL_AUX_VALUE_WKND(N)::value>
            ::template result_<V_>::type type;
    };
};

#if i_ > 0
template<>
struct front_impl< aux::vector_tag<i_> >
{
    template< typename Vector > struct apply
    {
        typedef typename Vector::item0 type;
    };
};

template<>
struct back_impl< aux::vector_tag<i_> >
{
    template< typename Vector > struct apply
    {
        typedef typename Vector::back type;
    };
};

template<>
struct empty_impl< aux::vector_tag<i_> >
{
    template< typename Vector > struct apply
        : false_
    {
    };
};
#endif

template<>
struct size_impl< aux::vector_tag<i_> >
{
    template< typename Vector > struct apply
        : long_<i_>
    {
    };
};

template<>
struct O1_size_impl< aux::vector_tag<i_> >
    : size_impl< aux::vector_tag<i_> >
{
};

template<>
struct clear_impl< aux::vector_tag<i_> >
{
    template< typename Vector > struct apply
    {
        typedef vector0<> type;
    };
};

#   endif // AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION

#endif // AUTOBOOST_MPL_CFG_TYPEOF_BASED_SEQUENCES

#undef i_

#endif // AUTOBOOST_PP_IS_ITERATING
