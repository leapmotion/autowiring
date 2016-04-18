
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

// NO INCLUDE GUARDS, THE HEADER IS INTENDED FOR MULTIPLE INCLUSION!

#if !defined(AUTOBOOST_MPL_PREPROCESSING_MODE)
#   include <autoboost/mpl/bool.hpp>
#   include <autoboost/mpl/aux_/nested_type_wknd.hpp>
#   include <autoboost/mpl/aux_/na_spec.hpp>
#   include <autoboost/mpl/aux_/lambda_support.hpp>
#endif

#include <autoboost/mpl/limits/arity.hpp>
#include <autoboost/mpl/aux_/preprocessor/params.hpp>
#include <autoboost/mpl/aux_/preprocessor/ext_params.hpp>
#include <autoboost/mpl/aux_/preprocessor/def_params_tail.hpp>
#include <autoboost/mpl/aux_/preprocessor/enum.hpp>
#include <autoboost/mpl/aux_/preprocessor/sub.hpp>
#include <autoboost/mpl/aux_/config/ctps.hpp>
#include <autoboost/mpl/aux_/config/workaround.hpp>

#include <autoboost/preprocessor/dec.hpp>
#include <autoboost/preprocessor/inc.hpp>
#include <autoboost/preprocessor/cat.hpp>

namespace autoboost { namespace mpl {

#   define AUX778076_PARAMS(param, sub) \
    AUTOBOOST_MPL_PP_PARAMS( \
          AUTOBOOST_MPL_PP_SUB(AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY, sub) \
        , param \
        ) \
    /**/

#   define AUX778076_SHIFTED_PARAMS(param, sub) \
    AUTOBOOST_MPL_PP_EXT_PARAMS( \
          2, AUTOBOOST_MPL_PP_SUB(AUTOBOOST_PP_INC(AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY), sub) \
        , param \
        ) \
    /**/

#   define AUX778076_SPEC_PARAMS(param) \
    AUTOBOOST_MPL_PP_ENUM( \
          AUTOBOOST_PP_DEC(AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY) \
        , param \
        ) \
    /**/

namespace aux {

#if !defined(AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

template< bool C_, AUX778076_PARAMS(typename T, 1) >
struct AUTOBOOST_PP_CAT(AUX778076_OP_NAME,impl)
    : AUTOBOOST_PP_CAT(AUX778076_OP_VALUE1,_)
{
};

template< AUX778076_PARAMS(typename T, 1) >
struct AUTOBOOST_PP_CAT(AUX778076_OP_NAME,impl)< AUX778076_OP_VALUE2,AUX778076_PARAMS(T, 1) >
    : AUTOBOOST_PP_CAT(AUX778076_OP_NAME,impl)<
          AUTOBOOST_MPL_AUX_NESTED_TYPE_WKND(T1)::value
        , AUX778076_SHIFTED_PARAMS(T, 1)
        , AUTOBOOST_PP_CAT(AUX778076_OP_VALUE2,_)
        >
{
};

template<>
struct AUTOBOOST_PP_CAT(AUX778076_OP_NAME,impl)<
          AUX778076_OP_VALUE2
        , AUX778076_SPEC_PARAMS(AUTOBOOST_PP_CAT(AUX778076_OP_VALUE2,_))
        >
    : AUTOBOOST_PP_CAT(AUX778076_OP_VALUE2,_)
{
};

#else

template< bool C_ > struct AUTOBOOST_PP_CAT(AUX778076_OP_NAME,impl)
{
    template< AUX778076_PARAMS(typename T, 1) > struct result_
        : AUTOBOOST_PP_CAT(AUX778076_OP_VALUE1,_)
    {
    };
};

template<> struct AUTOBOOST_PP_CAT(AUX778076_OP_NAME,impl)<AUX778076_OP_VALUE2>
{
    template< AUX778076_PARAMS(typename T, 1) > struct result_
        : AUTOBOOST_PP_CAT(AUX778076_OP_NAME,impl)<
              AUTOBOOST_MPL_AUX_NESTED_TYPE_WKND(T1)::value
            >::template result_< AUX778076_SHIFTED_PARAMS(T,1),AUTOBOOST_PP_CAT(AUX778076_OP_VALUE2,_) >
    {
    };

#if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, == 1300)
    template<> struct result_<AUX778076_SPEC_PARAMS(AUTOBOOST_PP_CAT(AUX778076_OP_VALUE2,_))>
        : AUTOBOOST_PP_CAT(AUX778076_OP_VALUE2,_)
    {
    };
};
#else
};

template<>
struct AUTOBOOST_PP_CAT(AUX778076_OP_NAME,impl)<AUX778076_OP_VALUE2>
    ::result_< AUX778076_SPEC_PARAMS(AUTOBOOST_PP_CAT(AUX778076_OP_VALUE2,_)) >
        : AUTOBOOST_PP_CAT(AUX778076_OP_VALUE2,_)
{
};
#endif // AUTOBOOST_MSVC == 1300

#endif // AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION

} // namespace aux

template<
      typename AUTOBOOST_MPL_AUX_NA_PARAM(T1)
    , typename AUTOBOOST_MPL_AUX_NA_PARAM(T2)
    AUTOBOOST_MPL_PP_DEF_PARAMS_TAIL(2, typename T, AUTOBOOST_PP_CAT(AUX778076_OP_VALUE2,_))
    >
struct AUX778076_OP_NAME
#if !defined(AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
    : aux::AUTOBOOST_PP_CAT(AUX778076_OP_NAME,impl)<
          AUTOBOOST_MPL_AUX_NESTED_TYPE_WKND(T1)::value
        , AUX778076_SHIFTED_PARAMS(T,0)
        >
#else
    : aux::AUTOBOOST_PP_CAT(AUX778076_OP_NAME,impl)<
          AUTOBOOST_MPL_AUX_NESTED_TYPE_WKND(T1)::value
        >::template result_< AUX778076_SHIFTED_PARAMS(T,0) >
#endif
{
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(
          AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY
        , AUX778076_OP_NAME
        , (AUX778076_PARAMS(T, 0))
        )
};

AUTOBOOST_MPL_AUX_NA_SPEC2(
      2
    , AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY
    , AUX778076_OP_NAME
    )

}}

#undef AUX778076_SPEC_PARAMS
#undef AUX778076_SHIFTED_PARAMS
#undef AUX778076_PARAMS
#undef AUX778076_OP_NAME
#undef AUX778076_OP_VALUE1
#undef AUX778076_OP_VALUE2
