//  tuple.hpp - Boost Tuple Library --------------------------------------

// Copyright (C) 1999, 2000 Jaakko Jarvi (jaakko.jarvi@cs.utu.fi)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

// -----------------------------------------------------------------

#ifndef AUTOBOOST_TUPLE_HPP
#define AUTOBOOST_TUPLE_HPP

#if defined(__sgi) && defined(_COMPILER_VERSION) && _COMPILER_VERSION <= 730
// Work around a compiler bug.
// autoboost::python::tuple has to be seen by the compiler before the
// autoboost::tuple class template.
namespace autoboost { namespace python { class tuple; }}
#endif

#include "autoboost/config.hpp"
#include "autoboost/static_assert.hpp"

// other compilers
#include "autoboost/ref.hpp"
#include "autoboost/tuple/detail/tuple_basic.hpp"


namespace autoboost {

using tuples::tuple;
using tuples::make_tuple;
using tuples::tie;
#if !defined(AUTOBOOST_NO_USING_TEMPLATE)
using tuples::get;
#else
//
// The "using tuples::get" statement causes the
// Borland compiler to ICE, use forwarding
// functions instead:
//
template<int N, class HT, class TT>
inline typename tuples::access_traits<
                  typename tuples::element<N, tuples::cons<HT, TT> >::type
                >::non_const_type
get(tuples::cons<HT, TT>& c) {
  return tuples::get<N,HT,TT>(c);
}
// get function for const cons-lists, returns a const reference to
// the element. If the element is a reference, returns the reference
// as such (that is, can return a non-const reference)
template<int N, class HT, class TT>
inline typename tuples::access_traits<
                  typename tuples::element<N, tuples::cons<HT, TT> >::type
                >::const_type
get(const tuples::cons<HT, TT>& c) {
  return tuples::get<N,HT,TT>(c);
}

#endif // AUTOBOOST_NO_USING_TEMPLATE

} // end namespace autoboost


#endif // AUTOBOOST_TUPLE_HPP
