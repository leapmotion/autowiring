// Boost.Function library

//  Copyright Douglas Gregor 2001-2003. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org/libs/function

// William Kempf, Jesse Jones and Karl Nelson were all very helpful in the
// design of this library.

#include <functional> // unary_function, binary_function

#include <autoboost/preprocessor/iterate.hpp>
#include <autoboost/detail/workaround.hpp>

#ifndef AUTOBOOST_FUNCTION_MAX_ARGS
#  define AUTOBOOST_FUNCTION_MAX_ARGS 10
#endif // AUTOBOOST_FUNCTION_MAX_ARGS

// Include the prologue here so that the use of file-level iteration
// in anything that may be included by function_template.hpp doesn't break
#include <autoboost/function/detail/prologue.hpp>

// Older Visual Age C++ version do not handle the file iteration well
#if AUTOBOOST_WORKAROUND(__IBMCPP__, >= 500) && AUTOBOOST_WORKAROUND(__IBMCPP__, < 800)
#  if AUTOBOOST_FUNCTION_MAX_ARGS >= 0
#    include <autoboost/function/function0.hpp>
#  endif
#  if AUTOBOOST_FUNCTION_MAX_ARGS >= 1
#    include <autoboost/function/function1.hpp>
#  endif
#  if AUTOBOOST_FUNCTION_MAX_ARGS >= 2
#    include <autoboost/function/function2.hpp>
#  endif
#  if AUTOBOOST_FUNCTION_MAX_ARGS >= 3
#    include <autoboost/function/function3.hpp>
#  endif
#  if AUTOBOOST_FUNCTION_MAX_ARGS >= 4
#    include <autoboost/function/function4.hpp>
#  endif
#  if AUTOBOOST_FUNCTION_MAX_ARGS >= 5
#    include <autoboost/function/function5.hpp>
#  endif
#  if AUTOBOOST_FUNCTION_MAX_ARGS >= 6
#    include <autoboost/function/function6.hpp>
#  endif
#  if AUTOBOOST_FUNCTION_MAX_ARGS >= 7
#    include <autoboost/function/function7.hpp>
#  endif
#  if AUTOBOOST_FUNCTION_MAX_ARGS >= 8
#    include <autoboost/function/function8.hpp>
#  endif
#  if AUTOBOOST_FUNCTION_MAX_ARGS >= 9
#    include <autoboost/function/function9.hpp>
#  endif
#  if AUTOBOOST_FUNCTION_MAX_ARGS >= 10
#    include <autoboost/function/function10.hpp>
#  endif
#else
// What is the '3' for?
#  define AUTOBOOST_PP_ITERATION_PARAMS_1 (3,(0,AUTOBOOST_FUNCTION_MAX_ARGS,<autoboost/function/detail/function_iterate.hpp>))
#  include AUTOBOOST_PP_ITERATE()
#  undef AUTOBOOST_PP_ITERATION_PARAMS_1
#endif
