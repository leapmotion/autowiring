// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include MEMORY_HEADER
#include <string>
#include <sstream>
#include "AnySharedPointer.h"
#include "autowiring_error.h"
#include "demangle.h"

// Check if 'T' has a valid stream conversion operator
template<typename T>
struct has_stream {
  template<class U>
  static auto select(std::istream* is, U* u) -> decltype(*is >> *u);
  
  template<class U>
  static void select(...);
  
  static const bool value = !std::is_same<void, decltype(select<T>(nullptr, nullptr))>::value;
};

struct ConfigRegistryEntry {
  ConfigRegistryEntry(const std::type_info& ti);

  // Next entry in the list:
  const ConfigRegistryEntry* const pFlink;
  
  // Configuration name
  const std::string m_key;
  
  bool is(const std::string& key) const;
  
  virtual bool verifyType(const std::type_info& ti) const = 0;
  
  virtual AnySharedPointer parse(const std::string&) const = 0;
};

// Template arguemnts TKey specify the key and optional namespace for a config attribute
template<class... TKey>
struct ConfigTypeExtractor {};

template<class T, class... TKey>
struct ConfigRegistryEntryT:
  public ConfigRegistryEntry
{
  ConfigRegistryEntryT(void):
    ConfigRegistryEntry(typeid(ConfigTypeExtractor<TKey...>))
  {}
  
  bool verifyType(const std::type_info& ti) const override {
    return typeid(T) == ti;
  }

  // Parse string into this ConfigEntry's type. Throw an exception
  // if no such stream operator exists
  AnySharedPointer parse(const std::string& str) const override {
    return parseInternal<T>(str);
  }
  
  // Only use if there is a stream operator
  template<typename U>
  typename std::enable_if<has_stream<U>::value, AnySharedPointer>::type
  parseInternal(const std::string& str) const {
    std::istringstream ss(str);
    T val;
    ss >> std::boolalpha >> val;
    
    if (ss.fail()) {
      std::stringstream msg;
      msg << "Failed to parse '" << str << "' as type '"
          << autowiring::demangle(typeid(T)) << "'";
      throw autowiring_error(msg.str());
    }
    return AnySharedPointer(std::make_shared<T>(val));
  }

  // Throw exception if there is no stream operator
  template<typename U>
  typename std::enable_if<!has_stream<U>::value, AnySharedPointer>::type
  parseInternal(const std::string&) const {
    throw autowiring_error("This type doesn't support stream conversions.\
                           Define one if you want this to be parsable");
  };
};

extern const ConfigRegistryEntry* g_pFirstConfigEntry;
extern size_t g_confgiEntryCount;

/// <summary>
/// Adds the specified type to the universal type registry
/// </summary>
/// <remarks>
/// Any instance of this type registry parameterized on type T will be added to the
/// global static type registry, and this registry is computed at link time.
/// </remarks>
template<class T, class... TKey>
class RegConfig
{
public:
  static const ConfigRegistryEntryT<T, TKey...> r;
};

template<class T, class... TKey>
const ConfigRegistryEntryT<T, TKey...> RegConfig<T, TKey...>::r;
