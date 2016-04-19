// Copyright David Abrahams 2006. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef AUTOBOOST_CONCEPT_DETAIL_BORLAND_DWA2006429_HPP
# define AUTOBOOST_CONCEPT_DETAIL_BORLAND_DWA2006429_HPP

# include <autoboost/preprocessor/cat.hpp>
# include <autoboost/concept/detail/backward_compatibility.hpp>

namespace autoboost { namespace concepts {

template <class ModelFnPtr>
struct require;

template <class Model>
struct require<void(*)(Model)>
{
    enum { instantiate = sizeof((((Model*)0)->~Model()), 3) };
};

#  define AUTOBOOST_CONCEPT_ASSERT_FN( ModelFnPtr )         \
  enum                                                  \
  {                                                     \
      AUTOBOOST_PP_CAT(autoboost_concept_check,__LINE__) =      \
      autoboost::concepts::require<ModelFnPtr>::instantiate  \
  }

}} // namespace autoboost::concept

#endif // AUTOBOOST_CONCEPT_DETAIL_BORLAND_DWA2006429_HPP
