// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AutoConfigBase.h"
#include "autowiring_error.h"
#include "has_validate.h"
#include <string>
#include <sstream>
#include FUNCTIONAL_HEADER
#include MEMORY_HEADER

// Check if 'T' has a valid stream conversion operator
template<typename T>
struct has_stream {
  template<class U>
  static auto select(std::istream* is, U* u) -> decltype(*is >> *u);
  
  template<class U>
  static void select(...);
  
  static const bool value = !std::is_same<void, decltype(select<T>(nullptr, nullptr))>::value;
};

// Get last type in parameter pack
template<typename... Ts>
struct get_last;

template<typename T, typename... Ts>
struct get_last<T, Ts...>{
  typedef typename get_last<Ts...>::last last;
};

template<typename T>
struct get_last<T>{
  typedef T last;
};

// Stores information about an AutoConfigVar type
struct ConfigRegistryEntry {
  ConfigRegistryEntry(const std::type_info& ti, bool has_validator);

  // Next entry in the list:
  const ConfigRegistryEntry* const pFlink;
  
  // Configuration name
  const std::string m_key;
  
  // True if a validator was provided
  const bool m_hasValidator;
  
  // Is this key identify this entry?
  bool is(const std::string& key) const;
  
  // Verify 'ti' is the same type as this entry's value
  virtual bool verifyType(const std::type_info& ti) const = 0;
  
  // Parse a string into this entrie's value type.
  // Type must have operator>> T defined
  virtual AnySharedPointer parse(const std::string&) const = 0;
  
  // Returns function which validates this input. The validator function is
  // defined as KEY::Validate(const T&) where KEY is the type identifing this entry
  virtual std::function<bool(const AnySharedPointer&)> validator(void) const = 0;
};

// Template arguemnts TKey specify the key and optional namespace for a config attribute
template<class... TKey>
struct ConfigTypeExtractor {};

namespace autowiring {
  void ThrowFailedTypeParseException(const std::string& str, const std::type_info& ti);
}

template<class T, class... TKey>
struct ConfigRegistryEntryT:
  public ConfigRegistryEntry
{
  // The "key" proper, without the namespace
  typedef typename get_last<TKey...>::last t_key;
  
  ConfigRegistryEntryT(void):
    ConfigRegistryEntry(typeid(ConfigTypeExtractor<TKey...>), has_validate<t_key>::value)
  {}
  
  bool verifyType(const std::type_info& ti) const override {
    return typeid(T) == ti;
  }

  // Parse string into this ConfigEntry's type. Throw an exception
  // if no such stream operator exists
  AnySharedPointer parse(const std::string& str) const override {
    return AnySharedPointer(std::make_shared<T>(std::move(parseInternal<T>(str))));
  }
  
public:
  // Only use if there is a stream operator
  template<typename U>
  typename std::enable_if<has_stream<U>::value, T>::type
  parseInternal(const std::string& str) const {
    std::istringstream ss(str);
    T val;
    ss >> std::boolalpha >> val;
    
    if (ss.fail())
      autowiring::ThrowFailedTypeParseException(str, typeid(T));
    return val;
  }

  // Throw exception if there is no stream operator
  template<typename U>
  typename std::enable_if<!has_stream<U>::value, T>::type
  parseInternal(const std::string&) const {
    throw autowiring_error("This type doesn't support stream conversions.  Define one if you want this to be parsable");
  };

  std::function<bool(const T&)> validatorInternal(void) {
    return[](const T& ptr) {
      return CallValidate<T, t_key>::Call(ptr);
    };
  }

  std::function<bool(const AnySharedPointer&)> validator(void) const override {
    return [] (const AnySharedPointer& ptr) {
      return CallValidate<T, t_key>::Call(*ptr.as<T>().get());
    };
  }
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
