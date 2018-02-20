// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#pragma once
#include "fast_pointer_cast.h"

class CoreObject;

namespace autowiring {
  template<typename> struct s {};

  /// <summary>
  /// Returns an index for use with the auto_id system
  /// </summary>
  int CreateIndex(void);

  /// <summary>
  /// Safe alignment extractor, useful because MSVC doesn't allow __alignof to be used on abstract types
  /// </summary>
  template<typename T, bool = std::is_abstract<T>::value>
  struct safe_align_of:
    std::integral_constant<size_t, AUTO_ALIGNOF(T)>
  {};

  // Abstract type, the alignment of this type is irrelevant
  template<typename T>
  struct safe_align_of<T, true>:
    std::integral_constant<size_t, 1>
  {};

  struct auto_id_block {
    auto_id_block(void) = default;

    auto_id_block(int index, const std::type_info& ti_synth):
      index(index),
      ti_synth(&ti_synth),
      pToObj(NullToObj),
      pFromObj(NullFromObj)
    {}

    static std::shared_ptr<CoreObject> NullToObj(const std::shared_ptr<void>&) AUTO_NOEXCEPT;
    static std::shared_ptr<void> NullFromObj(const std::shared_ptr<CoreObject>&) AUTO_NOEXCEPT;

    template<class T>
    auto_id_block(
      int index,
      std::shared_ptr<CoreObject>(*pToObj)(const std::shared_ptr<T>&),
      std::shared_ptr<T>(*pFromObj)(const std::shared_ptr<CoreObject>&)
    ):
      index(index),
      ti(&typeid(T)),
      ti_synth(&typeid(s<T>)),
      ncb(sizeof(T)),
      align(safe_align_of<T>::value),
      pToObj(
        reinterpret_cast<std::shared_ptr<CoreObject>(*)(const std::shared_ptr<void>&) AUTO_NOEXCEPT>(pToObj)
      ),
      pFromObj(
        reinterpret_cast<std::shared_ptr<void>(*)(const std::shared_ptr<CoreObject>&) AUTO_NOEXCEPT>(pFromObj)
      )
    {}

#if defined(__clang__)
#pragma clang diagnostic push
#if __clang_major__ >= 9
#pragma clang diagnostic ignored "-Wc++1z-compat-mangling"
#elif __clang_major__ >= 5
#pragma clang diagnostic ignored "-Wc++1z-compat"
#endif
#endif
    auto_id_block(
      int index,
      const std::type_info* ti,
      const std::type_info* ti_synth,
      size_t ncb,
      size_t align,
      std::shared_ptr<CoreObject>(*pToObj)(const std::shared_ptr<void>&) AUTO_NOEXCEPT,
      std::shared_ptr<void>(*pFromObj)(const std::shared_ptr<CoreObject>&) AUTO_NOEXCEPT
    ):
      index(index),
      ti(ti),
      ti_synth(ti_synth),
      ncb(ncb),
      align(align),
      pToObj(pToObj),
      pFromObj(pFromObj)
    {}
#if defined(__clang__) && __clang_major__ >= 9
#pragma clang diagnostic pop
#endif

    // Index and underlying type.  Indexes are guaranteed to start at 1.  The index value of 0
    // is reserved as the invalid index.
    int index;
    const std::type_info* ti;

    // Synthetic ID.  This ID is always valid but it's created as a template substitution.
    const std::type_info* ti_synth;

    // General properties of the underlying type
    size_t ncb;

    // Alignment requirement of this type:
    size_t align;

    // Generic fast casters to CoreObject
    std::shared_ptr<CoreObject>(*pToObj)(const std::shared_ptr<void>&) AUTO_NOEXCEPT;
    std::shared_ptr<void>(*pFromObj)(const std::shared_ptr<CoreObject>&) AUTO_NOEXCEPT;

    bool operator==(const auto_id_block& rhs) const { return index == rhs.index && ti == rhs.ti; }
    bool operator!=(const auto_id_block& rhs) const { return !(*this == rhs); }
  };
}

struct auto_id;

/// <summary>
/// Identifier sigil structure
/// </summary>
/// <remarks>
/// This structure is used to avoid uses of the very inefficient std::type_info comparison
/// and hashing operators.
/// </remarks>
template<class T>
struct auto_id_t
{
  // The identifier block proper
  static autowiring::auto_id_block s_block;

  bool operator==(const auto_id& rhs) const { return rhs == *this; }
  bool operator!=(const auto_id& rhs) const { return rhs != *this; }
};

