// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AnySharedPointer.h"
#include "auto_id.h"
#include "bounds.h"
#include "has_getconfigdescriptor.h"
#include "is_any.h"
#include "marshaller.h"
#include "vector_from_tuple.h"
#include <initializer_list>
#include <stdexcept>
#include <unordered_map>

namespace autowiring {
  struct config_field;

  template<typename T>
  struct is_multi {
    template<typename U>
    static std::integral_constant<bool, U::is_multi> select(decltype(U::is_multi)*);

    template<typename U>
    static std::false_type select(...);

    static const bool value = decltype(select<T>(0))::value;
  };

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

  template<typename T, typename... Args>
  struct has_bind
  {
    template<class U>
    static std::true_type select(
      decltype(
        static_cast<U*>(nullptr)->bind(
          *(Args*)nullptr...
        )
      )*
    );

    template<class U>
    static std::false_type select(...);

    static const bool value = decltype(select<T>(nullptr))::value;
  };

  /// <summary>
  /// Determines whether field type T can be described by metadata type M
  /// </summary>
  template<typename T, typename M>
  struct valid {
    template<class U>
    static std::integral_constant<bool, U::template valid<T>::value> select(void*);

    template<class U>
    static std::true_type select(...);

    static const bool value = decltype(select<M>(nullptr))::value;
  };

  struct metadata_base {
    virtual ~metadata_base(void) {}
    virtual auto_id id(void) const = 0;
    virtual const void* value(void) const = 0;
  };

  template<typename M>
  struct metadata :
    metadata_base
  {
    metadata(void) = delete;
    metadata(const M& value) : m_value(value) {}

    M m_value;

    template<typename T>
    typename std::enable_if<has_bind<M, typename std::remove_reference<T>::type>::value>::type bind(const config_field&, T& field) {
      m_value.bind(field);
    }
    template<typename T>
    typename std::enable_if<has_bind<M, const config_field, typename std::remove_reference<T>>::value>::type bind(const config_field& fieldDesc, T& field) {
      m_value.bind(fieldDesc, field);
    }

    template<typename T>
    typename std::enable_if<
      !has_bind<M, T>::value &&
      !has_bind<M, const config_field, T>::value
    >::type bind(const config_field&, T&) {}

    auto_id id(void) const override { return auto_id_t<M>{}; }
    const void* value(void) const override { return &m_value; }
  };

  struct metadata_pack_base {
    virtual const std::vector<const metadata_base*>& get_list(void) const = 0;
    virtual std::unique_ptr<metadata_pack_base> clone(void) const = 0;
    virtual void bind(const config_field& configField, void* pField) = 0;
  };

  template<typename T, typename... Ms>
  struct metadata_pack :
    metadata_pack_base
  {
    metadata_pack(const metadata_pack& rhs) :
      pack(rhs.pack),
      list(ptr_vector_from_tuple<const metadata_base>(pack))
    {}

    metadata_pack(Ms&&... ms) :
      pack(metadata<Ms>(std::forward<Ms&&>(ms))...),
      list(ptr_vector_from_tuple<const metadata_base>(pack))
    {}

    std::tuple<metadata<Ms>...> pack;
    std::vector<const metadata_base*> list;

    const std::vector<const metadata_base*>& get_list(void) const override { return list; }

    std::unique_ptr<metadata_pack_base> clone(void) const override {
      return std::unique_ptr<metadata_pack_base>{
        new metadata_pack{ *this }
      };
    }

  private:
    template<typename U, int... Ns>
    void bind(const config_field& configField, U& field, index_tuple<Ns...>) {
      noop(
        (std::get<Ns>(pack).template bind<U>(configField, field), false)...
      );
    }

  public:
    void bind(const config_field& configField, void* pField) override {
      bind<T>(
        configField,
        *static_cast<T*>(pField),
        typename make_index_tuple<sizeof...(Ms)>::type{}
      );
    }
  };

  template<typename M>
  struct metadata_list :
    std::vector<metadata<M>*>
  {};

  template<typename... Ms>
  struct recursive_single_check {};

  template<typename M, typename... Ms>
  struct recursive_single_check<M, Ms...> :
    recursive_single_check<Ms...>
  {
    static_assert(
      is_multi<M>::value || !is_any_same<M, Ms...>::value,
      "Decoration type M was marked as being a singleton decoration, but appeared more than once in an initializer list"
    );
  };

