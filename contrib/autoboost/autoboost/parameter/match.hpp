// Copyright David Abrahams 2005. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef AUTOBOOST_PARAMETER_MATCH_DWA2005714_HPP
# define AUTOBOOST_PARAMETER_MATCH_DWA2005714_HPP

# include <autoboost/detail/workaround.hpp>
# include <autoboost/preprocessor/seq/enum.hpp>

# if AUTOBOOST_WORKAROUND(__MWERKS__, <= 0x3003)
// Temporary version of AUTOBOOST_PP_SEQ_ENUM until Paul M. integrates the workaround.
#  define AUTOBOOST_PARAMETER_SEQ_ENUM_I(size,seq) AUTOBOOST_PP_CAT(AUTOBOOST_PP_SEQ_ENUM_, size) seq
#  define AUTOBOOST_PARAMETER_SEQ_ENUM(seq) AUTOBOOST_PARAMETER_SEQ_ENUM_I(AUTOBOOST_PP_SEQ_SIZE(seq), seq)
# else
#  define AUTOBOOST_PARAMETER_SEQ_ENUM(seq) AUTOBOOST_PP_SEQ_ENUM(seq)
# endif

# if AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x564))

#  include <autoboost/parameter/config.hpp>
#  include <autoboost/parameter/aux_/void.hpp>
#  include <autoboost/preprocessor/arithmetic/sub.hpp>
#  include <autoboost/preprocessor/facilities/intercept.hpp>
#  include <autoboost/preprocessor/repetition/enum_trailing_params.hpp>

#  define AUTOBOOST_PARAMETER_MATCH_DEFAULTS(ArgTypes)              \
        AUTOBOOST_PP_ENUM_TRAILING_PARAMS(                          \
            AUTOBOOST_PP_SUB(                                       \
                AUTOBOOST_PARAMETER_MAX_ARITY                       \
              , AUTOBOOST_PP_SEQ_SIZE(ArgTypes)                     \
            )                                                   \
          , ::autoboost::parameter::void_ AUTOBOOST_PP_INTERCEPT   \
        )

# else

#  define AUTOBOOST_PARAMETER_MATCH_DEFAULTS(ArgTypes)

# endif

//
// Generates, e.g.
//
//    typename dfs_params::match<A1,A2>::type name = dfs_params()
//
// with workarounds for Borland compatibility.
//

# define AUTOBOOST_PARAMETER_MATCH(ParameterSpec, ArgTypes, name)   \
    typename ParameterSpec ::match<                             \
        AUTOBOOST_PARAMETER_SEQ_ENUM(ArgTypes)                      \
        AUTOBOOST_PARAMETER_MATCH_DEFAULTS(ArgTypes)                \
    >::type name = ParameterSpec ()

#endif // AUTOBOOST_PARAMETER_MATCH_DWA2005714_HPP
