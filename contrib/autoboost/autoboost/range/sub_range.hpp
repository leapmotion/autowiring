// Boost.Range library
//
//  Copyright Neil Groves 2009.
//  Copyright Thorsten Ottosen 2003-2004. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef AUTOBOOST_RANGE_SUB_RANGE_HPP
#define AUTOBOOST_RANGE_SUB_RANGE_HPP

#include <autoboost/detail/workaround.hpp>

#if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, AUTOBOOST_TESTED_AT(1500))
    #pragma warning( push )
    #pragma warning( disable : 4996 )
#endif

#include <autoboost/range/config.hpp>
#include <autoboost/range/iterator_range.hpp>
#include <autoboost/range/value_type.hpp>
#include <autoboost/range/size_type.hpp>
#include <autoboost/range/difference_type.hpp>
#include <autoboost/range/reference.hpp>
#include <autoboost/range/algorithm/equal.hpp>
#include <autoboost/assert.hpp>
#include <autoboost/type_traits/is_reference.hpp>
#include <autoboost/type_traits/remove_reference.hpp>

namespace autoboost
{
    namespace range_detail
    {

template<class ForwardRange, class TraversalTag>
class sub_range_base
        : public iterator_range<
            AUTOBOOST_DEDUCED_TYPENAME range_iterator<ForwardRange>::type
        >
{
    typedef iterator_range<
        AUTOBOOST_DEDUCED_TYPENAME range_iterator<ForwardRange>::type
    > base;

protected:
    typedef AUTOBOOST_DEDUCED_TYPENAME base::iterator_range_ iterator_range_;

public:
    typedef AUTOBOOST_DEDUCED_TYPENAME range_value<ForwardRange>::type value_type;
    typedef AUTOBOOST_DEDUCED_TYPENAME range_iterator<ForwardRange>::type iterator;
    typedef AUTOBOOST_DEDUCED_TYPENAME range_iterator<const ForwardRange>::type const_iterator;
    typedef AUTOBOOST_DEDUCED_TYPENAME range_difference<ForwardRange>::type difference_type;
    typedef AUTOBOOST_DEDUCED_TYPENAME range_size<ForwardRange>::type size_type;
    typedef AUTOBOOST_DEDUCED_TYPENAME range_reference<ForwardRange>::type reference;
    typedef AUTOBOOST_DEDUCED_TYPENAME range_reference<const ForwardRange>::type const_reference;

    sub_range_base()
    {
    }

    template<class Iterator>
    sub_range_base(Iterator first, Iterator last)
        : base(first, last)
    {
    }

    reference front()
    {
        return base::front();
    }

    const_reference front() const
    {
        return base::front();
    }
};

template<class ForwardRange>
class sub_range_base<ForwardRange, bidirectional_traversal_tag>
        : public sub_range_base<ForwardRange, forward_traversal_tag>
{
    typedef sub_range_base<ForwardRange, forward_traversal_tag> base;
public:
    sub_range_base()
    {
    }

    template<class Iterator>
    sub_range_base(Iterator first, Iterator last)
        : base(first, last)
    {
    }

    AUTOBOOST_DEDUCED_TYPENAME base::reference back()
    {
        return base::back();
    }

    AUTOBOOST_DEDUCED_TYPENAME base::const_reference back() const
    {
        return base::back();
    }
};

template<class ForwardRange>
class sub_range_base<ForwardRange, random_access_traversal_tag>
        : public sub_range_base<ForwardRange, bidirectional_traversal_tag>
{
    typedef sub_range_base<ForwardRange, bidirectional_traversal_tag> base;

public:
    sub_range_base()
    {
    }

    template<class Iterator>
    sub_range_base(Iterator first, Iterator last)
        : base(first, last)
    {
    }

    AUTOBOOST_DEDUCED_TYPENAME base::reference
    operator[](AUTOBOOST_DEDUCED_TYPENAME base::difference_type n)
    {
        return this->begin()[n];
    }

    AUTOBOOST_DEDUCED_TYPENAME base::const_reference
    operator[](AUTOBOOST_DEDUCED_TYPENAME base::difference_type n) const
    {
        return this->begin()[n];
    }
};

    } // namespace range_detail

