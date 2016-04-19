// Boost.Range library
//
//  Copyright Thorsten Ottosen 2003-2004. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef AUTOBOOST_RANGE_DETAIL_COMMON_HPP
#define AUTOBOOST_RANGE_DETAIL_COMMON_HPP

#if defined(_MSC_VER)
# pragma once
#endif

#include <autoboost/range/config.hpp>
#include <autoboost/range/detail/sfinae.hpp>
#include <autoboost/type_traits/is_void.hpp>
#include <autoboost/mpl/bool.hpp>
#include <autoboost/mpl/if.hpp>
#include <autoboost/mpl/int.hpp>
#include <autoboost/mpl/or.hpp>
#include <cstddef>

//////////////////////////////////////////////////////////////////////////////
// missing partial specialization  workaround.
//////////////////////////////////////////////////////////////////////////////

namespace autoboost
{
    namespace range_detail
    {
        // 1 = std containers
        // 2 = std::pair
        // 3 = const std::pair
        // 4 = array
        // 5 = const array
        // 6 = char array
        // 7 = wchar_t array
        // 8 = char*
        // 9 = const char*
        // 10 = whar_t*
        // 11 = const wchar_t*
        // 12 = string

        typedef mpl::int_<1>::type    std_container_;
        typedef mpl::int_<2>::type    std_pair_;
        typedef mpl::int_<3>::type    const_std_pair_;
        typedef mpl::int_<4>::type    array_;
        typedef mpl::int_<5>::type    const_array_;
        typedef mpl::int_<6>::type    char_array_;
        typedef mpl::int_<7>::type    wchar_t_array_;
        typedef mpl::int_<8>::type    char_ptr_;
        typedef mpl::int_<9>::type    const_char_ptr_;
        typedef mpl::int_<10>::type   wchar_t_ptr_;
        typedef mpl::int_<11>::type   const_wchar_t_ptr_;
        typedef mpl::int_<12>::type   string_;

        template< typename C >
        struct range_helper
        {
            static C* c;
            static C  ptr;

            AUTOBOOST_STATIC_CONSTANT( bool, is_pair_                = sizeof( autoboost::range_detail::is_pair_impl( c ) ) == sizeof( yes_type ) );
            AUTOBOOST_STATIC_CONSTANT( bool, is_char_ptr_            = sizeof( autoboost::range_detail::is_char_ptr_impl( ptr ) ) == sizeof( yes_type ) );
            AUTOBOOST_STATIC_CONSTANT( bool, is_const_char_ptr_      = sizeof( autoboost::range_detail::is_const_char_ptr_impl( ptr ) ) == sizeof( yes_type ) );
            AUTOBOOST_STATIC_CONSTANT( bool, is_wchar_t_ptr_         = sizeof( autoboost::range_detail::is_wchar_t_ptr_impl( ptr ) ) == sizeof( yes_type ) );
            AUTOBOOST_STATIC_CONSTANT( bool, is_const_wchar_t_ptr_   = sizeof( autoboost::range_detail::is_const_wchar_t_ptr_impl( ptr ) ) == sizeof( yes_type ) );
            AUTOBOOST_STATIC_CONSTANT( bool, is_char_array_          = sizeof( autoboost::range_detail::is_char_array_impl( ptr ) ) == sizeof( yes_type ) );
            AUTOBOOST_STATIC_CONSTANT( bool, is_wchar_t_array_       = sizeof( autoboost::range_detail::is_wchar_t_array_impl( ptr ) ) == sizeof( yes_type ) );
            AUTOBOOST_STATIC_CONSTANT( bool, is_string_              = (autoboost::mpl::or_<autoboost::mpl::bool_<is_const_char_ptr_>, autoboost::mpl::bool_<is_const_wchar_t_ptr_> >::value ));
            AUTOBOOST_STATIC_CONSTANT( bool, is_array_               = autoboost::is_array<C>::value );

        };

        template< typename C >
        class range
        {
            typedef AUTOBOOST_RANGE_DEDUCED_TYPENAME   autoboost::mpl::if_c< ::autoboost::range_detail::range_helper<C>::is_pair_,
                                                                  autoboost::range_detail::std_pair_,
                                                                  void >::type pair_t;
            typedef AUTOBOOST_RANGE_DEDUCED_TYPENAME   autoboost::mpl::if_c< ::autoboost::range_detail::range_helper<C>::is_array_,
                                                                    autoboost::range_detail::array_,
                                                                    pair_t >::type array_t;
            typedef AUTOBOOST_RANGE_DEDUCED_TYPENAME   autoboost::mpl::if_c< ::autoboost::range_detail::range_helper<C>::is_string_,
                                                                    autoboost::range_detail::string_,
                                                                    array_t >::type string_t;
            typedef AUTOBOOST_RANGE_DEDUCED_TYPENAME   autoboost::mpl::if_c< ::autoboost::range_detail::range_helper<C>::is_const_char_ptr_,
                                                                    autoboost::range_detail::const_char_ptr_,
                                                                    string_t >::type const_char_ptr_t;
            typedef AUTOBOOST_RANGE_DEDUCED_TYPENAME   autoboost::mpl::if_c< ::autoboost::range_detail::range_helper<C>::is_char_ptr_,
                                                                    autoboost::range_detail::char_ptr_,
                                                                    const_char_ptr_t >::type char_ptr_t;
            typedef AUTOBOOST_RANGE_DEDUCED_TYPENAME   autoboost::mpl::if_c< ::autoboost::range_detail::range_helper<C>::is_const_wchar_t_ptr_,
                                                                    autoboost::range_detail::const_wchar_t_ptr_,
                                                                    char_ptr_t >::type const_wchar_ptr_t;
            typedef AUTOBOOST_RANGE_DEDUCED_TYPENAME   autoboost::mpl::if_c< ::autoboost::range_detail::range_helper<C>::is_wchar_t_ptr_,
                                                                    autoboost::range_detail::wchar_t_ptr_,
                                                                    const_wchar_ptr_t >::type wchar_ptr_t;
            typedef AUTOBOOST_RANGE_DEDUCED_TYPENAME   autoboost::mpl::if_c< ::autoboost::range_detail::range_helper<C>::is_wchar_t_array_,
                                                                    autoboost::range_detail::wchar_t_array_,
                                                                    wchar_ptr_t >::type wchar_array_t;
            typedef AUTOBOOST_RANGE_DEDUCED_TYPENAME   autoboost::mpl::if_c< ::autoboost::range_detail::range_helper<C>::is_char_array_,
                                                                    autoboost::range_detail::char_array_,
                                                                    wchar_array_t >::type char_array_t;
        public:
            typedef AUTOBOOST_RANGE_DEDUCED_TYPENAME   autoboost::mpl::if_c< ::autoboost::is_void<char_array_t>::value,
                                                                    autoboost::range_detail::std_container_,
                                                                    char_array_t >::type type;
        }; // class 'range'
    }
}

#endif

