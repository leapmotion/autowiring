//---------------------------------------------------------------------------//
// Copyright (c) 2013 Kyle Lutz <kyle.r.lutz@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See http://autoboostorg.github.com/compute for more information.
//---------------------------------------------------------------------------//

#ifndef AUTOBOOST_COMPUTE_HPP
#define AUTOBOOST_COMPUTE_HPP

#include <autoboost/compute/algorithm.hpp>
#include <autoboost/compute/allocator.hpp>
#include <autoboost/compute/async.hpp>
#include <autoboost/compute/buffer.hpp>
#include <autoboost/compute/cl.hpp>
#include <autoboost/compute/command_queue.hpp>
#include <autoboost/compute/config.hpp>
#include <autoboost/compute/container.hpp>
#include <autoboost/compute/context.hpp>
#include <autoboost/compute/device.hpp>
#include <autoboost/compute/functional.hpp>
#include <autoboost/compute/image.hpp>
#include <autoboost/compute/iterator.hpp>
#include <autoboost/compute/kernel.hpp>
#include <autoboost/compute/lambda.hpp>
#include <autoboost/compute/pipe.hpp>
#include <autoboost/compute/platform.hpp>
#include <autoboost/compute/program.hpp>
#include <autoboost/compute/random.hpp>
#include <autoboost/compute/svm.hpp>
#include <autoboost/compute/system.hpp>
#include <autoboost/compute/types.hpp>
#include <autoboost/compute/user_event.hpp>
#include <autoboost/compute/utility.hpp>
#include <autoboost/compute/version.hpp>

#ifdef AUTOBOOST_COMPUTE_HAVE_HDR_CL_EXT
#include <autoboost/compute/cl_ext.hpp>
#endif

#endif // AUTOBOOST_COMPUTE_HPP
