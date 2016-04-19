//Copyright (c) 2006-2008 Emil Dotchevski and Reverge Studios, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_316FDA946C0D11DEA9CBAE5255D89593
#define UUID_316FDA946C0D11DEA9CBAE5255D89593
#if (__GNUC__*100+__GNUC_MINOR__>301) && !defined(AUTOBOOST_EXCEPTION_ENABLE_WARNINGS)
#pragma GCC system_header
#endif
#if defined(_MSC_VER) && !defined(AUTOBOOST_EXCEPTION_ENABLE_WARNINGS)
#pragma warning(push,1)
#endif

#include <autoboost/exception/diagnostic_information.hpp>
#include <autoboost/exception/error_info.hpp>
#include <autoboost/exception/exception.hpp>
#include <autoboost/exception/get_error_info.hpp>
#include <autoboost/exception/info.hpp>
#include <autoboost/exception/info_tuple.hpp>
#include <autoboost/exception/errinfo_api_function.hpp>
#include <autoboost/exception/errinfo_at_line.hpp>
#include <autoboost/exception/errinfo_errno.hpp>
#include <autoboost/exception/errinfo_file_handle.hpp>
#include <autoboost/exception/errinfo_file_name.hpp>
#include <autoboost/exception/errinfo_file_open_mode.hpp>
#include <autoboost/exception/errinfo_type_info_name.hpp>
#ifndef AUTOBOOST_NO_EXCEPTIONS
#include <autoboost/exception/errinfo_nested_exception.hpp>
#include <autoboost/exception_ptr.hpp>
#endif

#if defined(_MSC_VER) && !defined(AUTOBOOST_EXCEPTION_ENABLE_WARNINGS)
#pragma warning(pop)
#endif
#endif
