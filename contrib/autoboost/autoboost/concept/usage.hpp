// Copyright David Abrahams 2006. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef AUTOBOOST_CONCEPT_USAGE_DWA2006919_HPP
# define AUTOBOOST_CONCEPT_USAGE_DWA2006919_HPP

# include <autoboost/concept/assert.hpp>
# include <autoboost/detail/workaround.hpp>
# include <autoboost/concept/detail/backward_compatibility.hpp>

namespace autoboost { namespace concepts {

template <class Model>
struct usage_requirements
{
    ~usage_requirements() { ((Model*)0)->~Model(); }
};

#  if AUTOBOOST_WORKAROUND(__GNUC__, <= 3)

#   define AUTOBOOST_CONCEPT_USAGE(model)                                    \
      model(); /* at least 2.96 and 3.4.3 both need this :( */           \
      AUTOBOOST_CONCEPT_ASSERT((autoboost::concepts::usage_requirements<model>)); \
      ~model()

#  else

#   define AUTOBOOST_CONCEPT_USAGE(model)                                    \
      AUTOBOOST_CONCEPT_ASSERT((autoboost::concepts::usage_requirements<model>)); \
      ~model()

#  endif

}} // namespace autoboost::concepts

#endif // AUTOBOOST_CONCEPT_USAGE_DWA2006919_HPP
