#pragma once
#include STL_UNORDERED_MAP
#include TYPE_INDEX_HEADER
#include TYPE_TRAITS_HEADER
#include SHARED_PTR_HEADER
#include <vector>

template<class T, bool b>
struct ground_type_of_helper {
  typedef typename T::ground type;
};

template<class T>
struct ground_type_of_helper<T, false> {
  typedef T type;
};

/// <summary>
/// Extracts the ground type, if available
/// </summary>
template<class T>
struct ground_type_of {
  template<class U>
  static int select(typename U::ground* );

  template<class U>
  static char select(...);

  typedef typename ground_type_of_helper<T, sizeof(select<T>(nullptr)) == sizeof(int)>::type type;
};

/// <summary>
/// Represents a type tree that will work on exactly one type
/// </summary>
template<class Ground>
struct SingleTypeTree {
  template<class T>
  void Add(const std::shared_ptr<T>& rhs) {
    auto casted = std::fast_pointer_cast<Ground, T>(rhs);
    if(casted)
      m_membership.push_back(casted);
  }

  template<class T>
  bool Resolve(std::shared_ptr<T>& ptr) {
    ptr.reset();

    // Attempt to cast each element in our membership collection, but return false if
    // more than one of our members satisfies the request
    for(size_t i = m_membership.size(); i--;) {
      auto casted = std::fast_pointer_cast<T, Ground>(m_membership[i]);
      if(!casted)
        continue;
      if(ptr)
        return false;
      ptr.swap(casted);
    }
    return true;
  }

private:
  // Known membership:
  std::vector<std::shared_ptr<Ground>> m_membership;
};

template<class T = void, class List = void>
struct ExplicitGrounds:
  protected ExplicitGrounds<T, void>,
  protected List
{
  template<class T>
  void Add(const std::shared_ptr<T>& rhs) {
    ExplicitGrounds<T, void>::Add(rhs);
    List::Add(rhs);
  }

  template<class T>
  bool Resolve(std::shared_ptr<T>& ptr) {
    // Reset so we can make a trivial-null assumption in base types:
    ptr.reset();

    // Pass control to the base types:
    return
      ExplicitGrounds<T, void>::Resolve(ptr) &&
      List::Resolve(ptr);
  }
};

template<class T>
struct ExplicitGrounds<T, void>
{
  template<class T>
  void Add(const std::shared_ptr<T>& rhs) {
    m_typeTree.Add(rhs);
  }

  template<class T>
  bool Resolve(std::shared_ptr<T>& ptr) {
    return m_typeTree.Resolve(ptr);
  }

private:
  SingleTypeTree<T> m_typeTree;
};

template<>
struct ExplicitGrounds<void, void>
{
  template<class T>
  void Add(const std::shared_ptr<T>& rhs) {}

  template<class T>
  bool Resolve(std::shared_ptr<T>& ptr) {return true;}
};

/// <summary>
/// Represents a type bag, which may be used to perform runtime type resolution
/// </summary>
/// <param name="ExplicitGrounds">
/// A pseudo-list of explicit grounds to be used with this type forest.  Explicit grounds
/// should be specified using the explicit grounds helper structure, or should be left
/// empty.
/// </param>
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
///         A         |
///        / \        |
///       B   C       |
///      / \          |
///     D   E         |
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
template<class V = ExplicitGrounds<>>
class PolymorphicTypeForest:
  public V
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
    virtual void* RawPointer(void) const = 0;
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
    virtual bool Contains(Ground* rhs) const {return false;}
  };

  template<class Ground, class T>
  struct Tree:
    TreeBase<Ground>
  {
  public:
    // Witness of this type
    std::shared_ptr<T> pWitness;

    void* RawPointer(void) const override {return pWitness.get();}

    virtual void operator=(const std::shared_ptr<Ground>& rhs) override {
      TreeBase<Ground>::pGround = rhs;
      pWitness = std::dynamic_pointer_cast<T, Ground>(rhs);
    }

    void Assign(const std::shared_ptr<T>& rhs) {
      TreeBase<Ground>::pGround = std::static_pointer_cast<Ground, T>(rhs);
      pWitness = rhs;
    }

    virtual bool Contains(Ground* rhs) const override {
      return !!dynamic_cast<T*>(rhs);
    }
  };

  struct GroundedCoordinate {
    GroundedCoordinate(const std::type_info& ground, const std::type_info& derived):
      ground(ground),
      derived(derived)
    {}

    const std::type_info& ground;
    const std::type_info& derived;

    bool operator==(const GroundedCoordinate& rhs) const {
      return ground == rhs.ground && derived == rhs.derived;
    }

    operator size_t(void) const {
      // The derived type provides adequate collision resistance, as a derived type has
      // a unique ground in any case
      return std::hash<std::type_index>()(derived);
    }
  };

  struct GroundedCoordinateHasher {
    size_t operator()(const GroundedCoordinate& rhs) const {
      return (size_t)rhs;
    }
  };
  
  // All known type trees
  // TODO:  These trees also include grounded coordinates from our explicit grounds.  We could be
  // a bit more efficient if we avoided adding members derived from one (or more) explicit ground
  // to this collection, and reserved this collection instead for alien types.
  typedef std::unordered_map<GroundedCoordinate, TreeBaseFoundation*, GroundedCoordinateHasher> t_mpType;
  t_mpType m_trees;

  // Memoized results for search efficiency and to facilitate ambiguation detection:
  t_mpType m_memos;

