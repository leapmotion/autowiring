#pragma once
#include <vector>
#include FUNCTIONAL_HEADER
#include RVALUE_HEADER
#include MEMORY_HEADER

/// <summary>
/// An ownership validator class, used to guarantee shared pointer expiration in its dtor
/// </summary>
/// <remarks>
/// This class allows consumers to ensure that the weak pointers added to it are all expired
/// when instances are destroyed.  The specified listeners are invoked when a weak pointer
/// is encountered which is not properly destroyed.  If there are no listeners, an assert is
/// used to raise a debugger event in debug mode.
/// </remarks>
class SimpleOwnershipValidator
{
public:
  ~SimpleOwnershipValidator(void);

  // The standard output stream used by the validator to report errors, just prints
  // to stderr
  static void PrintToStdErr(const std::vector<const std::type_info*>& violating);

private:
  struct EntryBase {
    virtual ~EntryBase(void) {}
    virtual const std::type_info& GetType(void) const = 0;
    virtual bool Validate(void) const = 0;
  };

  template<class T>
  struct Entry:
    EntryBase
  {
    Entry(std::weak_ptr<T>&& ptr) :
      m_ptr(std::move(ptr))
    {}

    std::weak_ptr<T> m_ptr;

    const std::type_info& GetType(void) const override { return typeid(T); }
    bool Validate(void) const override { return m_ptr.expired(); }
  };

  std::vector<EntryBase*> m_entries;

  std::vector<std::function<void(const std::vector<const std::type_info*>&)>> listeners;

protected:
  /// <summary>
  /// Adds a new entry whose expiration is to be checked at destruction time
  /// </summary>
  template<class T>
  void PendValidation(std::weak_ptr<T>&& entry) {
    m_entries.push_back(new Entry<T>(std::move(entry)));
  }

public:
  /// <summary>
  /// Adds a listener who is to be notified of unchecked entities at destruction time
  /// </summary>
  void AddValidationListener(const std::function<void(const std::vector<const std::type_info*>&)>& fn);
};

