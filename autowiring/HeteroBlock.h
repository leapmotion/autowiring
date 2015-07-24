#pragma once
#include "auto_id.h"
#include <initializer_list>

namespace autowiring {

/// <summary>
/// A single heterogenous block entry
/// </summary>
struct HeteroBlockEntry {
  // The size of the type that will be constructed
  size_t ncb;
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
  HeteroBlock(std::initializer_list<HeteroBlockEntry> entries);
  ~HeteroBlock();

private:
  // A shared pointer to the block proper.  This shared pointer points to a memory block
  // containing all of the subelements allocated in the arena.
  std::shared_ptr<void> block;

  struct entry {
    auto_id id;
    size_t index;
  };
  struct entry_hash {
    size_t operator()(const entry& e) const {
      std::hash<auto_id> h;
      return h(e.id) + e.index;
    }
  };

  // A map of identifier entries to their offsets in the block
  std::unordered_map<entry, void*, entry_hash> offsets;

public:
  /// <summary>
  /// Retrieves the nth instance of type T on the block
  /// </summary>
  template<typename T>
  std::shared_ptr<T> get(size_t index) {
    auto q = offsets.find({auto_id_t<T>{}, index});
    if (q == offsets.end())
      throw std::runtime_error("Attempted to obtain an index not provided by this type");
    ;
  }
};

}