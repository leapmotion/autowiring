/*
 * Copyright (c) 2014 Glen Joseph Fernandes
 * glenfe at live dot com
 *
 * Distributed under the Boost Software License,
 * Version 1.0. (See accompanying file LICENSE_1_0.txt
 * or copy at http://autoboost.org/LICENSE_1_0.txt)
 */
#ifndef AUTOBOOST_SMART_PTR_MAKE_UNIQUE_OBJECT_HPP
#define AUTOBOOST_SMART_PTR_MAKE_UNIQUE_OBJECT_HPP

#include <autoboost/config.hpp>
#include <autoboost/smart_ptr/detail/up_if_not_array.hpp>
#include <autoboost/type_traits/add_rvalue_reference.hpp>
#include <utility>

namespace autoboost {
    template<class T>
    inline typename autoboost::detail::up_if_not_array<T>::type
    make_unique() {
        return std::unique_ptr<T>(new T());
    }

#if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
    template<class T, class... Args>
    inline typename autoboost::detail::up_if_not_array<T>::type
    make_unique(Args&&... args) {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
#endif

    template<class T>
    inline typename autoboost::detail::up_if_not_array<T>::type
    make_unique(typename add_rvalue_reference<T>::type value) {
        return std::unique_ptr<T>(new T(std::move(value)));
    }

    template<class T>
    inline typename autoboost::detail::up_if_not_array<T>::type
    make_unique_noinit() {
        return std::unique_ptr<T>(new T);
    }
}

#endif
