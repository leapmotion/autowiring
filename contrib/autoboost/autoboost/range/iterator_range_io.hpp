// Boost.Range library
//
//  Copyright Neil Groves 2009.
//  Use, modification and distribution is subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef AUTOBOOST_RANGE_ITERATOR_RANGE_IO_HPP_INCLUDED
#define AUTOBOOST_RANGE_ITERATOR_RANGE_IO_HPP_INCLUDED

#include <autoboost/config.hpp>
#include <autoboost/detail/workaround.hpp>

#if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, AUTOBOOST_TESTED_AT(1500))
    #pragma warning( push )
    #pragma warning( disable : 4996 )
#endif

// From autoboost/dynamic_bitset.hpp; thanks to Matthias Troyer for Cray X1 patch.
#ifndef AUTOBOOST_OLD_IOSTREAMS
# if defined(__STL_CONFIG_H) && \
    !defined (__STL_USE_NEW_IOSTREAMS) && !defined(__crayx1) \
    /**/
#  define AUTOBOOST_OLD_IOSTREAMS
# endif
#endif // #ifndef AUTOBOOST_OLD_IOSTREAMS

#ifndef _STLP_NO_IOSTREAMS
# ifndef AUTOBOOST_OLD_IOSTREAMS
#  include <ostream>
# else
#  include <ostream.h>
# endif
#endif // _STLP_NO_IOSTREAMS

#include <autoboost/range/iterator_range_core.hpp>
#include <iterator>
#include <algorithm>
#include <cstddef>

namespace autoboost
{

#ifndef _STLP_NO_IOSTREAMS
# ifndef AUTOBOOST_OLD_IOSTREAMS

        //! iterator_range output operator
        /*!
            Output the range to an ostream. Elements are outputted
            in a sequence without separators.
        */
        template< typename IteratorT, typename Elem, typename Traits >
        inline std::basic_ostream<Elem,Traits>& operator<<(
                    std::basic_ostream<Elem, Traits>& Os,
                    const iterator_range<IteratorT>& r )
        {
            std::copy( r.begin(), r.end(),
                       std::ostream_iterator< AUTOBOOST_DEDUCED_TYPENAME
                                              iterator_value<IteratorT>::type,
                                              Elem, Traits>(Os) );
            return Os;
        }

# else

        //! iterator_range output operator
        /*!
            Output the range to an ostream. Elements are outputted
            in a sequence without separators.
        */
        template< typename IteratorT >
        inline std::ostream& operator<<(
                    std::ostream& Os,
                    const iterator_range<IteratorT>& r )
        {
            std::copy( r.begin(), r.end(), std::ostream_iterator<char>(Os));
            return Os;
        }

# endif
#endif // _STLP_NO_IOSTREAMS

} // namespace autoboost

#undef AUTOBOOST_OLD_IOSTREAMS

#if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, AUTOBOOST_TESTED_AT(1500))
    #pragma warning(pop)
#endif

#endif // include guard
