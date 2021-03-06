// Copyright (C) 2003, Fernando Luis Cacciola Carballal.
// Copyright (C) 2007, Tobias Schwinger.
//
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/optional for documentation.
//
// You are welcome to contact the author at:
//  fernando_cacciola@hotmail.com
//
#ifndef AUTOBOOST_UTILITY_TYPED_INPLACE_FACTORY_04APR2007_HPP
#ifndef AUTOBOOST_PP_IS_ITERATING

#include <autoboost/utility/detail/in_place_factory_prefix.hpp>

namespace autoboost {

class typed_in_place_factory_base {} ;

#define  AUTOBOOST_PP_ITERATION_LIMITS (0, AUTOBOOST_MAX_INPLACE_FACTORY_ARITY)
#define  AUTOBOOST_PP_FILENAME_1 <autoboost/utility/typed_in_place_factory.hpp>
#include AUTOBOOST_PP_ITERATE()

} // namespace autoboost

#include <autoboost/utility/detail/in_place_factory_suffix.hpp>

#define AUTOBOOST_UTILITY_TYPED_INPLACE_FACTORY_04APR2007_HPP
#else
#define N AUTOBOOST_PP_ITERATION()

template< class T AUTOBOOST_PP_ENUM_TRAILING_PARAMS(N,class A) >
class AUTOBOOST_PP_CAT(typed_in_place_factory,N)
  :
  public typed_in_place_factory_base
{
public:

  typedef T value_type;

  explicit AUTOBOOST_PP_CAT(typed_in_place_factory,N)
      ( AUTOBOOST_PP_ENUM_BINARY_PARAMS(N, A, const& a) )
#if N > 0
    : AUTOBOOST_PP_ENUM(N, AUTOBOOST_DEFINE_INPLACE_FACTORY_CLASS_MEMBER_INIT, _)
#endif
  {}

  void* apply (void* address) const
  {
    return new(address) T( AUTOBOOST_PP_ENUM_PARAMS(N, m_a) );
  }

  void* apply (void* address, std::size_t n) const
  {
    for(void* next = address = this->apply(address); !! --n;)
      this->apply(next = static_cast<char *>(next) + sizeof(T));
    return address;
  }

  AUTOBOOST_PP_REPEAT(N, AUTOBOOST_DEFINE_INPLACE_FACTORY_CLASS_MEMBER_DECL, _)
};

template< class T AUTOBOOST_PP_ENUM_TRAILING_PARAMS(N, class A) >
inline AUTOBOOST_PP_CAT(typed_in_place_factory,N)<
    T AUTOBOOST_PP_ENUM_TRAILING_PARAMS(N, A) >
in_place( AUTOBOOST_PP_ENUM_BINARY_PARAMS(N, A, const& a) )
{
  return AUTOBOOST_PP_CAT(typed_in_place_factory,N)<
      T AUTOBOOST_PP_ENUM_TRAILING_PARAMS(N, A) >( AUTOBOOST_PP_ENUM_PARAMS(N, a) );
}

#undef N
#endif
#endif

