#pragma once
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
/// </remarks>
class ContextEnumerator
{
public:
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
  // The iterator class which is actually used in enumerating contexts
  class iterator {
  public:
    typedef std::input_iterator_tag iterator_category;
    typedef const std::shared_ptr<CoreContext>& value_type;
    typedef size_t difference_type;
    typedef const std::shared_ptr<CoreContext>* pointer;
    typedef value_type reference;

    iterator(void) {}

    iterator(const std::shared_ptr<CoreContext>& root, const std::shared_ptr<CoreContext>& cur = std::shared_ptr<CoreContext>()) :
      m_root(root),
      m_cur(cur)
    {
    }

    ~iterator(void);

  protected:
    // Root context pointer:
    const std::shared_ptr<CoreContext> m_root;

    // Current context being inspected:
    std::shared_ptr<CoreContext> m_cur;

  public:
    // Operator overloads:
    const iterator& operator++(void);
    const std::shared_ptr<CoreContext>& operator*(void) const { return m_cur; }
    const CoreContext& operator->(void) const { return ***this; }
    bool operator==(const iterator& rhs) const { return m_root == rhs.m_root && m_cur == rhs.m_cur; }
    bool operator!=(const iterator& rhs) const { return !(*this == rhs); }
    explicit operator bool(void) const { return !!m_cur.get(); }
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

template<class Sigil>
class ContextEnumeratorT:
  public ContextEnumerator
{
public:
  ContextEnumeratorT(const std::shared_ptr<CoreContext>& ctxt):
    ContextEnumerator(ctxt)
  {}

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

    std::shared_ptr<CoreContextT<Sigil>> operator*(void) const { return std::static_pointer_cast<CoreContextT<Sigil>>(m_cur); }
    const CoreContext& operator->(void) const { return ***this; }
  };

  // Standard STL duck interface methods:
  iterator begin(void) { return iterator(m_root, m_root); };
  iterator end(void) { return iterator(m_root); }
};