  template<typename T>
  struct default_value_t {
    default_value_t(T&& value) :
      value(std::forward<T>(value))
    {}

    T value;
  };

  template<typename T>
  default_value_t<typename std::remove_cv<T>::type> default_value(T&& t) {
    return { std::forward<T>(t) };
  }

  /// <summary>
  /// Represents a single field in a context member that may be configured
  /// </summary>
  struct config_field {
    config_field(void) = default;
    config_field(const config_field& rhs) { *this = rhs; }

    template<typename T, typename U, typename... Ms>
    config_field(const char* name, const char* desc, U T::* memptr, Ms&&... ms) :
      name(name),
      description(desc),
      type(auto_id_t<U>()),
      offset(reinterpret_cast<size_t>(&(static_cast<T*>(nullptr)->*memptr))),
      default_value(std::make_shared<typename std::remove_cv<U>::type>()),
      marshaller{ &get_marshaller<U>() },
      metadata(new metadata_pack<U, Ms...>{ std::forward<Ms>(ms)... })
    {
      noop(
        (HandleMetadata(memptr, ms), false)...
      );
      for (const metadata_base* entry : metadata->get_list())
        m_mdMap[entry->id()].push_back(entry);
    }

    template<typename T, typename U, typename... Ms>
    config_field(const char* name, U T::* memptr, Ms&&... ms) :
      config_field(name, "", memptr, std::forward<Ms>(ms)...)
    {}

    // Name and optional description
    const char* name = nullptr;
    const char* description = nullptr;

    // Field type
    auto_id type = auto_id_t<void>{};

    // Offset from the base of the object to the member to be serialized
    size_t offset = 0;

    // Default value for this type.  Must not be empty, but there are no guarantees
    // about the type of the underlying object.  In particular, the object will be
    // initialized, but it may be value-initialized; for integer types this means
    // it may contain random data.
    AnySharedPointer default_value;

    // Pointer to the required marshaller singleton.  This allows us to move to and
    // from string representations of this field.
    const marshaller_base* marshaller = nullptr;

    // All of the metadata attached to this field
    std::unique_ptr<metadata_pack_base> metadata;

    /// <summary>
    /// Gets a list of all metadata of a specified type on this config descriptor
    /// </summary>
    template<typename M>
    const metadata_list<M>& collect_list(void) const {
      auto q = m_mdMap.find(auto_id_t<M>{});
      return static_cast<const metadata_list<M>&>(
        q == m_mdMap.end() ? empty : q->second
      );
    }

    config_field& operator=(const config_field& rhs) {
      name = rhs.name;
      description = rhs.description;
      offset = rhs.offset;
      default_value = rhs.default_value;
      marshaller = rhs.marshaller;

      m_mdMap.clear();
      if (rhs.metadata) {
        metadata = rhs.metadata->clone();
        for (auto& entry : metadata->get_list())
          m_mdMap[entry->id()].push_back(entry);
      }
      else
        metadata.reset();
      return *this;
    }

  private:
    std::vector<const metadata_base*> empty;
    std::unordered_map<auto_id, std::vector<const metadata_base*>> m_mdMap;

    // Initializes our default value
    template<typename T, typename U, typename V>
    void HandleMetadata(T U::*, const default_value_t<V>& value) {
      default_value = AnySharedPointerT<T>{ value.value };
    }

    // Final case does nothing with the metadata
    template<typename T, typename U, typename V>
    void HandleMetadata(T U::*, const V&) {}
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

    config_descriptor(void);
    config_descriptor(config_descriptor&& rhs) :
      fields(std::move(rhs.fields))
    {}
    config_descriptor(std::initializer_list<config_field> fields) :
      fields(MakeFields(fields))
    {}

    const t_mpType fields;

    /// <summary>
    /// Retrieves the configuration field at the specified offset
    /// </summary>
    const config_field& get(size_t offset) const;

    /// <summary>
    /// Retrieves the configuration field for the specified member
    /// </summary>
    template<typename T, typename U>
    const config_field& get(U T::*pMember) const {
      return get(reinterpret_cast<size_t>(&(static_cast<T*>(nullptr)->*pMember)));
    }
  };
}
