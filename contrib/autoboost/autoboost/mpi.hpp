// Copyright (C) 2006 Douglas Gregor <doug.gregor -at- gmail.com>.

// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// Message Passing Interface

//  See www.boost.org/libs/mpi for documentation.

/** @file mpi.hpp
 *
 *  This file is a top-level convenience header that includes all of
 *  the Boost.MPI library headers. Users concerned about compile time
 *  may wish to include only specific headers from the Boost.MPI
 *  library.
 *
 */
#ifndef AUTOBOOST_MPI_HPP
#define AUTOBOOST_MPI_HPP

#include <autoboost/mpi/allocator.hpp>
#include <autoboost/mpi/collectives.hpp>
#include <autoboost/mpi/communicator.hpp>
#include <autoboost/mpi/datatype.hpp>
#include <autoboost/mpi/environment.hpp>
#include <autoboost/mpi/graph_communicator.hpp>
#include <autoboost/mpi/group.hpp>
#include <autoboost/mpi/intercommunicator.hpp>
#include <autoboost/mpi/nonblocking.hpp>
#include <autoboost/mpi/operations.hpp>
#include <autoboost/mpi/skeleton_and_content.hpp>
#include <autoboost/mpi/timer.hpp>

#endif // AUTOBOOST_MPI_HPP
