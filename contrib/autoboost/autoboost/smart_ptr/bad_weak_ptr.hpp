#ifndef AUTOBOOST_SMART_PTR_BAD_WEAK_PTR_HPP_INCLUDED
#define AUTOBOOST_SMART_PTR_BAD_WEAK_PTR_HPP_INCLUDED

// MS compatible compilers support #pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//
//  autoboost/smart_ptr/bad_weak_ptr.hpp
//
//  Copyright (c) 2001, 2002, 2003 Peter Dimov and Multi Media Ltd.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <exception>

#ifdef __BORLANDC__
# pragma warn -8026     // Functions with excep. spec. are not expanded inline
#endif

namespace autoboost
{

// The standard library that comes with Borland C++ 5.5.1, 5.6.4
// defines std::exception and its members as having C calling
// convention (-pc). When the definition of bad_weak_ptr
// is compiled with -ps, the compiler issues an error.
// Hence, the temporary #pragma option -pc below.

#if defined(__BORLANDC__) && __BORLANDC__ <= 0x564
# pragma option push -pc
#endif

#if defined(__clang__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wweak-vtables"
#endif

class bad_weak_ptr: public std::exception
{
public:

    virtual char const * what() const throw()
    {
        return "tr1::bad_weak_ptr";
    }
};

#if defined(__clang__)
# pragma clang diagnostic pop
#endif

#if defined(__BORLANDC__) && __BORLANDC__ <= 0x564
# pragma option pop
#endif

} // namespace autoboost

#ifdef __BORLANDC__
# pragma warn .8026     // Functions with excep. spec. are not expanded inline
#endif

#endif  // #ifndef AUTOBOOST_SMART_PTR_BAD_WEAK_PTR_HPP_INCLUDED
