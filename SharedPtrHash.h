#ifndef _SHARED_PTR_HASH_H
#define _SHARED_PTR_HASH_H
#include <hash_set>
#include <memory>

/// <summary>
/// A simple hash class that allows the use of a shared pointer in a hash_set
/// </summary>
template<class T>
class SharedPtrHash:
  public std::hash_compare<std::shared_ptr<T>>
{
public:
  size_t operator()(const std::shared_ptr<T>& _Key) const {
    return (size_t)_Key.get();
  }

  using std::hash_compare<std::shared_ptr<T>>::operator();
};

#endif