
// Copyright Aleksey Gurtovoy 2000-2004
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

// Preprocessed version of "autoboost/mpl/equal_to.hpp" header
// -- DO NOT modify by hand!

namespace autoboost { namespace mpl {

template<
      typename Tag1
    , typename Tag2

    , AUTOBOOST_MPL_AUX_NTTP_DECL(int, tag1_)  = AUTOBOOST_MPL_AUX_MSVC_VALUE_WKND(Tag1)::value
    , AUTOBOOST_MPL_AUX_NTTP_DECL(int, tag2_)  = AUTOBOOST_MPL_AUX_MSVC_VALUE_WKND(Tag2)::value
    >
struct equal_to_impl
    : if_c<
          ( tag1_ > tag2_ )
        , aux::cast2nd_impl< equal_to_impl< Tag1,Tag1 >,Tag1, Tag2 >
        , aux::cast1st_impl< equal_to_impl< Tag2,Tag2 >,Tag1, Tag2 >
        >::type
{
};

/// for Digital Mars C++/compilers with no CTPS/TTP support
template<> struct equal_to_impl< na,na >
{
    template< typename U1, typename U2 > struct apply
    {
        typedef apply type;
        AUTOBOOST_STATIC_CONSTANT(int, value  = 0);
    };
};

template<> struct equal_to_impl< na,integral_c_tag >
{
    template< typename U1, typename U2 > struct apply
    {
        typedef apply type;
        AUTOBOOST_STATIC_CONSTANT(int, value  = 0);
    };
};

template<> struct equal_to_impl< integral_c_tag,na >
{
    template< typename U1, typename U2 > struct apply
    {
        typedef apply type;
        AUTOBOOST_STATIC_CONSTANT(int, value  = 0);
    };
};

template< typename T > struct equal_to_tag
    : tag< T,na >
{
};

template<
      typename AUTOBOOST_MPL_AUX_NA_PARAM(N1)
    , typename AUTOBOOST_MPL_AUX_NA_PARAM(N2)
    >
struct equal_to
    : aux::msvc_eti_base< typename apply_wrap2<
          equal_to_impl<
              typename equal_to_tag<N1>::type
            , typename equal_to_tag<N2>::type
            >
        , N1
        , N2
        >::type >::type

{
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(2, equal_to, (N1, N2))

};

AUTOBOOST_MPL_AUX_NA_SPEC2(2, 2, equal_to)

}}

namespace autoboost { namespace mpl {

template<>
struct equal_to_impl< integral_c_tag,integral_c_tag >
{
    template< typename N1, typename N2 > struct apply

    {
        AUTOBOOST_STATIC_CONSTANT(bool, value =
             ( AUTOBOOST_MPL_AUX_VALUE_WKND(N1)::value ==
             AUTOBOOST_MPL_AUX_VALUE_WKND(N2)::value )
            );
        typedef bool_<value> type;
    };
};

}}
