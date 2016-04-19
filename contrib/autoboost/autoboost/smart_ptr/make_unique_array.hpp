/*
 * Copyright (c) 2014 Glen Joseph Fernandes
 * glenfe at live dot com
 *
 * Distributed under the Boost Software License,
 * Version 1.0. (See accompanying file LICENSE_1_0.txt
 * or copy at http://autoboost.org/LICENSE_1_0.txt)
 */
#ifndef AUTOBOOST_SMART_PTR_MAKE_UNIQUE_ARRAY_HPP
#define AUTOBOOST_SMART_PTR_MAKE_UNIQUE_ARRAY_HPP

#include <autoboost/smart_ptr/detail/up_if_array.hpp>
#include <autoboost/smart_ptr/detail/array_traits.hpp>

namespace autoboost {
    template<class T>
    inline typename autoboost::detail::up_if_array<T>::type
    make_unique(std::size_t size) {
        typedef typename autoboost::detail::array_inner<T>::type U;
        return std::unique_ptr<T>(new U[size]());
    }

    template<class T>
    inline typename autoboost::detail::up_if_array<T>::type
    make_unique_noinit(std::size_t size) {
        typedef typename autoboost::detail::array_inner<T>::type U;
        return std::unique_ptr<T>(new U[size]);
    }
}

#endif
