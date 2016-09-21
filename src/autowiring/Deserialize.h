// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#pragma once
#include TYPE_TRAITS_HEADER
#include <string>

/// <summary>
/// Support class for deserializing byte-streams input as std::string into a variety of formats
/// </summary>
namespace autowiring {
  struct SerializableSigil{};

  struct Serialize:
    public SerializableSigil
  {
    virtual std::string AutoSerialize(void) = 0;
    virtual void AutoDeserialize(std::string data) = 0;
  };

  template <typename T, typename Tag = T>
  struct deser;

  template<typename T>
  struct deser<T> {
    /// <summary>
    /// const std::string * Deserialization
    /// </summary>
    template <class X = T>
    static typename std::enable_if<std::is_same<const std::string *, X>::value, const std::string *>::type
    deserialize(std::string & str){
      return &str;
    }

    /// <summary>
    /// Deserialization for types that do that thingAutoDeserialize
    /// </summary>
    template <class X = T>
    static typename std::enable_if<std::is_base_of<Serialize, X>::value, T>::type
    deserialize(std::string & str){
      T t;
      t.AutoDeserialize(str);
      return t;
    }
  };
}
