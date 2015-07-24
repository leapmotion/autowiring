// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "auto_id.h"
#include <initializer_list>
#include <unordered_map>
#include <vector>

namespace autowiring {

struct HeteroBlockEntryBase {
  HeteroBlockEntryBase(auto_id id, size_t index) :
    id(id),
    index(index)
  {}

  // Block entry for the constructed type.  This entry must be fully instantiated
  auto_id id;

  // A unique index for this block entry
  size_t index;

  struct hash {
    size_t operator()(const HeteroBlockEntryBase& e) const {
      std::hash<auto_id> h;
      return h(e.id) + e.index;
    }
  };

  bool operator==(const HeteroBlockEntryBase& rhs) const {
    return id == rhs.id && index == rhs.index;
  }
};

/// <summary>
/// A single heterogenous block entry
/// </summary>
struct HeteroBlockEntry:
  HeteroBlockEntryBase
{
  template<typename T>
  HeteroBlockEntry(auto_id_t<T> id, size_t index) :
    HeteroBlockEntryBase{ id, index },
    pfnCtor(&PlacementNew<T>),
    pfnDtor(&Destructor<T>)
  {
    (void) auto_id_t_init<T>::init;
  }

  template<typename T>
  static void PlacementNew(void* pMem) { new (pMem) T; }

  template<typename T>
  static void Destructor(void* pMem) { ((T*)pMem)->~T(); }

  // Constructor for this entry:
  void(*pfnCtor)(void*);

  // Destructor for this entry:
  void(*pfnDtor)(void*);
};

/// <summary>
/// Constructs a heterogenous lookaside block
/// </summary>
/// <remarks>
/// A heterogenous block
/// </remarks>
class HeteroBlock
{
public:
  HeteroBlock(const HeteroBlockEntry* pFirst, const HeteroBlockEntry* pLast);
  HeteroBlock(std::initializer_list<HeteroBlockEntry> entries) :
    HeteroBlock(entries.begin(), entries.end())
  {}
  ~HeteroBlock();

private:
  // A shared pointer to the block proper.  This shared pointer points to a memory block
  // containing all of the subelements allocated in the arena.  Entries in the block are
  // structured like this:
  //
  //  <--- sizeof(T) ---> | sizeof(void*) | sizeof(void*) | <--- sizeof(U) ---> | sizeof(void*) | sizeof(void*)
  //  T ----------------- | &T::~T        | sizeof(U)     | U ----------------- | &U::~U        | sizeof(V)
  //
  // The last entry looks like this:
  //
  // 
  //  <--- sizeof(X) ---> | sizeof(void*) | sizeof(void*)
  //  X ----------------- | &X::~X        | 0
  std::shared_ptr<void> block;

  // Structure describing the layout of block headers
  struct BlockHeader {
    // Deleter we'll be using to free the "data" field
    void(*pfnDtor)(void*);

    // Size of the next block's data field
    size_t nextSize;
  };

  struct entry {
    // Offset, from the base of the block, where the object is stored
    size_t offset;

    // May potentially be null, if the shared pointer isn't initialized
    std::shared_ptr<void> ptr;
  };

  // A map of identifier entries to their offsets in the block
  std::unordered_map<HeteroBlockEntryBase, entry, HeteroBlockEntryBase::hash> entries;

public:
  /// <summary>
  /// Retrieves the nth instance of type T on the block
  /// </summary>
  /// <param name="T">The type to be obtained, must be complete at the time of the call</param>
  /// <param name="index">The index of the type to be obtained</param>
  template<typename T>
  T& get(size_t index) {
    auto q = entries.find({auto_id_t<T>{}, index});
    if (q == entries.end())
      throw std::runtime_error("Attempted to obtain a type or index not initialized on this block");
    return *(T*)((uint8_t*)block.get() + q->second.offset);
  }

  /// <summary>
  /// Identical to get, except retrieves a shared pointer
  /// </summary>
  /// <param name="T">The type to be obtained, must be complete at the time of the call</param>
  /// <param name="index">The index of the type to be obtained</param>
  /// <remarks>
  /// In a multithreaded environment, this method can have worse performance than the non-shared override.
  /// This is due to the fact that initializing a shared_ptr requires a memory barrier, and barriers are
  /// very slow.
  /// </remarks>
  template<typename T>
  std::shared_ptr<T>& get_shared(size_t index) {
    auto q = entries.find({ auto_id_t<T>{}, index });
    if (q == entries.end())
      throw std::runtime_error("Attempted to obtain a type or index not initialized on this block");

    if (!q->second.ptr)
      // Need to initialize now
      q->second.ptr = std::shared_ptr<void>(
        block,
        (uint8_t*)block.get() + q->second.offset
      );

    // Safety is assured by AnySharedPointer unit tests.  We use a native reinterpret_cast and not
    // std::static_pointer_cast because we are using a form of type erasure on std::shared_ptr<void>
    return reinterpret_cast<std::shared_ptr<T>&>(q->second.ptr);
  }
};

}
