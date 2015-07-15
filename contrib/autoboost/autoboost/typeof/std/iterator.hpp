// Copyright (C) 2005 Arkadiy Vertleyb, Peder Holt.
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_TYPEOF_STD_iterator_hpp_INCLUDED
#define AUTOBOOST_TYPEOF_STD_iterator_hpp_INCLUDED

#include <iterator>
#include <autoboost/typeof/typeof.hpp>
#include <autoboost/typeof/std/string.hpp>

#include AUTOBOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()

AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::iterator_traits, 1)
#if AUTOBOOST_WORKAROUND(AUTOBOOST_DINKUMWARE_STDLIB, == 1)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::iterator, 2)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::iterator, 3)
#else
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::iterator, 2)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::iterator, 3)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::iterator, 4)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::iterator, 5)
#endif//AUTOBOOST_WORKAROUND(AUTOBOOST_DINKUMWARE_STDLIB, == 1)
AUTOBOOST_TYPEOF_REGISTER_TYPE(std::input_iterator_tag)
AUTOBOOST_TYPEOF_REGISTER_TYPE(std::output_iterator_tag)
AUTOBOOST_TYPEOF_REGISTER_TYPE(std::forward_iterator_tag)
AUTOBOOST_TYPEOF_REGISTER_TYPE(std::bidirectional_iterator_tag)
AUTOBOOST_TYPEOF_REGISTER_TYPE(std::random_access_iterator_tag)
#if AUTOBOOST_WORKAROUND(AUTOBOOST_DINKUMWARE_STDLIB, == 1)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::reverse_iterator, 2)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::reverse_iterator, 3)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::reverse_iterator, 4)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::reverse_iterator, 5)
#else
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::reverse_iterator, 1)
#endif//AUTOBOOST_WORKAROUND(AUTOBOOST_DINKUMWARE_STDLIB, == 1)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::back_insert_iterator, 1)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::front_insert_iterator, 1)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::insert_iterator, 1)
#if AUTOBOOST_WORKAROUND(AUTOBOOST_DINKUMWARE_STDLIB, == 1)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::istream_iterator, 1)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::istream_iterator, 2)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::istream_iterator, 3)
#else
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::istream_iterator, 1)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::istream_iterator, 2)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::istream_iterator, 3)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::istream_iterator, 4)
#endif//AUTOBOOST_WORKAROUND(AUTOBOOST_DINKUMWARE_STDLIB, == 1)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::ostream_iterator, 1)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::ostream_iterator, 2)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::ostream_iterator, 3)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::istreambuf_iterator, 1)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::istreambuf_iterator, 2)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::ostreambuf_iterator, 1)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::ostreambuf_iterator, 2)

#endif//AUTOBOOST_TYPEOF_STD_iterator_hpp_INCLUDED