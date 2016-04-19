// Copyright David Abrahams 2009. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef AUTOBOOST_CONCEPT_BACKWARD_COMPATIBILITY_DWA200968_HPP
# define AUTOBOOST_CONCEPT_BACKWARD_COMPATIBILITY_DWA200968_HPP

namespace autoboost
{
  namespace concepts {}

# if defined(AUTOBOOST_HAS_CONCEPTS) && !defined(AUTOBOOST_CONCEPT_NO_BACKWARD_KEYWORD)
  namespace concept = concepts;
# endif
} // namespace autoboost::concept

#endif // AUTOBOOST_CONCEPT_BACKWARD_COMPATIBILITY_DWA200968_HPP
