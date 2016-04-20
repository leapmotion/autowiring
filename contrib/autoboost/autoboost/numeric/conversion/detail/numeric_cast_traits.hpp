//
//! Copyright (c) 2011-2012
//! Brandon Kohn
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#if !defined(AUTOBOOST_NUMERIC_CONVERSION_DONT_USE_PREPROCESSED_FILES)

    #include <autoboost/numeric/conversion/detail/preprocessed/numeric_cast_traits_common.hpp>

	#if !defined(AUTOBOOST_NO_LONG_LONG)
        #include <autoboost/numeric/conversion/detail/preprocessed/numeric_cast_traits_long_long.hpp>
	#endif

#else
#if !AUTOBOOST_PP_IS_ITERATING

    #include <autoboost/preprocessor/seq/elem.hpp>
    #include <autoboost/preprocessor/seq/size.hpp>
    #include <autoboost/preprocessor/iteration/iterate.hpp>

    #if defined(__WAVE__) && defined(AUTOBOOST_NUMERIC_CONVERSION_CREATE_PREPROCESSED_FILES)
        #pragma wave option(preserve: 2, line: 0, output: "preprocessed/numeric_cast_traits_common.hpp")
    #endif
//
//! Copyright (c) 2011-2012
//! Brandon Kohn
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
    #if defined(__WAVE__) && defined(AUTOBOOST_NUMERIC_CONVERSION_CREATE_PREPROCESSED_FILES)
        #pragma wave option(preserve: 1)
    #endif

	//! These are the assumed common built in fundamental types (not typedefs/macros.)
	#define AUTOBOOST_NUMERIC_CONVERSION_BASE_BUILTIN_TYPES() \
        (char)                                            \
        (signed char)                                     \
        (unsigned char)                                   \
        (short)                                           \
        (unsigned short)                                  \
        (int)                                             \
        (unsigned int)                                    \
        (long)                                            \
        (unsigned long)                                   \
        (float)                                           \
        (double)                                          \
        (long double)                                     \
    /***/

    #define AUTOBOOST_NUMERIC_CONVERSION_SEQ_A() AUTOBOOST_NUMERIC_CONVERSION_BASE_BUILTIN_TYPES()
	#define AUTOBOOST_NUMERIC_CONVERSION_SEQ_B() AUTOBOOST_NUMERIC_CONVERSION_BASE_BUILTIN_TYPES()

namespace autoboost { namespace numeric {

    #define AUTOBOOST_PP_ITERATION_PARAMS_1 (3, (0, AUTOBOOST_PP_DEC(AUTOBOOST_PP_SEQ_SIZE(AUTOBOOST_NUMERIC_CONVERSION_SEQ_A())), <autoboost/numeric/conversion/detail/numeric_cast_traits.hpp>))
    #include AUTOBOOST_PP_ITERATE()

}}//namespace autoboost::numeric;

    #if defined(__WAVE__) && defined(AUTOBOOST_NUMERIC_CONVERSION_CREATE_PREPROCESSED_FILES)
        #pragma wave option(output: null)
    #endif

	#if ( defined(__WAVE__) && defined(AUTOBOOST_NUMERIC_CONVERSION_CREATE_PREPROCESSED_FILES) ) || !defined(AUTOBOOST_NO_LONG_LONG)

	    #undef AUTOBOOST_NUMERIC_CONVERSION_SEQ_A
	    #undef AUTOBOOST_NUMERIC_CONVERSION_SEQ_B

	    #if defined(__WAVE__) && defined(AUTOBOOST_NUMERIC_CONVERSION_CREATE_PREPROCESSED_FILES)
            #pragma wave option(preserve: 2, line: 0, output: "preprocessed/numeric_cast_traits_long_long.hpp")
        #endif

//
//! Copyright (c) 2011-2012
//! Brandon Kohn
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
        #if defined(__WAVE__) && defined(AUTOBOOST_NUMERIC_CONVERSION_CREATE_PREPROCESSED_FILES)
            #pragma wave option(preserve: 1)
        #endif

namespace autoboost { namespace numeric {

    #define AUTOBOOST_NUMERIC_CONVERSION_SEQ_A() AUTOBOOST_NUMERIC_CONVERSION_BASE_BUILTIN_TYPES()(autoboost::long_long_type)(autoboost::ulong_long_type)
	#define AUTOBOOST_NUMERIC_CONVERSION_SEQ_B() (autoboost::long_long_type)(autoboost::ulong_long_type)

    #define AUTOBOOST_PP_ITERATION_PARAMS_1 (3, (0, AUTOBOOST_PP_DEC(AUTOBOOST_PP_SEQ_SIZE(AUTOBOOST_NUMERIC_CONVERSION_SEQ_A())), <autoboost/numeric/conversion/detail/numeric_cast_traits.hpp>))
    #include AUTOBOOST_PP_ITERATE()

}}//namespace autoboost::numeric;

        #if defined(__WAVE__) && defined(AUTOBOOST_NUMERIC_CONVERSION_CREATE_PREPROCESSED_FILES)
            #pragma wave option(output: null)
        #endif

	#endif

    #undef AUTOBOOST_NUMERIC_CONVERSION_BASE_BUILTIN_TYPES
	#undef AUTOBOOST_NUMERIC_CONVERSION_SEQ_A
	#undef AUTOBOOST_NUMERIC_CONVERSION_SEQ_B

#elif AUTOBOOST_PP_ITERATION_DEPTH() == 1

    #define AUTOBOOST_PP_ITERATION_PARAMS_2 (3, (0, AUTOBOOST_PP_DEC(AUTOBOOST_PP_SEQ_SIZE(AUTOBOOST_NUMERIC_CONVERSION_SEQ_B())), <autoboost/numeric/conversion/detail/numeric_cast_traits.hpp>))
    #include AUTOBOOST_PP_ITERATE()

#elif AUTOBOOST_PP_ITERATION_DEPTH() == 2

    //! Generate default traits for the specified source and target.
    #define AUTOBOOST_NUMERIC_CONVERSION_A AUTOBOOST_PP_FRAME_ITERATION(1)
    #define AUTOBOOST_NUMERIC_CONVERSION_B AUTOBOOST_PP_FRAME_ITERATION(2)

    template <>
    struct numeric_cast_traits
        <
            AUTOBOOST_PP_SEQ_ELEM(AUTOBOOST_NUMERIC_CONVERSION_A, AUTOBOOST_NUMERIC_CONVERSION_SEQ_A())
          , AUTOBOOST_PP_SEQ_ELEM(AUTOBOOST_NUMERIC_CONVERSION_B, AUTOBOOST_NUMERIC_CONVERSION_SEQ_B())
        >
    {
        typedef def_overflow_handler overflow_policy;
        typedef UseInternalRangeChecker range_checking_policy;
        typedef Trunc<AUTOBOOST_PP_SEQ_ELEM(AUTOBOOST_NUMERIC_CONVERSION_B, AUTOBOOST_NUMERIC_CONVERSION_SEQ_B())> rounding_policy;
    };

    #undef AUTOBOOST_NUMERIC_CONVERSION_A
    #undef AUTOBOOST_NUMERIC_CONVERSION_B

#endif//! Depth 2.
#endif// AUTOBOOST_NUMERIC_CONVERSION_DONT_USE_PREPROCESSED_FILES
