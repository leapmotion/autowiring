#pragma once
#include <iosfwd>
#include <vector>
#include SHARED_PTR_HEADER

class SimpleOwnershipValidator
{
public:
  SimpleOwnershipValidator();
  ~SimpleOwnershipValidator();

  // The standard output stream used by the validator to report errors

private:
  struct EntryBase {
    virtual ~EntryBase(void) {}
    virtual const std::type_info& GetType(void) = 0;
    virtual bool Validate(void) = 0;
  };

  template<class T>
  struct Entry:
    EntryBase
  {
    Entry(std::weak_ptr<T> ptr) :
      m_ptr(ptr)
    {}

    std::weak_ptr<T> m_ptr;

    const std::type_info& GetType(void) const { return typeid(T); }

    bool Validate(void) override {
      return m_ptr.expired();
    }
  };

  std::vector<EntryBase*> m_entries;

protected:
  /// <summary>
  /// Adds a new entry whose expiration is to be checked at destruction time
  /// </summary>
  template<class T>
  void PendValidation(std::weak_ptr<T>& entry) {
    if(IS_INTERNAL_BUILD)
      m_entries.push_back(new Entry<T>(entry));
  }
};