    template<class ForwardRange>
    class sub_range
        : public range_detail::sub_range_base<
                ForwardRange,
                AUTOBOOST_DEDUCED_TYPENAME iterator_traversal<
                    AUTOBOOST_DEDUCED_TYPENAME range_iterator<ForwardRange>::type
                >::type
            >
    {
        typedef AUTOBOOST_DEDUCED_TYPENAME range_iterator<
            ForwardRange
        >::type iterator_t;

        typedef range_detail::sub_range_base<
            ForwardRange,
            AUTOBOOST_DEDUCED_TYPENAME iterator_traversal<
                AUTOBOOST_DEDUCED_TYPENAME range_iterator<ForwardRange>::type
            >::type
        > base;

        typedef AUTOBOOST_DEDUCED_TYPENAME base::impl impl;

    protected:
        typedef AUTOBOOST_DEDUCED_TYPENAME base::iterator_range_ iterator_range_;

    private:
        template<class Source>
        struct is_compatible_range
            : is_convertible<
                AUTOBOOST_DEDUCED_TYPENAME mpl::eval_if<
                    has_range_iterator<Source>,
                    range_iterator<Source>,
                    mpl::identity<void>
                >::type,
                AUTOBOOST_DEDUCED_TYPENAME base::iterator
            >
        {
        };

    public:
        sub_range()
        { }

#if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, AUTOBOOST_TESTED_AT(1500) )
        sub_range(const sub_range& r)
            : base(impl::adl_begin(static_cast<const base&>(r)),
                   impl::adl_end(static_cast<const base&>(r)))
        { }
#endif

        template< class ForwardRange2 >
        sub_range(
            ForwardRange2& r,
            AUTOBOOST_DEDUCED_TYPENAME ::autoboost::enable_if<
                is_compatible_range<ForwardRange2>
            >::type* = 0
        )
        : base(impl::adl_begin(r), impl::adl_end(r))
        {
        }

        template< class ForwardRange2 >
        sub_range(
            const ForwardRange2& r,
            AUTOBOOST_DEDUCED_TYPENAME ::autoboost::enable_if<
                is_compatible_range<const ForwardRange2>
            >::type* = 0
        )
        : base(impl::adl_begin(r), impl::adl_end(r))
        {
        }

        AUTOBOOST_DEDUCED_TYPENAME base::const_iterator begin() const
        {
            return base::begin();
        }

        AUTOBOOST_DEDUCED_TYPENAME base::iterator begin()
        {
            return base::begin();
        }

        AUTOBOOST_DEDUCED_TYPENAME base::const_iterator end() const
        {
            return base::end();
        }

        AUTOBOOST_DEDUCED_TYPENAME base::iterator end()
        {
            return base::end();
        }

        template< class Iter >
        sub_range( Iter first, Iter last ) :
            base( first, last )
        { }

        template<class ForwardRange2>
        AUTOBOOST_DEDUCED_TYPENAME ::autoboost::enable_if<
            is_compatible_range<ForwardRange2>,
            sub_range&
        >::type
        operator=(ForwardRange2& r)
        {
            iterator_range_::operator=( r );
            return *this;
        }

        template<class ForwardRange2>
        AUTOBOOST_DEDUCED_TYPENAME ::autoboost::enable_if<
            is_compatible_range<const ForwardRange2>,
            sub_range&
        >::type
        operator=( const ForwardRange2& r )
        {
            iterator_range_::operator=( r );
            return *this;
        }

        sub_range& operator=( const sub_range& r )
        {
            iterator_range_::operator=( static_cast<const iterator_range_&>(r) );
            return *this;
        }

        sub_range& advance_begin(
            AUTOBOOST_DEDUCED_TYPENAME base::difference_type n)
        {
            std::advance(this->m_Begin, n);
            return *this;
        }

        sub_range& advance_end(
            AUTOBOOST_DEDUCED_TYPENAME base::difference_type n)
        {
            std::advance(this->m_End, n);
            return *this;
        }
    };

} // namespace 'autoboost'

#if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, AUTOBOOST_TESTED_AT(1500))
    #pragma warning( pop )
#endif

#endif

