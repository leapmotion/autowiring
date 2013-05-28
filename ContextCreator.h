#ifndef _CONTEXT_CREATOR_H
#define _CONTEXT_CREATOR_H
#include "ContextCreatorBase.h"
#include "CoreContext.h"
#include STL_UNORDERED_MAP

/// <summary>
/// Implements a foundation class that allows named context registration and augmentation
/// </summary>
/// <param name="contextName">A unique name for this context type</param>
/// <param name="Key">A key type used to identify this context</param>
/// <remarks>
/// This class helps manage the creation of contexts with global names.  When the new child context
/// is created, a notification is broadcast throughout the entire current context to any registered
/// ContextCreationListener.
/// </remarks>
template<const char* contextName, class Key>
class ContextCreator:
  public ContextCreatorBase
{
public:
  ContextCreator(void) {
    m_context = CoreContext::CurrentContext();
  }

private:
  typedef std::unordered_map<Key, std::shared_ptr<CoreContext>> t_mpType;
  t_mpType m_mp;

  std::weak_ptr<CoreContext> m_context;

public:
  /// <summary>
  /// Attempts to find a context with the specified key
  /// </summary>
  std::shared_ptr<CoreContext> FindContext(const Key& key) {
    typename t_mpType::iterator q = m_mp.find(key);
    if(q == m_mp.end())
      return std::shared_ptr<CoreContext>();
    return q->second;
  }

  /// <summary>
  /// Finds or creates a context with the specified key, dependent upon the current context
  /// </summary>
  std::shared_ptr<CoreContext> Create(const Key& key) {
    // Try to find a context already existing with the given key:
    std::shared_ptr<CoreContext>& retVal = m_mp[key];
    if(!retVal) {
      // Attempt to lock the context.  Could already be destroyed by this point.
      std::shared_ptr<CoreContext> context = m_context.lock();
      if(context) {
        // Create:
        retVal = context->Create();

        // Notify all listeners that this creation event has taken place:
        context->BroadcastContextCreationNotice(contextName, retVal);
      }
    }
    return retVal;
  }

  /// <summary>
  /// Removes the specified context by its key
  /// </summary>
  void Remove(const Key& key) {
    m_mp.erase(key);
  }
};

#endif
