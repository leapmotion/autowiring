#ifndef AUTOBOOST_PP_IS_ITERATING
/*=============================================================================
    Copyright (c) 2011 Eric Niebler
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(AUTOBOOST_FUSION_VECTOR20_FWD_HPP_INCLUDED)
#define AUTOBOOST_FUSION_VECTOR20_FWD_HPP_INCLUDED

#include <autoboost/fusion/support/config.hpp>
#include <autoboost/preprocessor/cat.hpp>
#include <autoboost/preprocessor/iteration/iterate.hpp>
#include <autoboost/preprocessor/repetition/enum_params.hpp>

#if !defined(AUTOBOOST_FUSION_DONT_USE_PREPROCESSED_FILES)
#include <autoboost/fusion/container/vector/detail/preprocessed/vector20_fwd.hpp>
#else
#if defined(__WAVE__) && defined(AUTOBOOST_FUSION_CREATE_PREPROCESSED_FILES)
#pragma wave option(preserve: 2, line: 0, output: "detail/preprocessed/vector20_fwd.hpp")
#endif

/*=============================================================================
    Copyright (c) 2011 Eric Niebler
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

    This is an auto-generated file. Do not edit!
==============================================================================*/

#if defined(__WAVE__) && defined(AUTOBOOST_FUSION_CREATE_PREPROCESSED_FILES)
#pragma wave option(preserve: 1)
#endif

namespace autoboost { namespace fusion
{
    // expand vector11 to vector20
    #define AUTOBOOST_PP_FILENAME_1 <autoboost/fusion/container/vector/vector20_fwd.hpp>
    #define AUTOBOOST_PP_ITERATION_LIMITS (11, 20)
    #include AUTOBOOST_PP_ITERATE()
}}

#if defined(__WAVE__) && defined(AUTOBOOST_FUSION_CREATE_PREPROCESSED_FILES)
#pragma wave option(output: null)
#endif

#endif // AUTOBOOST_FUSION_DONT_USE_PREPROCESSED_FILES

#endif

#else

    template <AUTOBOOST_PP_ENUM_PARAMS(AUTOBOOST_PP_ITERATION(), typename T)>
    struct AUTOBOOST_PP_CAT(vector, AUTOBOOST_PP_ITERATION());

#endif
