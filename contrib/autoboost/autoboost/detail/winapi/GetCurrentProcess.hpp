//  GetCurrentProcess.hpp  --------------------------------------------------------------//

//  Copyright 2010 Vicente J. Botet Escriba
//  Copyright 2015 Andrey Semashev

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_DETAIL_WINAPI_GETCURRENTPROCESS_HPP
#define AUTOBOOST_DETAIL_WINAPI_GETCURRENTPROCESS_HPP

#include <autoboost/detail/winapi/get_current_process.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if defined(__GNUC__) && (((__GNUC__*100)+__GNUC_MINOR__) > 403)
#pragma message "This header is deprecated, use autoboost/detail/winapi/get_current_process.hpp instead."
#elif defined(_MSC_VER)
#pragma message("This header is deprecated, use autoboost/detail/winapi/get_current_process.hpp instead.")
#endif

#endif // AUTOBOOST_DETAIL_WINAPI_GETCURRENTPROCESS_HPP
