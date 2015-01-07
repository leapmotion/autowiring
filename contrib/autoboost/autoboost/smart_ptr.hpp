#ifndef AUTOBOOST_SMART_PTR_HPP_INCLUDED
#define AUTOBOOST_SMART_PTR_HPP_INCLUDED

//
//  smart_ptr.hpp
//
//  For convenience, this header includes the rest of the smart
//  pointer library headers.
//
//  Copyright (c) 2003 Peter Dimov  Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//  http://www.boost.org/libs/smart_ptr/smart_ptr.htm
//

#include <autoboost/config.hpp>

#include <autoboost/scoped_ptr.hpp>
#include <autoboost/scoped_array.hpp>
#include <autoboost/shared_ptr.hpp>
#include <autoboost/shared_array.hpp>

#if !defined(AUTOBOOST_NO_MEMBER_TEMPLATES) || defined(AUTOBOOST_MSVC6_MEMBER_TEMPLATES)
# include <autoboost/weak_ptr.hpp>
# include <autoboost/intrusive_ptr.hpp>
# include <autoboost/enable_shared_from_this.hpp>
# include <autoboost/make_shared.hpp>
#endif

#endif // #ifndef AUTOBOOST_SMART_PTR_HPP_INCLUDED
