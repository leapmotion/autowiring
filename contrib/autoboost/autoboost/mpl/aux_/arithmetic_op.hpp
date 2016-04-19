
// NO INCLUDE GUARDS, THE HEADER IS INTENDED FOR MULTIPLE INCLUSION

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
#   include <autoboost/mpl/integral_c.hpp>
#   include <autoboost/mpl/aux_/largest_int.hpp>
#   include <autoboost/mpl/aux_/value_wknd.hpp>
#endif

#if !defined(AUX778076_OP_PREFIX)
#   define AUX778076_OP_PREFIX AUX778076_OP_NAME
#endif

#include <autoboost/mpl/aux_/numeric_op.hpp>
#include <autoboost/mpl/aux_/config/static_constant.hpp>
#include <autoboost/mpl/aux_/config/use_preprocessed.hpp>

#if !defined(AUTOBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS) \
    && !defined(AUTOBOOST_MPL_PREPROCESSING_MODE)

#   define AUTOBOOST_MPL_PREPROCESSED_HEADER AUX778076_OP_PREFIX.hpp
#   include <autoboost/mpl/aux_/include_preprocessed.hpp>

#else

#   include <autoboost/mpl/aux_/config/workaround.hpp>
#   include <autoboost/preprocessor/cat.hpp>


namespace autoboost { namespace mpl {

#if defined(AUTOBOOST_MPL_CFG_NO_NESTED_VALUE_ARITHMETIC)
namespace aux {
template< typename T, T n1, T n2 >
struct AUTOBOOST_PP_CAT(AUX778076_OP_PREFIX,_wknd)
{
    AUTOBOOST_STATIC_CONSTANT(T, value = (n1 AUX778076_OP_TOKEN n2));
    typedef integral_c<T,value> type;
};
}
#endif

template<>
struct AUX778076_OP_IMPL_NAME<integral_c_tag,integral_c_tag>
{
    template< typename N1, typename N2 > struct apply
#if !defined(AUTOBOOST_MPL_CFG_NO_NESTED_VALUE_ARITHMETIC)
        : integral_c<
              typename aux::largest_int<
                  typename N1::value_type
                , typename N2::value_type
                >::type
            , ( AUTOBOOST_MPL_AUX_VALUE_WKND(N1)::value
                  AUX778076_OP_TOKEN AUTOBOOST_MPL_AUX_VALUE_WKND(N2)::value
                )
            >
#else
        : aux::AUTOBOOST_PP_CAT(AUX778076_OP_PREFIX,_wknd)<
              typename aux::largest_int<
                  typename N1::value_type
                , typename N2::value_type
                >::type
            , N1::value
            , N2::value
            >::type
#endif
    {
    };
};

}}

#endif // AUTOBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS

#undef AUX778076_OP_TAG_NAME
#undef AUX778076_OP_IMPL_NAME
#undef AUX778076_OP_ARITY
#undef AUX778076_OP_PREFIX
#undef AUX778076_OP_NAME
#undef AUX778076_OP_TOKEN
