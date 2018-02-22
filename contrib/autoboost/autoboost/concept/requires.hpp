// Copyright David Abrahams 2006. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef AUTOBOOST_CONCEPT_REQUIRES_DWA2006430_HPP
# define AUTOBOOST_CONCEPT_REQUIRES_DWA2006430_HPP

# include <autoboost/config.hpp>
# include <autoboost/concept/assert.hpp>
# include <autoboost/preprocessor/seq/for_each.hpp>

namespace autoboost {

// unaryfunptr_arg_type from parameter/aux_/parenthesized_type.hpp

namespace ccheck_aux {

// A metafunction that transforms void(*)(T) -> T
template <class UnaryFunctionPointer>
struct unaryfunptr_arg_type;

template <class Arg>
struct unaryfunptr_arg_type<void(*)(Arg)>
{
    typedef Arg type;
};

template <>
struct unaryfunptr_arg_type<void(*)(void)>
{
    typedef void type;
};

} // namespace ccheck_aux

// Template for use in handwritten assertions
template <class Model, class More>
struct requires_ : More
{
    AUTOBOOST_CONCEPT_ASSERT((Model));
};

// Template for use by macros, where models must be wrapped in parens.
// This isn't in namespace detail to keep extra cruft out of resulting
// error messages.
template <class ModelFn>
struct _requires_
{
    enum { value = 0 };
    AUTOBOOST_CONCEPT_ASSERT_FN(ModelFn);
};

template <int check, class Result>
struct Requires_ : ::autoboost::ccheck_aux::unaryfunptr_arg_type<Result>
{
};

# if AUTOBOOST_WORKAROUND(AUTOBOOST_INTEL_WIN, AUTOBOOST_TESTED_AT(1010))
#  define AUTOBOOST_CONCEPT_REQUIRES_(r,data,t) | (::autoboost::_requires_<void(*)t>::value)
# else
#  define AUTOBOOST_CONCEPT_REQUIRES_(r,data,t) + (::autoboost::_requires_<void(*)t>::value)
# endif

#if defined(NDEBUG)

# define AUTOBOOST_CONCEPT_REQUIRES(models, result)                                    \
    typename ::autoboost::ccheck_aux::unaryfunptr_arg_type<void(*)result>::type

#elif AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x564))

// Same thing as below without the initial typename
# define AUTOBOOST_CONCEPT_REQUIRES(models, result)                                \
    ::autoboost::Requires_<                                                        \
      (0 AUTOBOOST_PP_SEQ_FOR_EACH(AUTOBOOST_CONCEPT_REQUIRES_, ~, models)),           \
      ::autoboost::ccheck_aux::unaryfunptr_arg_type<void(*)result>          \
                     >::type

#else

// This just ICEs on MSVC6 :(
# define AUTOBOOST_CONCEPT_REQUIRES(models, result)                                        \
    typename ::autoboost::Requires_<                                                       \
      (0 AUTOBOOST_PP_SEQ_FOR_EACH(AUTOBOOST_CONCEPT_REQUIRES_, ~, models)),                   \
      void(*)result                                                                 \
    >::type

#endif

// C++0x proposed syntax changed.  This supports an older usage
#define AUTOBOOST_CONCEPT_WHERE(models,result) AUTOBOOST_CONCEPT_REQUIRES(models,result)

} // namespace autoboost::concept_check

#endif // AUTOBOOST_CONCEPT_REQUIRES_DWA2006430_HPP
