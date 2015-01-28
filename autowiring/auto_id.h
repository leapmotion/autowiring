// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include TYPE_TRAITS_HEADER
#include TYPE_INDEX_HEADER

/// <summary>
/// Identifier sigil structure
/// </summary>
/// <remarks>
/// This structure is used to avoid attempts to obtain the typeid of an incomplete type.
/// It's only used in cases where typeid(T) would have been used alone.
/// </remarks>
template<class T>
struct auto_id {
  
  // Return this type_info for this type with 'const' and 'volitile' removed
  static const std::type_info& key(void) {
    return typeid(auto_id<typename std::remove_cv<T>::type>);
  }
};
