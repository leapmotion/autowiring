// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#pragma once
#include "CoreContext.h"

/// A special class designed to make it easier to detect when our context is the global context
class GlobalCoreContext:
  public CoreContextT<GlobalCoreContext>
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
  /// <returns>
  /// A reference to the global context.  The caller may test `std::shared_ptr::unique`
  /// to determine whether the pointer is the last outstanding reference to the global
  /// context.
  /// </returns>
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
  static std::shared_ptr<GlobalCoreContext> Release(void);

private:
  // Global context shared pointer and lock:
  static std::mutex& getInitLock();
  static std::shared_ptr<GlobalCoreContext>& getGlobalContextSharedPtr();
};

/// <summary>
/// Obtains the global context, provided at global scope to allow forward declaration
/// </summary>
std::shared_ptr<GlobalCoreContext> GetGlobalContext(void);
