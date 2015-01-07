/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_VECTOR10_05042005_0257)
#define FUSION_VECTOR10_05042005_0257

#include <autoboost/fusion/support/config.hpp>
#include <autoboost/fusion/container/vector/vector10_fwd.hpp>
#include <autoboost/fusion/support/sequence_base.hpp>
#include <autoboost/fusion/support/detail/access.hpp>
#include <autoboost/fusion/iterator/next.hpp>
#include <autoboost/fusion/iterator/deref.hpp>
#include <autoboost/fusion/sequence/intrinsic/begin.hpp>
#include <autoboost/fusion/container/vector/detail/at_impl.hpp>
#include <autoboost/fusion/container/vector/detail/value_at_impl.hpp>
#include <autoboost/fusion/container/vector/detail/begin_impl.hpp>
#include <autoboost/fusion/container/vector/detail/end_impl.hpp>

#include <autoboost/mpl/void.hpp>
#include <autoboost/mpl/int.hpp>
#include <autoboost/mpl/bool.hpp>
#include <autoboost/mpl/at.hpp>
#include <autoboost/mpl/vector/vector10.hpp>
#include <autoboost/type_traits/is_convertible.hpp>
#include <autoboost/utility/enable_if.hpp>

#include <autoboost/preprocessor/dec.hpp>
#include <autoboost/preprocessor/iteration/iterate.hpp>
#include <autoboost/preprocessor/repetition/enum.hpp>
#include <autoboost/preprocessor/repetition/enum_shifted.hpp>
#include <autoboost/preprocessor/repetition/enum_params.hpp>
#include <autoboost/preprocessor/repetition/enum_binary_params.hpp>
#include <autoboost/preprocessor/repetition/repeat_from_to.hpp>

namespace autoboost { namespace fusion
{
    struct vector_tag;
    struct fusion_sequence_tag;
    struct random_access_traversal_tag;

    template <typename Dummy>
    struct vector0 : sequence_base<vector0<Dummy> >
    {
        typedef mpl::vector0<> types;
        typedef vector_tag fusion_tag;
        typedef fusion_sequence_tag tag; // this gets picked up by MPL
        typedef mpl::false_ is_view;
        typedef random_access_traversal_tag category;
        typedef mpl::int_<0> size;

        AUTOBOOST_FUSION_GPU_ENABLED
        vector0() {}

        template<typename Sequence>
        AUTOBOOST_FUSION_GPU_ENABLED
        vector0(Sequence const& /*seq*/)
        {}
    };
}}

#if !defined(AUTOBOOST_FUSION_DONT_USE_PREPROCESSED_FILES)
#include <autoboost/fusion/container/vector/detail/preprocessed/vector10.hpp>
#else
#if defined(__WAVE__) && defined(AUTOBOOST_FUSION_CREATE_PREPROCESSED_FILES)
#pragma wave option(preserve: 2, line: 0, output: "detail/preprocessed/vector10.hpp")
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
    struct vector_tag;
    struct fusion_sequence_tag;
    struct random_access_traversal_tag;

#define FUSION_HASH #
// expand vector1 to vector10
#define AUTOBOOST_PP_FILENAME_1 <autoboost/fusion/container/vector/detail/vector_n.hpp>
#define AUTOBOOST_PP_ITERATION_LIMITS (1, 10)
#include AUTOBOOST_PP_ITERATE()
#undef FUSION_HASH
}}

#if defined(__WAVE__) && defined(AUTOBOOST_FUSION_CREATE_PREPROCESSED_FILES)
#pragma wave option(output: null)
#endif

#endif // AUTOBOOST_FUSION_DONT_USE_PREPROCESSED_FILES

#endif
