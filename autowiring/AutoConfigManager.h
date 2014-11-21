// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include <string>
#include STL_UNORDERED_MAP

struct AnySharedPointer;

class AutoConfigManager {
  
private:
  std::unordered_map<std::string, AnySharedPointer> m_attributes;

public:
  /// <summary>
  /// Assigns the specified value to an AnySharedPointer slot
  /// </summary>
  /// <remarks>
  /// This method will throw an exception if the specified name cannot be found as a configurable value
  /// in the application, or if the specified value type does not match the type expected by this field
  /// </remarks>
  template<class T>
  void Set(const char* name, const T& value) {}

  /// <summary>
  /// Coerces the string representation of the specified field to the correct value type
  /// </summary>
  /// <remarks>
  /// This method will throw an exception if there is no string converter available on this type
  /// </remarks>
  void SetParsed(const char* name, const char* value) {}
};