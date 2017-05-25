///////////////////////////////////////////////////////////////////////////////
/// \file algorithm.hpp
///   Includes the range-based versions of the algorithms in the
///   C++ standard header file <algorithm>
//
/////////////////////////////////////////////////////////////////////////////

// Copyright 2009 Neil Groves.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// Acknowledgements:
// This code uses combinations of ideas, techniques and code snippets
// from: Thorsten Ottosen, Eric Niebler, Jeremy Siek,
// and Vladimir Prus'
//
// The original mutating algorithms that served as the first version
// were originally written by Vladimir Prus'
// <ghost@cs.msu.su> code from Boost Wiki

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef AUTOBOOST_RANGE_ALGORITHM_HPP_INCLUDED_01012009
#define AUTOBOOST_RANGE_ALGORITHM_HPP_INCLUDED_01012009

#include <autoboost/range/concepts.hpp>
#include <autoboost/range/iterator_range.hpp>
#include <autoboost/range/difference_type.hpp>
#include <autoboost/range/detail/range_return.hpp>
#include <autoboost/iterator/iterator_traits.hpp>
#include <autoboost/next_prior.hpp>
#include <algorithm>

// Non-mutating algorithms
#include <autoboost/range/algorithm/adjacent_find.hpp>
#include <autoboost/range/algorithm/count.hpp>
#include <autoboost/range/algorithm/count_if.hpp>
#include <autoboost/range/algorithm/equal.hpp>
#include <autoboost/range/algorithm/for_each.hpp>
#include <autoboost/range/algorithm/find.hpp>
#include <autoboost/range/algorithm/find_end.hpp>
#include <autoboost/range/algorithm/find_first_of.hpp>
#include <autoboost/range/algorithm/find_if.hpp>
#include <autoboost/range/algorithm/lexicographical_compare.hpp>
#include <autoboost/range/algorithm/mismatch.hpp>
#include <autoboost/range/algorithm/search.hpp>
#include <autoboost/range/algorithm/search_n.hpp>

// Mutating algorithms
#include <autoboost/range/algorithm/copy.hpp>
#include <autoboost/range/algorithm/copy_backward.hpp>
#include <autoboost/range/algorithm/fill.hpp>
#include <autoboost/range/algorithm/fill_n.hpp>
#include <autoboost/range/algorithm/generate.hpp>
#include <autoboost/range/algorithm/inplace_merge.hpp>
#include <autoboost/range/algorithm/merge.hpp>
#include <autoboost/range/algorithm/nth_element.hpp>
#include <autoboost/range/algorithm/partial_sort.hpp>
#include <autoboost/range/algorithm/partial_sort_copy.hpp>
#include <autoboost/range/algorithm/partition.hpp>
#include <autoboost/range/algorithm/random_shuffle.hpp>
#include <autoboost/range/algorithm/remove.hpp>
#include <autoboost/range/algorithm/remove_copy.hpp>
#include <autoboost/range/algorithm/remove_copy_if.hpp>
#include <autoboost/range/algorithm/remove_if.hpp>
#include <autoboost/range/algorithm/replace.hpp>
#include <autoboost/range/algorithm/replace_copy.hpp>
#include <autoboost/range/algorithm/replace_copy_if.hpp>
#include <autoboost/range/algorithm/replace_if.hpp>
#include <autoboost/range/algorithm/reverse.hpp>
#include <autoboost/range/algorithm/reverse_copy.hpp>
#include <autoboost/range/algorithm/rotate.hpp>
#include <autoboost/range/algorithm/rotate_copy.hpp>
#include <autoboost/range/algorithm/sort.hpp>
#include <autoboost/range/algorithm/stable_partition.hpp>
#include <autoboost/range/algorithm/stable_sort.hpp>
#include <autoboost/range/algorithm/transform.hpp>
#include <autoboost/range/algorithm/unique.hpp>
#include <autoboost/range/algorithm/unique_copy.hpp>

// Binary search
#include <autoboost/range/algorithm/binary_search.hpp>
#include <autoboost/range/algorithm/equal_range.hpp>
#include <autoboost/range/algorithm/lower_bound.hpp>
#include <autoboost/range/algorithm/upper_bound.hpp>

// Set operations of sorted ranges
#include <autoboost/range/algorithm/set_algorithm.hpp>

// Heap operations
#include <autoboost/range/algorithm/heap_algorithm.hpp>

// Minimum and Maximum
#include <autoboost/range/algorithm/max_element.hpp>
#include <autoboost/range/algorithm/min_element.hpp>

// Permutations
#include <autoboost/range/algorithm/permutation.hpp>

#endif // include guard

