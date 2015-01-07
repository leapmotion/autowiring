/*=============================================================================
    Copyright (c) 2005-2012 Joel de Guzman
    Copyright (c) 2005-2007 Dan Marsden

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_PP_DEQUE_FORWARD_02092007_0749)
#define FUSION_PP_DEQUE_FORWARD_02092007_0749

#if defined(AUTOBOOST_FUSION_HAS_VARIADIC_DEQUE)
#error "C++03 only! This file should not have been included"
#endif

#include <autoboost/fusion/container/deque/detail/cpp03/limits.hpp>
#include <autoboost/preprocessor/repetition/enum_params_with_a_default.hpp>

#if !defined(AUTOBOOST_FUSION_DONT_USE_PREPROCESSED_FILES)
#include <autoboost/fusion/container/deque/detail/cpp03/preprocessed/deque_fwd.hpp>
#else
#if defined(__WAVE__) && defined(AUTOBOOST_FUSION_CREATE_PREPROCESSED_FILES)
#pragma wave option(preserve: 2, line: 0, output: "preprocessed/deque" FUSION_MAX_DEQUE_SIZE_STR "_fwd.hpp")
#endif

/*=============================================================================
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
    struct void_;

    template<
        AUTOBOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(
            FUSION_MAX_DEQUE_SIZE, typename T, void_)>
    struct deque;
}}

#if defined(__WAVE__) && defined(AUTOBOOST_FUSION_CREATE_PREPROCESSED_FILES)
#pragma wave option(output: null)
#endif

#endif // AUTOBOOST_FUSION_DONT_USE_PREPROCESSED_FILES

#endif
