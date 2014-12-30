/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga  2014-2014
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_INTRUSIVE_DETAIL_NODE_HOLDER_HPP
#define AUTOBOOST_INTRUSIVE_DETAIL_NODE_HOLDER_HPP

#if defined(_MSC_VER)
#  pragma once
#endif

namespace autoboost {
namespace intrusive {

template<class Node, class Tag, unsigned int>
struct node_holder
   :  public Node
{};

}  //namespace intrusive{
}  //namespace autoboost{

#endif //AUTOBOOST_INTRUSIVE_DETAIL_NODE_HOLDER_HPP
