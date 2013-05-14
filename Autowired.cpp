#include "stdafx.h"
#include "Autowired.h"
#include "CoreContext.h"
#include "GlobalCoreContext.h"

// Autowired CoreContext:
void AutowiredCreator<CoreContext>::Create() {
  // We allow creation on a CoreContext
  // TODO:  Evaluate whether this idiom is a bad one
  // Create will not create in any other circumstance if the autowired
  // member is already set, but we do so here unconditionally.  Is this
  // a good idea?

  // Typically we must do an m_context->Add, but that's not needed here.
  // All we have to do is to create a context.  Contexts are required to
  // be members of themselves, meaning that the context ctor will handle
  // the addition for us.
  *this = CoreContext::CurrentContext()->Create();
  m_context = *this;
}

std::shared_ptr<CoreContext> GetGlobalContextAsCoreContext(void) {
  return std::static_pointer_cast<CoreContext, GlobalCoreContext>(GetGlobalContext());
}

Autowired<CoreContext>::Autowired(bool forceNew) {
  if(forceNew)
    Create();
  else
    *this = GetCurrentContext();
  AutowirableSlot::m_context = *this;
}
