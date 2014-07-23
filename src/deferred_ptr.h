#pragma once
#include MEMORY_HEADER

/// <summary>
/// Implements a weak pointer type which may optionally be promoted to contain a shared pointer
/// </summary>
template<class T>
class deferred_ptr:
  std::weak_ptr<T>
{
public:
  deferred_ptr(void) {}

  deferred_ptr(const std::shared_ptr<T>& ptr, bool obtain = false):
    std::weak_ptr<T>(ptr)
  {
    if(obtain)
      interior = ptr;
  }

private:
  // The optionally obtained interior pointer
  mutable std::shared_ptr<T> interior;

public:
  /// <summary>
  /// Locks and assigns the interior shared pointer to the enclosed type
  /// </summary>
  /// <remarks>
  /// This method is idempotent
  /// </remarks>
  const std::shared_ptr<T>& obtain(void) const {
    return interior = lock();
  }

  // The only methods we allow from the base type:
  using std::weak_ptr<T>::lock;
  using std::weak_ptr<T>::expired;
};
