#ifndef _GLOBAL_CORE_CONTEXT
#define _GLOBAL_CORE_CONTEXT

#include "CoreContext.h"

// A special class designed to make it easier to detect when our context is the global context
class GlobalCoreContext:
  public CoreContext
{
public:
  GlobalCoreContext(void):
    CoreContext(cpp11::shared_ptr<CoreContext>())
  {
  }

  /// <summary>
  /// Initializes, then destroys, a single instance of class W in local scope
  /// </summary>
  template<class W>
  static void InstantiateW() {
    W w;
  }

  /// <summary>
  /// A special method to load the core context with default fixtures.
  /// </summary>
  /// <param name="T">A "global structure" containing autowired pointers to all global members</param>
  /// <remarks>
  /// The default fixtures are generally required by all classes, but because not
  /// all consumers may want these objects, they must be explicitly instantiated.
  /// This method is idempotent.
  /// </remarks>
  template<class W>
  static void FillWithDefaults(void) {
    FillWithDefaults(&InstantiateW<W>);
  }
  
  /// <summary>
  /// Similar to the templated function, but takes a function pointer to the instantiator rather than the type
  /// </summary>
  static void FillWithDefaults(void (*instantiator)(void)) {
    cpp11::shared_ptr<CoreContext> priorCurrent = GetCurrentContext();
    cpp11::shared_ptr<GlobalCoreContext> ctxt = GetGlobalContext();
    ctxt->SetCurrent();

    // Do the instantiation in this scope
    instantiator();

    // Reset the current context:
    priorCurrent->SetCurrent();
  }
};

/// <summary>
/// Alias of GlobalCoreContext::FillWithDefaults, provided here to allow forward declaration of this method
/// </summary>
void FillWithDefaults(void (*instantiator)(void));

#endif