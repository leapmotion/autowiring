#ifndef AUTOBOOST_ARCHIVE_ITERATORS_DATAFLOW_HPP
#define AUTOBOOST_ARCHIVE_ITERATORS_DATAFLOW_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// dataflow.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <autoboost/assert.hpp>

#include <autoboost/mpl/eval_if.hpp>
#include <autoboost/mpl/if.hpp>
#include <autoboost/mpl/bool.hpp>
#include <autoboost/mpl/apply.hpp>
#include <autoboost/mpl/plus.hpp>
#include <autoboost/mpl/int.hpp>

#include <autoboost/type_traits/is_convertible.hpp>
#include <autoboost/type_traits/is_base_and_derived.hpp>
#include <autoboost/type_traits/is_pointer.hpp>
#include <autoboost/iterator/iterator_adaptor.hpp>
#include <autoboost/iterator/iterator_traits.hpp>
#include <autoboost/static_assert.hpp>

namespace autoboost {
namespace archive {
namespace iterators {

// poor man's tri-state
struct tri_state {
    enum state_enum {
        is_false = false,
        is_true = true,
        is_indeterminant
    } m_state;
    // convert to bool
    operator bool (){
        AUTOBOOST_ASSERT(is_indeterminant != m_state);
        return is_true == m_state ? true : false;
    }
    // assign from bool
    tri_state & operator=(bool rhs) {
        m_state = rhs ? is_true : is_false;
        return *this;
    }
    tri_state(bool rhs) :
        m_state(rhs ? is_true : is_false)
    {}
    tri_state(state_enum state) :
        m_state(state)
    {}
    bool operator==(const tri_state & rhs) const {
        return m_state == rhs.m_state;
    }
    bool operator!=(const tri_state & rhs) const {
        return m_state != rhs.m_state;
    }
};

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// implement functions common to dataflow iterators
template<class Derived>
class dataflow {
    bool m_eoi;
protected:
    // test for iterator equality
    tri_state equal(const Derived & rhs) const {
        if(m_eoi && rhs.m_eoi)
            return true;
        if(m_eoi || rhs.m_eoi)
            return false;
        return tri_state(tri_state::is_indeterminant);
    }
    void eoi(bool tf){
        m_eoi = tf;
    }
    bool eoi() const {
        return m_eoi;
    }
public:
    dataflow(bool tf) :
        m_eoi(tf)
    {}
    dataflow() : // used for iterator end
        m_eoi(true)
    {}
};

} // namespace iterators
} // namespace archive
} // namespace autoboost

#endif // AUTOBOOST_ARCHIVE_ITERATORS_DATAFLOW_HPP
