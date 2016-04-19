// Copyright David Abrahams 2006. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef AUTOBOOST_CONCEPT_DETAIL_CONCEPT_DEF_DWA200651_HPP
# define AUTOBOOST_CONCEPT_DETAIL_CONCEPT_DEF_DWA200651_HPP
# include <autoboost/preprocessor/seq/for_each_i.hpp>
# include <autoboost/preprocessor/seq/enum.hpp>
# include <autoboost/preprocessor/comma_if.hpp>
# include <autoboost/preprocessor/cat.hpp>
#endif // AUTOBOOST_CONCEPT_DETAIL_CONCEPT_DEF_DWA200651_HPP

// AUTOBOOST_concept(SomeName, (p1)(p2)...(pN))
//
// Expands to "template <class p1, class p2, ...class pN> struct SomeName"
//
// Also defines an equivalent SomeNameConcept for backward compatibility.
// Maybe in the next release we can kill off the "Concept" suffix for good.
# define AUTOBOOST_concept(name, params)                                            \
    template < AUTOBOOST_PP_SEQ_FOR_EACH_I(AUTOBOOST_CONCEPT_typename,~,params) >       \
    struct name; /* forward declaration */                                      \
                                                                                \
    template < AUTOBOOST_PP_SEQ_FOR_EACH_I(AUTOBOOST_CONCEPT_typename,~,params) >       \
    struct AUTOBOOST_PP_CAT(name,Concept)                                           \
      : name< AUTOBOOST_PP_SEQ_ENUM(params) >                                       \
    {                                                                           \
    };                                                                          \
                                                                                \
    template < AUTOBOOST_PP_SEQ_FOR_EACH_I(AUTOBOOST_CONCEPT_typename,~,params) >       \
    struct name

// Helper for AUTOBOOST_concept, above.
# define AUTOBOOST_CONCEPT_typename(r, ignored, index, t) \
    AUTOBOOST_PP_COMMA_IF(index) typename t

