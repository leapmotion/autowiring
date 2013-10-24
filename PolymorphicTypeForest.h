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
  struct TreeBaseFoundation {
    virtual ~TreeBaseFoundation(void) {}
  };

  template<class Ground>
  struct TreeBase:
    TreeBaseFoundation
  {
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

  template<class Ground, class T>
  struct Tree:
    TreeBase<Ground>
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

  struct GroundedCoordinate {
    GroundedCoordinate(std::type_index ground, std::type_index derived):
      ground(ground),
      derived(derived)
    {}

    std::type_index ground;
    std::type_index derived;

    bool operator==(const GroundedCoordinate& rhs) const {
      return ground == rhs.ground && derived == rhs.derived;
    }

    operator size_t(void) const {
      // The derived type provides adequate collision resistance, as a derived type has
      // a unique ground in any case
      return derived.hash_code();
    }
  };
  
  // All known type trees
  typedef std::unordered_map<GroundedCoordinate, TreeBaseFoundation*> t_mpType;
  t_mpType m_trees;

  // Memoized results for search efficiency and to facilitate ambiguation detection:
  t_mpType m_memos;

public:
  // Accessor methods:
  size_t size(void) const {return m_trees.size();}
  t_mpType::iterator begin(void) {return m_trees.begin();}
  t_mpType::const_iterator begin(void) const {return m_trees.begin();}
  t_mpType::iterator end(void) {return m_trees.end();}
  t_mpType::const_iterator end(void) const {return m_trees.end();}

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
    typedef ground_type_of<T>::type Ground;

    // Cast the witness down to the ground type:
    auto pWitnessGround = static_cast<Ground*>(pWitness.get());

    // Collection of unsatisfied witnesses
    std::vector<TreeBase<Ground>*> te;

    // Linear scan for collisions:
    for(auto q = m_memos.begin(); q != m_memos.end(); q++) {
      if(q->first.ground != typeid(Ground))
        continue;

      auto cur = static_cast<TreeBase<Ground>*>(q->second);

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
    GroundedCoordinate coord(typeid(Ground), typeid(T));
    auto pTree = new Tree<Ground, T>;
    m_trees[coord] = pTree;
    pTree->pWitness = pWitness;
    pTree->pGround = pWitness;

    // Also construct the memoization for this type:
    m_memos[coord] = &(*new Tree<Ground, T> = *pTree);

    return true;
  }

  /// <returns>
  /// True if we contain any member of type T
  /// </returns>
  template<class T>
  bool Contains(void) const {
    return Contains(typeid(ground_type_of<T>::type), typeid(T));
  }
  
  /// <returns>
  /// True if we contain any member of type type_info
  /// </returns>
  bool Contains(const type_info& ground, const type_info& type) const {
    auto q = m_memos.find(GroundedCoordinate(ground, type));
    return
      q == m_memos.end() ?
      false :
      !!q->second;
  }

  /// <summary>
  /// True if we contain a member of type T and that member matches the passed member
  /// </summary>
  template<class T>
  bool Contains(T* ptr) const {
    auto q = m_memos.find(typeid(T));
    if(q == m_memos.end())
      return false;

    auto val = static_cast<Tree<T>*>(q->second);
    return val->pWitness.get() == ptr;
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
    typedef ground_type_of<T>::type Ground;
    const GroundedCoordinate coord(typeid(Ground), typeid(T));

    // Attempt to find a precise match:
    auto q = m_memos.find(coord);
    if(q != m_memos.end())
      return ptr = static_cast<Tree<Ground, T>*>(q->second)->pWitness, true;

    TreeBase<Ground>* pMatchedTree = nullptr;
    std::shared_ptr<T> match;

    // Linear scan on all trees (but not memos)
    for(auto q = m_trees.begin(); q != m_trees.end(); q++) {
      if(q->first.ground != typeid(Ground))
        continue;

      auto pCur = static_cast<TreeBase<Ground>*>(q->second);
      std::shared_ptr<T> attemptedCast = std::dynamic_pointer_cast<T, Ground>(pCur->pGround);
      if(!attemptedCast)
        // No match, try the next tree in the forest
        continue;

      if(match)
        // Match already identified for this type--T is ambiguous!  Short return
        return false;

      // Copy over the match
      pMatchedTree = pCur;
      match = attemptedCast;
    }

    // Memoize the consequences of our search:
    auto pTree = new Tree<Ground, T>;
    pTree->pWitness = match;
    pTree->pGround = pMatchedTree ? pMatchedTree->pGround : nullptr;
    m_memos[coord] = pTree;

    // Pass the results back to the caller and indicate unambiguous success
    ptr.swap(match);
    return true;
  }
};

