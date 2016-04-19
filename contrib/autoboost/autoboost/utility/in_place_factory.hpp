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
#ifndef AUTOBOOST_UTILITY_INPLACE_FACTORY_04APR2007_HPP
#ifndef AUTOBOOST_PP_IS_ITERATING

#include <autoboost/utility/detail/in_place_factory_prefix.hpp>

namespace autoboost {

class in_place_factory_base {} ;

#define  AUTOBOOST_PP_ITERATION_LIMITS (0, AUTOBOOST_MAX_INPLACE_FACTORY_ARITY)
#define  AUTOBOOST_PP_FILENAME_1 <autoboost/utility/in_place_factory.hpp>
#include AUTOBOOST_PP_ITERATE()

} // namespace autoboost

#include <autoboost/utility/detail/in_place_factory_suffix.hpp>

#define AUTOBOOST_UTILITY_INPLACE_FACTORY_04APR2007_HPP
#else
#define N AUTOBOOST_PP_ITERATION()

#if N
template< AUTOBOOST_PP_ENUM_PARAMS(N, class A) >
#endif
class AUTOBOOST_PP_CAT(in_place_factory,N)
  :
  public in_place_factory_base
{
public:

  explicit AUTOBOOST_PP_CAT(in_place_factory,N)
      ( AUTOBOOST_PP_ENUM_BINARY_PARAMS(N,A,const& a) )
#if N > 0
    : AUTOBOOST_PP_ENUM(N, AUTOBOOST_DEFINE_INPLACE_FACTORY_CLASS_MEMBER_INIT, _)
#endif
  {}

  template<class T>
  void* apply(void* address) const
  {
    return new(address) T( AUTOBOOST_PP_ENUM_PARAMS(N, m_a) );
  }

  template<class T>
  void* apply(void* address, std::size_t n) const
  {
    for(char* next = address = this->AUTOBOOST_NESTED_TEMPLATE apply<T>(address);
        !! --n;)
      this->AUTOBOOST_NESTED_TEMPLATE apply<T>(next = next+sizeof(T));
    return address;
  }

  AUTOBOOST_PP_REPEAT(N, AUTOBOOST_DEFINE_INPLACE_FACTORY_CLASS_MEMBER_DECL, _)
};

#if N > 0
template< AUTOBOOST_PP_ENUM_PARAMS(N, class A) >
inline AUTOBOOST_PP_CAT(in_place_factory,N)< AUTOBOOST_PP_ENUM_PARAMS(N, A) >
in_place( AUTOBOOST_PP_ENUM_BINARY_PARAMS(N, A, const& a) )
{
  return AUTOBOOST_PP_CAT(in_place_factory,N)< AUTOBOOST_PP_ENUM_PARAMS(N, A) >
      ( AUTOBOOST_PP_ENUM_PARAMS(N, a) );
}
#else
inline in_place_factory0 in_place()
{
  return in_place_factory0();
}
#endif

#undef N
#endif
#endif

