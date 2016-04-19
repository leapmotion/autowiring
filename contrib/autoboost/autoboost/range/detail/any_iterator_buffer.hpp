// Boost.Range library
//
//  Copyright Neil Groves 2010. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef AUTOBOOST_RANGE_DETAIL_ANY_ITERATOR_BUFFER_HPP_INCLUDED
#define AUTOBOOST_RANGE_DETAIL_ANY_ITERATOR_BUFFER_HPP_INCLUDED

#include <autoboost/array.hpp>
#include <autoboost/assert.hpp>
#include <autoboost/static_assert.hpp>
#include <autoboost/noncopyable.hpp>

namespace autoboost
{
    template<std::size_t StackBufferSize>
    class any_iterator_buffer
        : noncopyable
    {
        AUTOBOOST_STATIC_ASSERT(( StackBufferSize > 0 ));
    public:
        any_iterator_buffer()
            : m_ptr()
        {
        }

        ~any_iterator_buffer()
        {
            delete [] m_ptr;
        }

        void* allocate(std::size_t bytes)
        {
            AUTOBOOST_ASSERT( !m_ptr );
            if (bytes <= StackBufferSize)
                return m_buffer.data();

            m_ptr = new char[bytes];
            return m_ptr;
        }

        void deallocate()
        {
            delete [] m_ptr;
            m_ptr = 0;
        }

    private:
        // Rationale:
        // Do not use inheritance from noncopyable because this causes
        // the concepts to erroneous detect the derived any_iterator
        // as noncopyable.
        any_iterator_buffer(const any_iterator_buffer&);
        void operator=(const any_iterator_buffer&);

        char* m_ptr;
        autoboost::array<char, StackBufferSize> m_buffer;
    };

    class any_iterator_heap_only_buffer
        : noncopyable
    {
    public:
        any_iterator_heap_only_buffer()
            : m_ptr()
        {
        }

        ~any_iterator_heap_only_buffer()
        {
            delete [] m_ptr;
        }

        void* allocate(std::size_t bytes)
        {
            AUTOBOOST_ASSERT( !m_ptr );
            m_ptr = new char[bytes];
            return m_ptr;
        }

        void deallocate()
        {
            delete [] m_ptr;
            m_ptr = 0;
        }

    private:
        char* m_ptr;
    };

    template<std::size_t StackBufferSize>
    class any_iterator_stack_only_buffer
    {
        AUTOBOOST_STATIC_ASSERT(( StackBufferSize > 0 ));
    public:
        void* allocate(std::size_t bytes)
        {
            AUTOBOOST_ASSERT( bytes <= m_buffer.size() );
            return m_buffer.data();
        }

        void deallocate()
        {
        }

    private:
        autoboost::array<char, StackBufferSize> m_buffer;
    };

    typedef any_iterator_buffer<64> any_iterator_default_buffer;
} // namespace autoboost

#endif // include guard
