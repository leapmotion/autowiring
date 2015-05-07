// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "autowiring_error.h"
#include MEMORY_HEADER

class CoreContext;

template<class T>
class CoreContextT;

/// <summary>
/// A virtual container which may be used to enumerate all children of a particular context
/// </summary>
/// <remarks>
/// The context enumerator will enumerate contexts in a preorder traversal.  A context enumerator is a minimally locked
/// collection, and is guaranteed to iterate over a context's children safely in a multithreaded way even if the context
/// being enumerated is modified while the enumeration takes place.
///
/// Iterators returned by this enumerator is guaranteed to traverse all children of the bound root context which were
/// children at the time of the first call to iterator::operator++.  It will also enumerate some of the children which
/// were added after the call to operator++.  It is guaranteed not to enumerate any context which is currently being
/// destroyed.
///
/// All context enumerators are inclusive, meaning that they will also enumerate the root.  The root is guaranteed to be
/// enumerated before any children.
/// </remarks>
class ContextEnumerator
{
public:
  /// <summary>
  /// Constructs a context enumerator for the current context
  /// </summary>
  ContextEnumerator(void);

  /// <summary>
  /// Constructs an enumerator which may enumerate all of the contexts rooted at the specified root
  /// </summary>
  /// <param name="root">The root context, optionally null</param>
  ContextEnumerator(const std::shared_ptr<CoreContext>& root);
  ~ContextEnumerator(void);

protected:
  // The root of the tree we're enumerating
  const std::shared_ptr<CoreContext> m_root;

public:
  /// The iterator class which is actually used in enumerating contexts
  class iterator {
  public:
    typedef std::forward_iterator_tag iterator_category;
    typedef const std::shared_ptr<CoreContext>& value_type;
    typedef size_t difference_type;
    typedef const std::shared_ptr<CoreContext>* pointer;
    typedef value_type reference;

    iterator(void) {}

    iterator(const std::shared_ptr<CoreContext>& root, const std::shared_ptr<CoreContext>& cur);

    ~iterator(void);

  protected:
    // Pointer to the ancestor of the root context:
    const std::shared_ptr<CoreContext> m_root;

    // Current context being inspected:
    std::shared_ptr<CoreContext> m_cur;

    void _next(const std::shared_ptr<CoreContext>& start);

  public:
    const iterator& NextSibling(void);
    
    // Operator overloads:
    const iterator& operator++(void);
    iterator operator++(int);

    const std::shared_ptr<CoreContext>& operator*(void) const { return m_cur; }
    const std::shared_ptr<CoreContext>& operator->(void) const { return m_cur; }
    bool operator==(const iterator& rhs) const { return m_root == rhs.m_root && m_cur == rhs.m_cur; }
    bool operator!=(const iterator& rhs) const { return !(*this == rhs); }
    explicit operator bool(void) const { return !!m_cur.get(); }
  };

  // Standard STL duck interface methods:
  iterator begin(void) { return iterator(m_root, m_root); };
  iterator end(void) { return iterator(m_root, std::shared_ptr<CoreContext>()); }
};

/// CurrentContextEnumerator
class CurrentContextEnumerator:
  public ContextEnumerator
{
public:
  /// <summary>
  /// Constructs an enumerator based on the current context
  /// </summary>
  CurrentContextEnumerator(void);
};

/// CurrentContextEnumeratorT
template<class Sigil>
class ContextEnumeratorT:
  public ContextEnumerator
{
public:
  ContextEnumeratorT()
  {}
  ContextEnumeratorT(const std::shared_ptr<CoreContext>& ctxt) :
    ContextEnumerator(ctxt)
  {}

  /// CurrentContextEnumeratorT iterator
  class iterator:
    public ContextEnumerator::iterator
  {
  public:
    iterator(const std::shared_ptr<CoreContext>& root, const std::shared_ptr<CoreContext>& cur = std::shared_ptr<CoreContext>()) :
      ContextEnumerator::iterator(root, cur)
    {
      // Get to the next sigil type, if we didn't start off on one
      _advance();
    }

  private:
    void _advance(void) {
      // Increment to the next sigil type:
      while(*this && !(***this).template Is<Sigil>())
        ContextEnumerator::iterator::operator++();
    }

  public:
    const iterator& operator++(void) {
      ContextEnumerator::iterator::operator++();
      _advance();
      return *this;
    }

    iterator operator++(int) {
      auto retVal = *this;
      ContextEnumerator::iterator::operator++(0);
      _advance();
      return retVal;
    }

    std::shared_ptr<CoreContextT<Sigil>> operator*(void) const { return std::static_pointer_cast<CoreContextT<Sigil>>(m_cur); }
    const CoreContext& operator->(void) const { return ***this; }
  };

  // Convenience routine for returning a single unique element
  std::shared_ptr<CoreContext> unique(void) {
    iterator q = begin();

    // If there are no elements in this enumerator (indicated by begin() == end()), then throw an error,
    // as this operation is undefined.
    if (q == end())
      throw autowiring_error("Attempted to get a unique context on a context enumerator that enumerates no children");

    iterator r = q;

    // If advancing q gets us to the end, then we only have one element and we can return success
    if (++q == end())
      return *r;

    throw autowiring_error("Attempted to get a unique context on a context enumerator that enumerates more than one child");
  }

  // Standard STL duck interface methods:
  iterator begin(void) { return iterator(m_root, m_root); };
  iterator end(void) { return iterator(m_root); }
};