// Recursive auto_id_t definitions are not allowed, don't try to use them
template<typename T>
struct auto_id_t<auto_id_t<T>>;

/// <summary>
/// Base type identifier structure
/// </summary>
struct auto_id {
  template<typename T = void>
  auto_id(const auto_id_t<T>& = auto_id_t<T>{}) :
    auto_id(auto_id_t<T>::s_block)
  {}

  auto_id(const autowiring::auto_id_block& block) :
    block(&block)
  {}

  // A pointer to the identifier block containing the index
  const autowiring::auto_id_block* block;

  bool operator==(auto_id rhs) const {
    return block == rhs.block;
  }

  bool operator!=(auto_id rhs) const {
    return block != rhs.block;
  }

  template<typename T>
  bool operator==(const auto_id_t<T>) const {
    return block == &auto_id_t<T>::s_block;
  }

  template<typename T>
  bool operator!=(const auto_id_t<T>) const {
    return block != &auto_id_t<T>::s_block;
  }

  explicit operator bool(void) const {
    return block && block->index;
  }
};

template<>
struct auto_id_t<void>
{
  // Void is defined to have an index of zero, so we can mark it const
  static const autowiring::auto_id_block s_block;

  bool operator==(const auto_id& rhs) const { return rhs == *this; }
  bool operator!=(const auto_id& rhs) const { return rhs != *this; }
};

namespace std {
  template<>
  struct hash<auto_id> {
    size_t operator()(const auto_id& id) const {
      return id.block ? id.block->index : 0;
    }
  };
}

/// <summary>
/// Invokes the ctor for the auto_id_t field.
/// </summary>
/// <remarks>
/// Performs the full initialization.  We have to do template shenanegans to prevent an accidental
/// mention of sizeof(T) or typeid(T), because one or both of these might not be available in this
/// context depending on what types are available at this parse point in the compilation unit and
/// what platform we are currently compiling for.
/// </remarks>
template<typename T, bool full = true>
class auto_id_t_init;

// Performs full initialization for type T
template<typename T>
class auto_id_t_init<T, true>
{
  auto_id_t_init(void) {
    auto* ptr = &auto_id_t<T>::s_block;

    // We are instantiating the fast pointer casters here, also initialize the blind block:
    (void) autowiring::fast_pointer_cast_initializer<CoreObject, T>::sc_init;
    (void) autowiring::fast_pointer_cast_initializer<T, CoreObject>::sc_init;

    // Only create an index if one is not already assigned.  We rely on the fact that the "index"
    // field will be allocated in the segment, where we can be assured it will default to 0 due
    // to the nature of the segment.
    new (ptr) autowiring::auto_id_block(
      ptr->index ? ptr->index : autowiring::CreateIndex(),
      &autowiring::fast_pointer_cast<CoreObject, T>,
      &autowiring::fast_pointer_cast<T, CoreObject>
    );
  }

public:
  static const auto_id_t_init init;
};


// Specialization just initialize the traits known to be available on T
template<typename T>
class auto_id_t_init<T, false>
{
  auto_id_t_init(void) {
    auto* ptr = &auto_id_t<T>::s_block;
    new (ptr) autowiring::auto_id_block(ptr->index ? ptr->index : autowiring::CreateIndex(), typeid(autowiring::s<T>));
  }

public:
  static const auto_id_t_init init;
};

// Don't need to initialize void
template<>
class auto_id_t_init<void, true> {
public:
  static const auto_id_t_init init;
};

template<>
class auto_id_t_init<void, false>
{
public:
  static const auto_id_t_init init;
};

template<typename T>
class auto_id_t_init<T&, true> : public auto_id_t_init<T, true>{};
template<typename T>
class auto_id_t_init<T&, false> : public auto_id_t_init<T, false>{};
template<typename T>
class auto_id_t_init<const T, true> : public auto_id_t_init<T, true>{};
template<typename T>
class auto_id_t_init<const T, false> : public auto_id_t_init<T, false>{};

template<typename T>
const auto_id_t_init<T, false> auto_id_t_init<T, false>::init;

template<typename T>
const auto_id_t_init<T, true> auto_id_t_init<T, true>::init;

template<class T>
struct auto_id_t<T&> : auto_id_t<T> {};

template<class T>
struct auto_id_t<const T> : auto_id_t<T> {};

template<class T>
autowiring::auto_id_block auto_id_t<T>::s_block;
