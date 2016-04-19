//
//=======================================================================
// Copyright 1997, 1998, 1999, 2000 University of Notre Dame.
// Authors: Andrew Lumsdaine, Lie-Quan Lee, Jeremy G. Siek
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
//

#ifndef AUTOBOOST_INDIRECT_CMP_HPP
#define AUTOBOOST_INDIRECT_CMP_HPP

#include <functional>
#include <autoboost/config.hpp>
#include <autoboost/property_map/property_map.hpp>

namespace autoboost {

  //: indirect_cmp
  //
  // could also do this with compose_f_gx_hx, and the member binder...
  //
  //!category: functors
  //!component: type
  //!tparam: ReadablePropertyMap - a model of ReadablePropertyMap
  //!definition: functor.h
  template <class ReadablePropertyMap, class Compare>
  class indirect_cmp {
  public:
    typedef typename autoboost::property_traits<ReadablePropertyMap>::value_type T;
    typedef typename autoboost::property_traits<ReadablePropertyMap>::key_type K;
    typedef K first_argument_type;
    typedef K second_argument_type;
    typedef bool result_type;
    inline indirect_cmp(const ReadablePropertyMap& df, const Compare& c = Compare())
      : d(df), cmp(c) { }

    template <class A, class B>
    inline bool
    operator()(const A& u, const B& v) const {
      const T& du = get(d, u);
      const T& dv = get(d, v);
      return cmp(du, dv);
    }
  protected:
    ReadablePropertyMap d;
    Compare cmp;
  };

  template <typename Compare, typename ReadablePropertyMap>
  indirect_cmp<ReadablePropertyMap, Compare>
  make_indirect_cmp(const Compare& cmp, ReadablePropertyMap pmap) {
    indirect_cmp<ReadablePropertyMap, Compare> p(pmap, cmp);
    return p;
  }

  template <class ReadablePropertyMap>
  class indirect_pmap {
  public:
    typedef typename autoboost::property_traits<ReadablePropertyMap>::value_type T;
    typedef typename autoboost::property_traits<ReadablePropertyMap>::key_type K;
    typedef K argument_type;
    typedef T result_type;
    inline indirect_pmap(const ReadablePropertyMap& df)
      : d(df) { }

    inline T operator()(const K& u) const {
      return get(d, u);
    }
  protected:
    ReadablePropertyMap d;
  };

  template <typename ReadablePropertyMap>
  indirect_pmap<ReadablePropertyMap>
  make_indirect_pmap(ReadablePropertyMap pmap) {
    indirect_pmap<ReadablePropertyMap> f(pmap);
    return f;
  }


} // namespace autoboost


#endif // GGCL_INDIRECT_CMP_HPP
