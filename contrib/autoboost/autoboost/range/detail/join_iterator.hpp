// Boost.Range library
//
//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Acknowledgements:
// aschoedl contributed an improvement to the determination
// of the Reference type parameter.
//
// Leonid Gershanovich reported Trac ticket 7376 about the dereference operator
// requiring identical reference types due to using the ternary if.
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef AUTOBOOST_RANGE_DETAIL_JOIN_ITERATOR_HPP_INCLUDED
#define AUTOBOOST_RANGE_DETAIL_JOIN_ITERATOR_HPP_INCLUDED

#include <iterator>
#include <autoboost/assert.hpp>
#include <autoboost/iterator/iterator_traits.hpp>
#include <autoboost/iterator/iterator_facade.hpp>
#include <autoboost/range/begin.hpp>
#include <autoboost/range/end.hpp>
#include <autoboost/range/empty.hpp>
#include <autoboost/range/detail/demote_iterator_traversal_tag.hpp>
#include <autoboost/range/value_type.hpp>
#include <autoboost/type_traits/add_const.hpp>
#include <autoboost/type_traits/add_reference.hpp>
#include <autoboost/type_traits/remove_const.hpp>
#include <autoboost/type_traits/remove_reference.hpp>
#include <autoboost/next_prior.hpp>

namespace autoboost
{
    namespace range_detail
    {

template<typename Iterator1, typename Iterator2>
struct join_iterator_link
{
public:
    join_iterator_link(Iterator1 last1, Iterator2 first2)
        :    last1(last1)
        ,    first2(first2)
    {
    }

    Iterator1 last1;
    Iterator2 first2;

private:
    join_iterator_link() /* = delete */ ;
};

class join_iterator_begin_tag {};
class join_iterator_end_tag {};

template<typename Iterator1
       , typename Iterator2
       , typename Reference
>
class join_iterator_union
{
public:
    typedef Iterator1 iterator1_t;
    typedef Iterator2 iterator2_t;

    join_iterator_union() {}
    join_iterator_union(unsigned int /*selected*/, const iterator1_t& it1, const iterator2_t& it2) : m_it1(it1), m_it2(it2) {}

    iterator1_t& it1() { return m_it1; }
    const iterator1_t& it1() const { return m_it1; }

    iterator2_t& it2() { return m_it2; }
    const iterator2_t& it2() const { return m_it2; }

    Reference dereference(unsigned int selected) const
    {
        if (selected)
            return *m_it2;
        return *m_it1;
    }

    bool equal(const join_iterator_union& other, unsigned int selected) const
    {
        return selected
            ? m_it2 == other.m_it2
            : m_it1 == other.m_it1;
    }

private:
    iterator1_t m_it1;
    iterator2_t m_it2;
};

template<class Iterator, class Reference>
class join_iterator_union<Iterator, Iterator, Reference>
{
public:
    typedef Iterator iterator1_t;
    typedef Iterator iterator2_t;

    join_iterator_union() {}

    join_iterator_union(unsigned int selected, const iterator1_t& it1, const iterator2_t& it2)
        : m_it(selected ? it2 : it1)
    {
    }

    iterator1_t& it1() { return m_it; }
    const iterator1_t& it1() const { return m_it; }

    iterator2_t& it2() { return m_it; }
    const iterator2_t& it2() const { return m_it; }

    Reference dereference(unsigned int) const
    {
        return *m_it;
    }

