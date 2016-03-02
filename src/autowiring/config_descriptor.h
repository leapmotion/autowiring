// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AnySharedPointer.h"
#include "has_getconfigdescriptor.h"
#include "marshaller.h"
#include <initializer_list>
#include <stdexcept>
#include <unordered_map>

namespace autowiring {
  template<typename T>
  struct marshaller;

  template<typename T>
  static const marshaller<T>& get_marshaller(void) {
    // To fix this, specialize autowiring::marshal for your type
    static_assert(
      !std::is_base_of<invalid_marshal_base, autowiring::marshaller<T>>::value,
      "Autowiring does not provide a built-in marshaller for type T"
    );

    // To fix this, ensure your specialization implements marshal_base
    static_assert(
      std::is_base_of<autowiring::marshaller_base, autowiring::marshaller<T>> ::value,
      "Marshaller for type T does not implement the marshal_base interface"
    );

    static const marshaller<T> sc_marshaller{};
    return sc_marshaller;
  }

  struct config_field {
    config_field(void) = default;

    template<typename T, typename U, typename V>
    config_field(const char* name, const char* desc, U T::* memptr, V&& default_value) :
      name(name),
      description(desc),
      offset(reinterpret_cast<size_t>(&(static_cast<T*>(nullptr)->*memptr))),
      default_value(std::make_shared<typename std::remove_cv<U>::type>()),
      marshaller{ &get_marshaller<U>() }
    {}

    template<typename T, typename U>
    config_field(const char* name, U T::* memptr) :
      name(name),
      offset(reinterpret_cast<size_t>(&(static_cast<T*>(nullptr)->*memptr))),
      marshaller{ &get_marshaller<U>() },
      default_value(std::make_shared<typename std::remove_cv<U>::type>())
    {}

    // Name and optional description
    const char* name = nullptr;
    const char* description = nullptr;

    // Offset from the base of the object to the member to be serialized
    size_t offset = 0;

    // Default value for this type, if one exists:
    AnySharedPointer default_value;

    // Pointer to the required marshaller singleton:
    const marshaller_base* marshaller = nullptr;
  };

  struct string_hash {
    size_t operator()(const char* p) const {
      size_t rv = 5381;
      for (; *p; p++)
        rv += (rv << 5) + *p;
      return rv;
    }

    bool operator()(const char* lhs, const char* rhs) const {
      return !strcmp(lhs, rhs);
    }
  };

  struct config_descriptor {
    typedef std::unordered_map<const char*, config_field, string_hash, string_hash> t_mpType;

    static t_mpType MakeFields(const std::initializer_list<config_field>& fields);

    config_descriptor(std::initializer_list<config_field> fields) :
      fields(MakeFields(fields))
    {}

    const t_mpType fields;
  };
}
