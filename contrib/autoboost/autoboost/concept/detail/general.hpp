// Copyright David Abrahams 2006. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef AUTOBOOST_CONCEPT_DETAIL_GENERAL_DWA2006429_HPP
# define AUTOBOOST_CONCEPT_DETAIL_GENERAL_DWA2006429_HPP

# include <autoboost/preprocessor/cat.hpp>
# include <autoboost/concept/detail/backward_compatibility.hpp>

# ifdef AUTOBOOST_OLD_CONCEPT_SUPPORT
#  include <autoboost/concept/detail/has_constraints.hpp>
#  include <autoboost/mpl/if.hpp>
# endif

// This implementation works on Comeau and GCC, all the way back to
// 2.95
namespace autoboost { namespace concepts {

template <class ModelFn>
struct requirement_;

namespace detail
{
  template <void(*)()> struct instantiate {};
}

template <class Model>
struct requirement
{
    static void failed() { ((Model*)0)->~Model(); }
};

struct failed {};

template <class Model>
struct requirement<failed ************ Model::************>
{
    static void failed() { ((Model*)0)->~Model(); }
};

# ifdef AUTOBOOST_OLD_CONCEPT_SUPPORT

template <class Model>
struct constraint
{
    static void failed() { ((Model*)0)->constraints(); }
};

template <class Model>
struct requirement_<void(*)(Model)>
  : mpl::if_<
        concepts::not_satisfied<Model>
      , constraint<Model>
      , requirement<failed ************ Model::************>
    >::type
{};

# else

// For GCC-2.x, these can't have exactly the same name
template <class Model>
struct requirement_<void(*)(Model)>
    : requirement<failed ************ Model::************>
{};

# endif

// Version check from https://svn.boost.org/trac/autoboost/changeset/82886
// (autoboost/static_assert.hpp)
#if defined(__GNUC__) && ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 7)))
#define AUTOBOOST_CONCEPT_UNUSED_TYPEDEF __attribute__((unused))
#else
#define AUTOBOOST_CONCEPT_UNUSED_TYPEDEF /**/
#endif

#  define AUTOBOOST_CONCEPT_ASSERT_FN( ModelFnPtr )             \
    typedef ::autoboost::concepts::detail::instantiate<          \
    &::autoboost::concepts::requirement_<ModelFnPtr>::failed>    \
      AUTOBOOST_PP_CAT(autoboost_concept_check,__LINE__)             \
      AUTOBOOST_CONCEPT_UNUSED_TYPEDEF

}}

#endif // AUTOBOOST_CONCEPT_DETAIL_GENERAL_DWA2006429_HPP
