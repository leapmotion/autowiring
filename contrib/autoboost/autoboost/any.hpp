// See http://www.boost.org/libs/any for Documentation.

#ifndef AUTOBOOST_ANY_INCLUDED
#define AUTOBOOST_ANY_INCLUDED

#if defined(_MSC_VER)
# pragma once
#endif

// what:  variant type autoboost::any
// who:   contributed by Kevlin Henney,
//        with features contributed and bugs found by
//        Antony Polukhin, Ed Brey, Mark Rodgers,
//        Peter Dimov, and James Curran
// when:  July 2001, April 2013 - May 2013

#include <algorithm>

#include "autoboost/config.hpp"
#include <autoboost/type_index.hpp>
#include <autoboost/type_traits/remove_reference.hpp>
#include <autoboost/type_traits/decay.hpp>
#include <autoboost/type_traits/remove_cv.hpp>
#include <autoboost/type_traits/add_reference.hpp>
#include <autoboost/type_traits/is_reference.hpp>
#include <autoboost/type_traits/is_const.hpp>
#include <autoboost/throw_exception.hpp>
#include <autoboost/static_assert.hpp>
#include <autoboost/utility/enable_if.hpp>
#include <autoboost/type_traits/is_same.hpp>
#include <autoboost/type_traits/is_const.hpp>
#include <autoboost/mpl/if.hpp>

namespace autoboost
{
    class any
    {
    public: // structors

        any() AUTOBOOST_NOEXCEPT
          : content(0)
        {
        }

        template<typename ValueType>
        any(const ValueType & value)
          : content(new holder<
                AUTOBOOST_DEDUCED_TYPENAME remove_cv<AUTOBOOST_DEDUCED_TYPENAME decay<const ValueType>::type>::type
            >(value))
        {
        }

        any(const any & other)
          : content(other.content ? other.content->clone() : 0)
        {
        }

#ifndef AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
        // Move constructor
        any(any&& other) AUTOBOOST_NOEXCEPT
          : content(other.content)
        {
            other.content = 0;
        }

        // Perfect forwarding of ValueType
        template<typename ValueType>
        any(ValueType&& value
            , typename autoboost::disable_if<autoboost::is_same<any&, ValueType> >::type* = 0 // disable if value has type `any&`
            , typename autoboost::disable_if<autoboost::is_const<ValueType> >::type* = 0) // disable if value has type `const ValueType&&`
          : content(new holder< typename decay<ValueType>::type >(static_cast<ValueType&&>(value)))
        {
        }
#endif

        ~any() AUTOBOOST_NOEXCEPT
        {
            delete content;
        }

    public: // modifiers

        any & swap(any & rhs) AUTOBOOST_NOEXCEPT
        {
            std::swap(content, rhs.content);
            return *this;
        }


#ifdef AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
        template<typename ValueType>
        any & operator=(const ValueType & rhs)
        {
            any(rhs).swap(*this);
            return *this;
        }

        any & operator=(any rhs)
        {
            any(rhs).swap(*this);
            return *this;
        }

#else
        any & operator=(const any& rhs)
        {
            any(rhs).swap(*this);
            return *this;
        }

        // move assignement
        any & operator=(any&& rhs) AUTOBOOST_NOEXCEPT
        {
            rhs.swap(*this);
            any().swap(rhs);
            return *this;
        }

        // Perfect forwarding of ValueType
        template <class ValueType>
        any & operator=(ValueType&& rhs)
        {
            any(static_cast<ValueType&&>(rhs)).swap(*this);
            return *this;
        }
#endif

    public: // queries

        bool empty() const AUTOBOOST_NOEXCEPT
        {
            return !content;
        }

        void clear() AUTOBOOST_NOEXCEPT
        {
            any().swap(*this);
        }

        const autoboost::typeindex::type_info& type() const AUTOBOOST_NOEXCEPT
        {
            return content ? content->type() : autoboost::typeindex::type_id<void>().type_info();
        }

#ifndef AUTOBOOST_NO_MEMBER_TEMPLATE_FRIENDS
    private: // types
#else
    public: // types (public so any_cast can be non-friend)
#endif

        class placeholder
        {
        public: // structors

            virtual ~placeholder()
            {
            }

        public: // queries

            virtual const autoboost::typeindex::type_info& type() const AUTOBOOST_NOEXCEPT = 0;

            virtual placeholder * clone() const = 0;

        };

        template<typename ValueType>
        class holder : public placeholder
        {
        public: // structors