public:
  // Accessor methods:
  size_t size(void) const {return m_trees.size();}
  typename t_mpType::iterator begin(void) {return m_trees.begin();}
  typename t_mpType::const_iterator begin(void) const {return m_trees.begin();}
  typename t_mpType::iterator end(void) {return m_trees.end();}
  typename t_mpType::const_iterator end(void) const {return m_trees.end();}

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
    typedef typename ground_type_of<T>::type Ground;

    // Cast the witness down to the ground type:
    const GroundedCoordinate coord(typeid(Ground), typeid(T));
    auto pWitnessGround = static_cast<Ground*>(pWitness.get());

    // Collection of unsatisfied witnesses
    std::vector<TreeBase<Ground>*> te;

    // Explicit base introduction:
    V::Add(pWitness);

    // Linear scan for collisions:
    for(auto q = m_memos.begin(); q != m_memos.end(); q++) {
      if(!(q->first.ground == typeid(Ground)))
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

    // Transfer assignment on all memos:
    for(size_t i = te.size(); i--;)
      *te[i] = std::static_pointer_cast<Ground, T>(pWitness);

    // Ensure that the desired slot exists before we run through our memo search:
    auto& pTreeMemo = m_memos[coord];
    if(!pTreeMemo)
      pTreeMemo = new Tree<Ground, T>;
    static_cast<Tree<Ground, T>*>(pTreeMemo)->Assign(pWitness);

    // Construct the forest entry for this type:
    auto& pTree = m_trees[coord];
    if(!pTree)
      pTree = new Tree<Ground, T>;
    static_cast<Tree<Ground, T>*>(pTree)->Assign(pWitness);

    // We have successfully resolved a type unambiguously, we can return here
    return true;
  }

  /// <returns>
  /// True if we contain any member of type T
  /// </returns>
  template<class T>
  bool Contains(void) const {
    return Contains(typeid(typename ground_type_of<T>::type), typeid(T));
  }
  
  /// <returns>
  /// True if we contain any member of type type_info
  /// </returns>
  bool Contains(const std::type_info& ground, const std::type_info& type) const {
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
    typedef typename ground_type_of<T>::type Ground;
    const GroundedCoordinate coord(typeid(Ground), typeid(T));

    auto q = m_memos.find(coord);
    if(q == m_memos.end())
      return false;

    auto val = static_cast<Tree<Ground, T>*>(q->second);
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
    typedef typename ground_type_of<T>::type Ground;
    const GroundedCoordinate coord(typeid(Ground), typeid(T));

    // Attempt to find a precise match:
    auto q = m_memos.find(coord);
    if(q != m_memos.end())
      return ptr = static_cast<Tree<Ground, T>*>(q->second)->pWitness, true;

    TreeBase<Ground>* pMatchedTree = nullptr;
    std::shared_ptr<T> match;

    // Scan on all explicit bases--if this succeeds and a tree scan rematches, then we wind up
    // with a failure in the aggregate due to multiple resolvable pathways.
    V::Resolve(match);

    // Linear scan on all trees (but not memos)
    for(auto q = m_trees.begin(); q != m_trees.end(); q++) {
      if(
        // We make an exception for exact matches of named types, even if grounds don't match
        !(q->first.ground == typeid(T)) &&
        !(q->first.ground == typeid(Ground))
      )
        continue;

      auto pCur = static_cast<TreeBase<Ground>*>(q->second);
      std::shared_ptr<T> attemptedCast = std::dynamic_pointer_cast<T, Ground>(pCur->pGround);
      if(!attemptedCast)
        // No match, try the next tree in the forest
        continue;

      if(match && match != attemptedCast)
        // Match already identified for this type, and it's not the same--T is ambiguous!  Short return
        return false;

      // Copy over the match
      pMatchedTree = pCur;
      match.swap(attemptedCast);
    }

    // Memoize the consequences of our search:
    auto pTree = new Tree<Ground, T>;
    pTree->pWitness = match;
    if(pMatchedTree)
      pTree->pGround = pMatchedTree->pGround;
    m_memos[coord] = pTree;

    // Pass the results back to the caller and indicate unambiguous success
    ptr.swap(match);
    return true;
  }
};

