/*
Copyright 2014-2015 Glen Joseph Fernandes
(glenjofe@gmail.com)

Distributed under the Boost Software License, Version 1.0.
(http://www.boost.org/LICENSE_1_0.txt)
*/
#ifndef AUTOBOOST_ALIGN_ALIGN_HPP
#define AUTOBOOST_ALIGN_ALIGN_HPP

#include <autoboost/config.hpp>

#if !defined(AUTOBOOST_NO_CXX11_STD_ALIGN)
#include <autoboost/align/detail/align_cxx11.hpp>
#else
#include <autoboost/align/detail/align.hpp>
#endif

#endif