            holder(const ValueType & value)
              : held(value)
            {
            }

#ifndef AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
            holder(ValueType&& value)
              : held(static_cast< ValueType&& >(value))
            {
            }
#endif
        public: // queries

            virtual const autoboost::typeindex::type_info& type() const AUTOBOOST_NOEXCEPT
            {
                return autoboost::typeindex::type_id<ValueType>().type_info();
            }

            virtual placeholder * clone() const
            {
                return new holder(held);
            }

        public: // representation

            ValueType held;

        private: // intentionally left unimplemented
            holder & operator=(const holder &);
        };

#ifndef AUTOBOOST_NO_MEMBER_TEMPLATE_FRIENDS

    private: // representation

        template<typename ValueType>
        friend ValueType * any_cast(any *) AUTOBOOST_NOEXCEPT;

        template<typename ValueType>
        friend ValueType * unsafe_any_cast(any *) AUTOBOOST_NOEXCEPT;

#else

    public: // representation (public so any_cast can be non-friend)

#endif

        placeholder * content;

    };

    inline void swap(any & lhs, any & rhs) AUTOBOOST_NOEXCEPT
    {
        lhs.swap(rhs);
    }

    class AUTOBOOST_SYMBOL_VISIBLE bad_any_cast :
#ifndef AUTOBOOST_NO_RTTI
        public std::bad_cast
#else
        public std::exception
#endif
    {
    public:
        virtual const char * what() const AUTOBOOST_NOEXCEPT_OR_NOTHROW
        {
            return "autoboost::bad_any_cast: "
                   "failed conversion using autoboost::any_cast";
        }
    };

    template<typename ValueType>
    ValueType * any_cast(any * operand) AUTOBOOST_NOEXCEPT
    {
        return operand && operand->type() == autoboost::typeindex::type_id<ValueType>()
            ? &static_cast<any::holder<AUTOBOOST_DEDUCED_TYPENAME remove_cv<ValueType>::type> *>(operand->content)->held
            : 0;
    }

    template<typename ValueType>
    inline const ValueType * any_cast(const any * operand) AUTOBOOST_NOEXCEPT
    {
        return any_cast<ValueType>(const_cast<any *>(operand));
    }

    template<typename ValueType>
    ValueType any_cast(any & operand)
    {
        typedef AUTOBOOST_DEDUCED_TYPENAME remove_reference<ValueType>::type nonref;


        nonref * result = any_cast<nonref>(&operand);
        if(!result)
            autoboost::throw_exception(bad_any_cast());

        // Attempt to avoid construction of a temporary object in cases when
        // `ValueType` is not a reference. Example:
        // `static_cast<std::string>(*result);`
        // which is equal to `std::string(*result);`
        typedef AUTOBOOST_DEDUCED_TYPENAME autoboost::mpl::if_<
            autoboost::is_reference<ValueType>,
            ValueType,
            AUTOBOOST_DEDUCED_TYPENAME autoboost::add_reference<ValueType>::type
        >::type ref_type;

        return static_cast<ref_type>(*result);
    }

    template<typename ValueType>
    inline ValueType any_cast(const any & operand)
    {
        typedef AUTOBOOST_DEDUCED_TYPENAME remove_reference<ValueType>::type nonref;
        return any_cast<const nonref &>(const_cast<any &>(operand));
    }

#ifndef AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
    template<typename ValueType>
    inline ValueType any_cast(any&& operand)
    {
        AUTOBOOST_STATIC_ASSERT_MSG(
            autoboost::is_rvalue_reference<ValueType&&>::value /*true if ValueType is rvalue or just a value*/
            || autoboost::is_const< typename autoboost::remove_reference<ValueType>::type >::value,
            "autoboost::any_cast shall not be used for getting nonconst references to temporary objects"
        );
        return any_cast<ValueType>(operand);
    }
#endif


    // Note: The "unsafe" versions of any_cast are not part of the
    // public interface and may be removed at any time. They are
    // required where we know what type is stored in the any and can't
    // use typeid() comparison, e.g., when our types may travel across
    // different shared libraries.
    template<typename ValueType>
    inline ValueType * unsafe_any_cast(any * operand) AUTOBOOST_NOEXCEPT
    {
        return &static_cast<any::holder<ValueType> *>(operand->content)->held;
    }

    template<typename ValueType>
    inline const ValueType * unsafe_any_cast(const any * operand) AUTOBOOST_NOEXCEPT
    {
        return unsafe_any_cast<ValueType>(const_cast<any *>(operand));
    }
}

// Copyright Kevlin Henney, 2000, 2001, 2002. All rights reserved.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#endif
