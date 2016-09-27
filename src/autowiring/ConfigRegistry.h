// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#pragma once
#include "config_descriptor.h"
#include <atomic>

namespace autowiring {
  struct config_registry_entry_base {
  protected:
    config_registry_entry_base(config_descriptor&& descriptor);

  public:
    // Next entry in the registry:
    const config_registry_entry_base* const pFlink = nullptr;

    // Descriptor proper:
    const config_descriptor descriptor;
  };

  struct config_registry_entry_default {
    config_registry_entry_default(void);

    config_descriptor descriptor;

    static const config_registry_entry_default entry;
  };

  template<typename T, typename = void>
  struct config_registry_entry :
    config_registry_entry_default
  {};

  template<typename T>
  struct config_registry_entry<T, typename std::enable_if<has_getconfigdescriptor<T>::value>::type> :
    config_registry_entry_base
  {
  private:
    config_registry_entry(void) :
      config_registry_entry_base(T::GetConfigDescriptor())
    {}

  public:
    static const config_registry_entry entry;
  };

  template<typename T>
  const config_registry_entry<T, typename std::enable_if<has_getconfigdescriptor<T>::value>::type>
    config_registry_entry<T, typename std::enable_if<has_getconfigdescriptor<T>::value>::type>::entry;

  struct config_registry {
    /// <summary>
    /// Standard iterator type
    /// </summary>
    class const_iterator {
    public:
      typedef std::forward_iterator_tag iterator_category;
      typedef std::ptrdiff_t difference_type;
      typedef config_registry_entry_base value_type;
      typedef const config_descriptor* pointer;
      typedef const config_descriptor& reference;

      const_iterator(const config_registry_entry_base* entry = nullptr) :
        m_cur(entry)
      {}

    private:
      const config_registry_entry_base* m_cur;

    public:
      pointer operator->(void) const { return &m_cur->descriptor; }
      reference operator*(void) { return m_cur->descriptor; }

      bool operator==(const const_iterator& rhs) const { return m_cur == rhs.m_cur; }
      bool operator!=(const const_iterator& rhs) const { return m_cur != rhs.m_cur; }

      // Iterator operator overloads:
      const_iterator& operator++(void) {
        m_cur = m_cur->pFlink;
        return *this;
      }
      const_iterator operator++(int) {
        auto prior = m_cur;
        ++*this;
        return{ prior };
      }
    };

  private:
    // Application linked list head
    static config_registry_entry_base* g_pFirstEntry;

  public:
    /// <summary>
    /// Adds a link to the specified registry entry
    /// </summary>
    /// <remarks>
    /// This routine is not thread safe, and must be externally synchronized
    /// </remarks>
    static const config_registry_entry_base* Link(config_registry_entry_base& entry);

    // Iterator support logic:
    const_iterator begin(void) const { return{ g_pFirstEntry }; }
    const_iterator end(void) const { return{ nullptr }; }
  };

  /// <summary>
  /// Gets a string representation of the named configuration value
  /// </summary>
  template<typename T>
  std::string ConfigGet(const char* name, T& obj) {
    const config_descriptor& desc = config_registry_entry<T>::entry.descriptor;
    auto q = desc.fields.find(name);
    if (q == desc.fields.end())
      throw std::invalid_argument("Configuration name not found in the specified object's configuration descriptor");

    const autowiring::config_field& f = q->second;
    return f.marshaller->marshal(reinterpret_cast<uint8_t*>(&obj) + f.offset);
  }

  /// <summary>
  /// Sets the named configuration value to the specified string value
  /// </summary>
  template<typename T>
  void ConfigSet(const char* name, T& obj, const char* value) {
    const config_descriptor& desc = config_registry_entry<T>::entry.descriptor;
    auto q = desc.fields.find(name);
    if (q == desc.fields.end())
      throw std::invalid_argument("Configuration name not found in the specified object's configuration descriptor");

    const autowiring::config_field& f = q->second;
    f.marshaller->unmarshal(
      reinterpret_cast<uint8_t*>(&obj) + f.offset,
      value
    );
  }
}
