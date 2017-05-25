/*!
@file
Includes all the library components except the adapters for external
libraries.

@copyright Louis Dionne 2013-2017
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://autoboost.org/LICENSE_1_0.txt)
 */

#ifndef AUTOBOOST_HANA_HPP
#define AUTOBOOST_HANA_HPP

//! @defgroup group-concepts Concepts
//! Concepts provided by the library.

//! @defgroup group-datatypes Data types
//! General purpose data types provided by the library.

//! @defgroup group-functional Functional
//! General purpose function objects.

//! @defgroup group-core Core
//! Core utilities of the library.

//! @defgroup group-experimental Experimental features
//! Experimental features that may or may not make it into the library.
//! These features should not expected to be stable.

//! @defgroup group-ext External adapters
//! Adapters for external libraries.

//! @defgroup group-config Configuration options
//! Configurable options to tweak the global behavior of the library.

//! @defgroup group-assertions Assertions
//! Macros to perform different kinds of assertions.

//! @defgroup group-details Details
//! Implementation details. Do not rely on anything here, even if it is
//! documented.

// Include config.hpp first, so that warning messages about compiler support
// appear as soon as possible.
#include <autoboost/hana/config.hpp>


#ifdef AUTOBOOST_HANA_DOXYGEN_INVOKED
namespace autoboost {
    //! Namespace containing everything in the library.
    namespace hana {
        //! Namespace containing C++14 user-defined literals provided by Hana.
        namespace literals {}
    }
}
#endif

