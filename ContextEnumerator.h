#pragma once
#include MEMORY_HEADER

class CoreContext;

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
/// </remarks>
class ContextEnumerator
{
public:
  /// <summary>
  /// Constructs an enumerator which may enumerate all of the contexts rooted at the specified root
  /// </summary>
  ContextEnumerator(const std::shared_ptr<CoreContext>& root);
  ~ContextEnumerator(void);

private:
  // The root of the tree we're enumerating
  const std::shared_ptr<CoreContext> m_root;

public:
  // The iterator class which is actually used in enumerating contexts
  class iterator {
  public:
    iterator(void) {}

    iterator(const std::shared_ptr<CoreContext>& root, const std::shared_ptr<CoreContext>& cur = std::shared_ptr<CoreContext>()) :
      m_root(root),
      m_cur(cur)
    {
    }

    ~iterator(void);

  private:
    // Root context pointer:
    const std::shared_ptr<CoreContext> m_root;

    // Current context being inspected:
    std::shared_ptr<CoreContext> m_cur;

  public:
    // Operator overloads:
    const iterator& operator++(void);
    const std::shared_ptr<CoreContext>& operator*(void) const { return m_cur; }
    const std::shared_ptr<CoreContext>& operator->(void) const { return m_cur; }
    bool operator==(const iterator& rhs) const { return m_root == rhs.m_root && m_cur == rhs.m_cur; }
    bool operator!=(const iterator& rhs) const { return m_root != rhs.m_root || m_cur != rhs.m_cur; }
  };

  // Standard STL duck interface methods:
  iterator begin(void) { return iterator(m_root, m_root); };
  iterator end(void) { return iterator(m_root); }
};


class CurrentContextEnumerator:
  public ContextEnumerator
{
public:
  /// <summary>
  /// Constructs an enumerator based on the current context
  /// </summary>
  CurrentContextEnumerator(void);
};