    bool equal(const join_iterator_union& other,
               unsigned int /*selected*/) const
    {
        return m_it == other.m_it;
    }

private:
    iterator1_t m_it;
};

template<typename Iterator1
       , typename Iterator2
       , typename ValueType = typename iterator_value<Iterator1>::type
       // find least demanding, commonly supported reference type, in the order &, const&, and by-value:
       , typename Reference = typename mpl::if_c<
                !is_reference<typename iterator_reference<Iterator1>::type>::value
             || !is_reference<typename iterator_reference<Iterator2>::type>::value,
                        typename remove_const<
                            typename remove_reference<
                                typename iterator_reference<Iterator1>::type
                            >::type
                        >::type,
                        typename mpl::if_c<
                            is_const<
                                typename remove_reference<
                                    typename iterator_reference<Iterator1>::type
                                >::type
                            >::value
                            || is_const<
                                typename remove_reference<
                                    typename iterator_reference<Iterator2>::type
                                >::type
                            >::value,
                            typename add_const<
                                typename iterator_reference<Iterator1>::type
                            >::type,
                            typename iterator_reference<Iterator1>::type
                        >::type
                    >::type
       , typename Traversal = typename demote_iterator_traversal_tag<
                                  typename iterator_traversal<Iterator1>::type
                                , typename iterator_traversal<Iterator2>::type>::type
>
class join_iterator
    : public iterator_facade<join_iterator<Iterator1,Iterator2,ValueType,Reference,Traversal>, ValueType, Traversal, Reference>
{
    typedef join_iterator_link<Iterator1, Iterator2> link_t;
    typedef join_iterator_union<Iterator1, Iterator2, Reference> iterator_union;
public:
    typedef Iterator1 iterator1_t;
    typedef Iterator2 iterator2_t;

    join_iterator()
        : m_section(0u)
        , m_it(0u, iterator1_t(), iterator2_t())
        , m_link(link_t(iterator1_t(), iterator2_t()))
    {}

    join_iterator(unsigned int section, Iterator1 current1, Iterator1 last1, Iterator2 first2, Iterator2 current2)
        : m_section(section)
        , m_it(section, current1, current2)
        , m_link(link_t(last1, first2))
        {
        }

    template<typename Range1, typename Range2>
    join_iterator(Range1& r1, Range2& r2, join_iterator_begin_tag)
        : m_section(autoboost::empty(r1) ? 1u : 0u)
        , m_it(autoboost::empty(r1) ? 1u : 0u, autoboost::begin(r1), autoboost::begin(r2))
        , m_link(link_t(autoboost::end(r1), autoboost::begin(r2)))
    {
    }

    template<typename Range1, typename Range2>
    join_iterator(const Range1& r1, const Range2& r2, join_iterator_begin_tag)
        : m_section(autoboost::empty(r1) ? 1u : 0u)
        , m_it(autoboost::empty(r1) ? 1u : 0u, autoboost::const_begin(r1), autoboost::const_begin(r2))
        , m_link(link_t(autoboost::const_end(r1), autoboost::const_begin(r2)))
    {
    }

    template<typename Range1, typename Range2>
    join_iterator(Range1& r1, Range2& r2, join_iterator_end_tag)
        : m_section(1u)
        , m_it(1u, autoboost::end(r1), autoboost::end(r2))
        , m_link(link_t(autoboost::end(r1), autoboost::begin(r2)))
    {
    }

    template<typename Range1, typename Range2>
    join_iterator(const Range1& r1, const Range2& r2, join_iterator_end_tag)
        : m_section(1u)
        , m_it(1u, autoboost::const_end(r1), autoboost::const_end(r2))
        , m_link(link_t(autoboost::const_end(r1), autoboost::const_begin(r2)))
    {
    }

private:
    void increment()
    {
        if (m_section)
            ++m_it.it2();
        else
        {
            ++m_it.it1();
            if (m_it.it1() == m_link.last1)
            {
                m_it.it2() = m_link.first2;
                m_section = 1u;
            }
        }
    }

    void decrement()
    {
        if (m_section)
        {
            if (m_it.it2() == m_link.first2)
            {
                m_it.it1() = autoboost::prior(m_link.last1);
                m_section = 0u;
            }
            else
                --m_it.it2();
        }
        else
            --m_it.it1();
    }

    typename join_iterator::reference dereference() const
    {
        return m_it.dereference(m_section);
    }

    bool equal(const join_iterator& other) const
    {
        return m_section == other.m_section
            && m_it.equal(other.m_it, m_section);
    }

    void advance(typename join_iterator::difference_type offset)
    {
        if (m_section)
            advance_from_range2(offset);
        else
            advance_from_range1(offset);
    }

    typename join_iterator::difference_type distance_to(const join_iterator& other) const
    {
        typename join_iterator::difference_type result;
        if (m_section)
        {
            if (other.m_section)
                result = other.m_it.it2() - m_it.it2();
            else
            {
                result = (m_link.first2 - m_it.it2())
                       + (other.m_it.it1() - m_link.last1);

                AUTOBOOST_ASSERT( result <= 0 );
            }
        }
        else
        {
            if (other.m_section)
            {
                result = (m_link.last1 - m_it.it1())
                       + (other.m_it.it2() - m_link.first2);
            }
            else
                result = other.m_it.it1() - m_it.it1();
        }
        return result;
    }

    void advance_from_range2(typename join_iterator::difference_type offset)
    {
        typedef typename join_iterator::difference_type difference_t;
        AUTOBOOST_ASSERT( m_section == 1u );
        if (offset < 0)
        {
            difference_t r2_dist = m_link.first2 - m_it.it2();
            AUTOBOOST_ASSERT( r2_dist <= 0 );
            if (offset >= r2_dist)
                std::advance(m_it.it2(), offset);
            else
            {
                difference_t r1_dist = offset - r2_dist;
                AUTOBOOST_ASSERT( r1_dist <= 0 );
                m_it.it1() = m_link.last1 + r1_dist;
                m_section = 0u;
            }
        }
        else
            std::advance(m_it.it2(), offset);
    }

    void advance_from_range1(typename join_iterator::difference_type offset)
    {
        typedef typename join_iterator::difference_type difference_t;
        AUTOBOOST_ASSERT( m_section == 0u );
        if (offset > 0)
        {
            difference_t r1_dist = m_link.last1 - m_it.it1();
            AUTOBOOST_ASSERT( r1_dist >= 0 );
            if (offset < r1_dist)
                std::advance(m_it.it1(), offset);
            else
            {
                difference_t r2_dist = offset - r1_dist;
                AUTOBOOST_ASSERT( r2_dist >= 0 );
                m_it.it2() = m_link.first2 + r2_dist;
                m_section = 1u;
            }
        }
        else
            std::advance(m_it.it1(), offset);
    }

    unsigned int m_section;
    iterator_union m_it;
    link_t m_link;

    friend class ::autoboost::iterator_core_access;
};

    } // namespace range_detail

} // namespace autoboost

#endif // include guard
