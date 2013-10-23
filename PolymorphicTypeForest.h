#pragma once
#include STL_UNORDERED_MAP
#include TYPE_INDEX_HEADER
#include TYPE_TRAITS_HEADER
#include SHARED_PTR_HEADER
#include <allocators>
#include <list>
#include <vector>

/// <summary>
/// Extracts the ground type, if available
/// </summary>
template<class T>
struct ground_type_of {
  template<bool b>
  struct resolver {
    typedef typename T::ground type;
  };

  template<>
  struct resolver<false> {
    typedef typename T type;
  };
  
  template<class U>
  static typename U::ground select(typename U::ground*);

  template<class U>
  static U select(...);

  typedef decltype(select<T>(nullptr)) type;
};

/// <summary>
/// Represents a type bag, which may be used to perform runtime type resolution
/// </summary>
/// <remarks>
/// A type tree is a collection of types related by inheritance.  A type forest is
/// a collection of such trees.
///
/// A type tree is identified by a witness for that tree.  The witness caps the tree,
/// and prevents the insertion of any types which might derive from the witness.
/// All types in the tree must inherit from the Ground type.  By extension, if a witness
/// of type Ground is added to the forest, it will not be possible to add anything else.
///
/// Consider the following inheritance graph:
///
///      A
///     / \
///    B   C
///   / \
///  D   E
///
/// The following are the maxima for the type forest.  That is to say, given a type forest
/// containing these types, it would not be possible to expand the type forest any further:
///
/// [A]
/// [B C]
/// [D E C]
///
/// The caller is responsible for externally synchronizing this structure.
/// </remarks>
template<class Ground>
class PolymorphicTypeForest
{
public:
  ~PolymorphicTypeForest(void) {
    for(auto q = m_trees.begin(); q != m_trees.end(); q++)
      delete q->second;
    for(auto q = m_memos.begin(); q != m_memos.end(); q++)
      delete q->second;
  }

private:
  struct TreeBase {
    // Ground type shared pointer
    std::shared_ptr<Ground> pGround;
    
    /// <summary>
    /// Casts the passed value to the bound type
    /// </summary>
    virtual void operator=(const std::shared_ptr<Ground>& rhs) = 0;

    /// <summary>
    /// Returns true if rhs is of type T, the type named by this entry
    /// </summary>
    virtual bool Contains(Ground* rhs) const = 0;
  };

  template<class T>
  struct Tree:
    TreeBase
  {
  public:
    // Witness of this type
    std::shared_ptr<T> pWitness;

    virtual void operator=(const std::shared_ptr<Ground>& rhs) override {
      pGround = rhs;
      pWitness = std::dynamic_pointer_cast<T, Ground>(rhs);
    }

    virtual bool Contains(Ground* rhs) const override {
      return !!dynamic_cast<T*>(rhs);
    }
  };

  // All known type trees
  std::unordered_map<std::type_index, TreeBase*> m_trees;

  // Memoized results for search efficiency and to facilitate ambiguation detection:
  std::unordered_map<std::type_index, TreeBase*> m_memos;

public:
  // Accessor methods:
  size_t size(void) const {return m_trees.size();}

  /// <summary>
  /// Adds the passed type to this collection
  /// </summary>
  /// <param name="pWitness">A witness to the tree to be added</param>
  /// <returns>True if the type was added successfully</returns>
  /// <remarks>
  /// This method will fail if type T has already been introduced to the collection, but
  /// it may also fail if any descendant type of T is currently present in the collection.
  ///
  /// Insertion complexity is linear with the number of trees.
  /// </remarks>
  template<class T>
  bool AddTree(std::shared_ptr<T> pWitness) {
    static_assert(std::is_base_of<Ground, T>::value, "Cannot add an unrelated type to the forest");

    // Cast the witness down to the ground type:
    auto pWitnessGround = static_cast<Ground*>(pWitness.get());

    // Collection of unsatisfied witnesses
    std::vector<TreeBase*> te;

    // Linear scan for collisions:
    for(auto q = m_memos.begin(); q != m_memos.end(); q++) {
      auto cur = q->second;

      if(cur->pGround)
        // Already-resolved instance, we need to ensure there are no collisions
      {
        if(
          cur->Contains(pWitnessGround) ||
          dynamic_cast<T*>(cur->pGround.get())
        )
          // Relationship exists between this witness and an already-resolved witness, we cannot proceed
          return false;
      }
      else if(cur->Contains(pWitnessGround))
        // We can delay-resolve this entry:
        te.push_back(cur);
    }

    // Run through all necessary transfer assignments:
    for(size_t i = te.size(); i--;)
      *te[i] = std::static_pointer_cast<Ground, T>(pWitness);

    // Construct and introduce the new tree
    auto pTree = new Tree<T>;
    m_trees[typeid(T)] = pTree;
    pTree->pWitness = pWitness;
    pTree->pGround = pWitness;

    // Also construct the memoization for this type:
    m_memos[typeid(T)] = &(*new Tree<T> = *pTree);

    return true;
  }

  /// <returns>
  /// False if resolution was ambiguous
  /// </returns>
  /// <param name="ptr">Receives a pointer to the resolved instance, or nullptr</param>
  /// <remarks>
  /// If an exact match does not exist, then a linear scan will be made in which each
  /// enclosed type will be dynamic casted to the desired type.  If there is a hit, an
  /// entry is made mapping the exact specified type to the hit type.  If there is a
  /// miss, an empty entry is inserted to prevent the search from being required again.
  ///
  /// This method has amortized constant time complexity.
  /// </remarks>
  template<class T>
  bool Resolve(std::shared_ptr<T>& ptr) {
    // Attempt to find a precise match:
    auto q = m_memos.find(typeid(T));
    if(q != m_memos.end())
      return ptr = static_cast<Tree<T>*>(q->second)->pWitness, true;

    std::shared_ptr<T> match;

    // Linear scan on all trees (but not memos)
    for(auto q = m_trees.begin(); q != m_trees.end(); q++) {
      std::shared_ptr<T> attemptedCast = std::dynamic_pointer_cast<T, Ground>(q->second->pGround);
      if(!attemptedCast)
        // No match, try the next tree in the forest
        continue;

      if(match)
        // Match already identified for this type--T is ambiguous!  Short return
        return false;

      // Copy over the match
      match = attemptedCast;
    }

    // Memoize the consequences of our search:
    auto pTree = new Tree<T>;
    pTree->pWitness = match;
    pTree->pGround = match;
    m_memos[typeid(T)] = pTree;

    // Pass the results back to the caller and indicate unambiguous success
    ptr.swap(match);
    return true;
  }
};

