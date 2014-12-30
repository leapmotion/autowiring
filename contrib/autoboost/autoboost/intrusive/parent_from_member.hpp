/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga  2010-2013
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////
#ifndef AUTOBOOST_INTRUSIVE_PARENT_FROM_MEMBER_HPP
#define AUTOBOOST_INTRUSIVE_PARENT_FROM_MEMBER_HPP

#if defined(_MSC_VER)
#  pragma once
#endif

#include <autoboost/intrusive/detail/config_begin.hpp>
#include <autoboost/intrusive/intrusive_fwd.hpp>

#include <autoboost/intrusive/detail/parent_from_member.hpp>

namespace autoboost {
namespace intrusive {

//! Given a pointer to a member and its corresponding pointer to data member,
//! this function returns the pointer of the parent containing that member.
//! Note: this function does not work with pointer to members that rely on
//! virtual inheritance.
template<class Parent, class Member>
inline Parent *get_parent_from_member(Member *member, const Member Parent::* ptr_to_member)
{  return ::autoboost::intrusive::detail::parent_from_member(member, ptr_to_member);  }

//! Given a const pointer to a member and its corresponding const pointer to data member,
//! this function returns the const pointer of the parent containing that member.
//! Note: this function does not work with pointer to members that rely on
//! virtual inheritance.
template<class Parent, class Member>
inline const Parent *get_parent_from_member(const Member *member, const Member Parent::* ptr_to_member)
{  return ::autoboost::intrusive::detail::parent_from_member(member, ptr_to_member);  }

}  //namespace intrusive {
}  //namespace autoboost {

#include <autoboost/intrusive/detail/config_end.hpp>

#endif   //#ifndef AUTOBOOST_INTRUSIVE_PARENT_FROM_MEMBER_HPP