#include <autoboost/hana/accessors.hpp>
#include <autoboost/hana/adapt_adt.hpp>
#include <autoboost/hana/adapt_struct.hpp>
#include <autoboost/hana/adjust.hpp>
#include <autoboost/hana/adjust_if.hpp>
#include <autoboost/hana/all.hpp>
#include <autoboost/hana/all_of.hpp>
#include <autoboost/hana/and.hpp>
#include <autoboost/hana/any.hpp>
#include <autoboost/hana/any_of.hpp>
#include <autoboost/hana/ap.hpp>
#include <autoboost/hana/append.hpp>
#include <autoboost/hana/assert.hpp>
#include <autoboost/hana/at.hpp>
#include <autoboost/hana/at_key.hpp>
#include <autoboost/hana/back.hpp>
#include <autoboost/hana/basic_tuple.hpp>
#include <autoboost/hana/bool.hpp>
#include <autoboost/hana/cartesian_product.hpp>
#include <autoboost/hana/chain.hpp>
#include <autoboost/hana/comparing.hpp>
#include <autoboost/hana/concat.hpp>
#include <autoboost/hana/concept.hpp>
#include <autoboost/hana/contains.hpp>
#include <autoboost/hana/core.hpp>
#include <autoboost/hana/count.hpp>
#include <autoboost/hana/count_if.hpp>
#include <autoboost/hana/cycle.hpp>
#include <autoboost/hana/define_struct.hpp>
#include <autoboost/hana/difference.hpp>
#include <autoboost/hana/div.hpp>
#include <autoboost/hana/drop_back.hpp>
#include <autoboost/hana/drop_front.hpp>
#include <autoboost/hana/drop_front_exactly.hpp>
#include <autoboost/hana/drop_while.hpp>
#include <autoboost/hana/duplicate.hpp>
#include <autoboost/hana/empty.hpp>
#include <autoboost/hana/equal.hpp>
#include <autoboost/hana/erase_key.hpp>
#include <autoboost/hana/eval.hpp>
#include <autoboost/hana/eval_if.hpp>
#include <autoboost/hana/extend.hpp>
#include <autoboost/hana/extract.hpp>
#include <autoboost/hana/fill.hpp>
#include <autoboost/hana/filter.hpp>
#include <autoboost/hana/find.hpp>
#include <autoboost/hana/find_if.hpp>
#include <autoboost/hana/first.hpp>
#include <autoboost/hana/flatten.hpp>
#include <autoboost/hana/fold.hpp>
#include <autoboost/hana/fold_left.hpp>
#include <autoboost/hana/fold_right.hpp>
#include <autoboost/hana/for_each.hpp>
#include <autoboost/hana/front.hpp>
#include <autoboost/hana/functional.hpp>
#include <autoboost/hana/fuse.hpp>
#include <autoboost/hana/greater.hpp>
#include <autoboost/hana/greater_equal.hpp>
#include <autoboost/hana/group.hpp>
#include <autoboost/hana/if.hpp>
#include <autoboost/hana/insert.hpp>
#include <autoboost/hana/insert_range.hpp>
#include <autoboost/hana/integral_constant.hpp>
#include <autoboost/hana/intersection.hpp>
#include <autoboost/hana/intersperse.hpp>
#include <autoboost/hana/is_disjoint.hpp>
#include <autoboost/hana/is_empty.hpp>
#include <autoboost/hana/is_subset.hpp>
#include <autoboost/hana/keys.hpp>
#include <autoboost/hana/lazy.hpp>
#include <autoboost/hana/length.hpp>
#include <autoboost/hana/less.hpp>
#include <autoboost/hana/less_equal.hpp>
#include <autoboost/hana/lexicographical_compare.hpp>
#include <autoboost/hana/lift.hpp>
#include <autoboost/hana/map.hpp>
#include <autoboost/hana/max.hpp>
#include <autoboost/hana/maximum.hpp>
#include <autoboost/hana/members.hpp>
#include <autoboost/hana/min.hpp>
#include <autoboost/hana/minimum.hpp>
#include <autoboost/hana/minus.hpp>
#include <autoboost/hana/mod.hpp>
#include <autoboost/hana/monadic_compose.hpp>
#include <autoboost/hana/monadic_fold_left.hpp>
#include <autoboost/hana/monadic_fold_right.hpp>
#include <autoboost/hana/mult.hpp>
#include <autoboost/hana/negate.hpp>
#include <autoboost/hana/none.hpp>
#include <autoboost/hana/none_of.hpp>
#include <autoboost/hana/not.hpp>
#include <autoboost/hana/not_equal.hpp>
#include <autoboost/hana/one.hpp>
#include <autoboost/hana/optional.hpp>
#include <autoboost/hana/or.hpp>
#include <autoboost/hana/ordering.hpp>
#include <autoboost/hana/pair.hpp>
#include <autoboost/hana/partition.hpp>
#include <autoboost/hana/permutations.hpp>
#include <autoboost/hana/plus.hpp>
#include <autoboost/hana/power.hpp>
#include <autoboost/hana/prefix.hpp>
#include <autoboost/hana/prepend.hpp>
#include <autoboost/hana/product.hpp>
#include <autoboost/hana/range.hpp>
#include <autoboost/hana/remove.hpp>
#include <autoboost/hana/remove_at.hpp>
#include <autoboost/hana/remove_if.hpp>
#include <autoboost/hana/remove_range.hpp>
#include <autoboost/hana/repeat.hpp>
#include <autoboost/hana/replace.hpp>
#include <autoboost/hana/replace_if.hpp>
#include <autoboost/hana/replicate.hpp>
#include <autoboost/hana/reverse.hpp>
#include <autoboost/hana/reverse_fold.hpp>
#include <autoboost/hana/scan_left.hpp>
#include <autoboost/hana/scan_right.hpp>
#include <autoboost/hana/second.hpp>
#include <autoboost/hana/set.hpp>
#include <autoboost/hana/size.hpp>
#include <autoboost/hana/slice.hpp>
#include <autoboost/hana/sort.hpp>
#include <autoboost/hana/span.hpp>
#include <autoboost/hana/string.hpp>
#include <autoboost/hana/suffix.hpp>
#include <autoboost/hana/sum.hpp>
#include <autoboost/hana/symmetric_difference.hpp>
#include <autoboost/hana/take_back.hpp>
#include <autoboost/hana/take_front.hpp>
#include <autoboost/hana/take_while.hpp>
#include <autoboost/hana/tap.hpp>
#include <autoboost/hana/then.hpp>
#include <autoboost/hana/traits.hpp>
#include <autoboost/hana/transform.hpp>
#include <autoboost/hana/tuple.hpp>
#include <autoboost/hana/type.hpp>
#include <autoboost/hana/unfold_left.hpp>
#include <autoboost/hana/unfold_right.hpp>
#include <autoboost/hana/union.hpp>
#include <autoboost/hana/unique.hpp>
#include <autoboost/hana/unpack.hpp>
#include <autoboost/hana/value.hpp>
#include <autoboost/hana/version.hpp>
#include <autoboost/hana/while.hpp>
#include <autoboost/hana/zero.hpp>
#include <autoboost/hana/zip.hpp>
#include <autoboost/hana/zip_shortest.hpp>
#include <autoboost/hana/zip_shortest_with.hpp>
#include <autoboost/hana/zip_with.hpp>

#endif // !AUTOBOOST_HANA_HPP
