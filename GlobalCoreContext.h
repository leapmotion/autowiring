// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _GLOBAL_CORE_CONTEXT
#define _GLOBAL_CORE_CONTEXT
#include "AutowirableSlot.h"
#include "CoreContext.h"
#include <boost/thread/mutex.hpp>

template<class T>
struct EnableBoltInternal {

};

// A special class designed to make it easier to detect when our context is the global context
class GlobalCoreContext:
  public CoreContext
{
private:
  /// <summary>
  /// The global context ctor
  /// </summary>
  /// <param name="instantiator">A function which will fill the global context with global members</param>
  /// <remarks>
  /// In any single program, only one global context exists.  Once the global context
  /// is initialized, it cannot be destroyed or re-initialized.  Members added to the
  /// global context cannot be removed until the whole program terminates.  Users who
  /// wish to add members to the global context should do so only after consideration
  /// of necessity.
  /// </remarks>
  GlobalCoreContext(void);

public:
  ~GlobalCoreContext(void);

  /// <summary>
  /// Obtains the global core context, or initializes it if necessary
  /// </summary>
  static std::shared_ptr<GlobalCoreContext> Get(void);

  /// <summary>
  /// Releases the global context, allowing reinitialization
  /// </summary>
  /// <remarks>
  /// In addition to releasing the global core context, all initializer operations
  /// are dumped.  Note that releasing the context is not synonymous with destroying
  /// the global context--if there are shared_ptr references to the global context,
  /// those references will keep the global context resident.  Here are some places
  /// which may maintain shared_ptr references to the global contexts:
  ///
  /// * Held references via SetCurrent
  /// * Child contexts of the global context
  ///
  /// This method should only be used with great caution.  Note that, if the calling
  /// thread holds a reference to the current global context via SetCurrent, it is
  /// the caller's responsibility to manually release that reference via EvictCurrent.
  /// </remarks>
  static void Release(void) {
    // Release local:
    boost::lock_guard<boost::mutex> lk(getInitLock());
    getGlobalContextSharedPtr().reset();
  }

  /// <summary>
  /// Broadcasts a notice to any listener in the current context regarding a creation event on a particular context name
  /// </summary>
  /// <remarks>
  /// The broadcast is made without altering the current context.  Recipients expect that the current context will be the
  /// one about which they are being informed.
  /// </remarks>
  void BroadcastContextCreationNotice(const std::type_info& sigil) const;

  /// <summary>
  /// Allows a specifically named class to be bolted
  /// </summary>
  /// <remarks>
  /// If the specified type does not inherit from BoltTo, this method has no effect
  /// </remarks>
  template<class T>
  void Enable(void) {
    static_assert(!std::is_abstract<T>::value, "Cannot enable an abstract class for bolting");
    EnableInternal((T*)nullptr, (T*)nullptr);
  }

private:
  // Lists of event receivers, by name:
  typedef std::unordered_map<std::type_index, std::list<BoltBase*>> t_contextNameListeners;
  t_contextNameListeners m_nameListeners;

  // Adds a bolt proper to this context
  template<class T, class Sigil>
  void EnableInternal(T*, Bolt<Sigil>*) {
    std::shared_ptr<T> ptr;
    this->FindByType(ptr);
    if(ptr)
      return;
    
    CurrentContextPusher pshr(shared_from_this());
    ptr.reset(AutowirableSlot::New<T>());
    this->Add(ptr);
  }

  // Enables a boltable class
  template<class T, class Sigil1, class Sigil2, class Sigil3>
  void EnableInternal(T*, Boltable<Sigil1, Sigil2, Sigil3>*) {
    static MicroBolt<Sigil1, &InsertNameIntoContext<T>> s_sigil1;
    static MicroBolt<Sigil2, &InsertNameIntoContext<T>> s_sigil2;
    static MicroBolt<Sigil3, &InsertNameIntoContext<T>> s_sigil3;
  }

  void EnableInternal(...) {}

  // CoreContext overrides:
  void AddBolt(const std::shared_ptr<BoltBase>& pBase) override;

  // Global context shared pointer and lock:
  static inline boost::mutex& getInitLock() {
    static boost::mutex s_initLock;
    return s_initLock;
  }

  static inline std::shared_ptr<GlobalCoreContext>& getGlobalContextSharedPtr() {
    static std::shared_ptr<GlobalCoreContext> s_globalContextSharedPtr;
    return s_globalContextSharedPtr;
  }
};

/// <summary>
/// Obtains the global context, provided at global scope to allow forward declaration
/// </summary>
std::shared_ptr<GlobalCoreContext> GetGlobalContext(void);

/// <summary>
/// Provides a declarative way to set the global context
/// </summary>
template<class W>
struct GlobalContextDesignation {
  GlobalContextDesignation(void) {
    CurrentContextPusher pshr(GetGlobalContext());
    W w;
  }
};

#endif
