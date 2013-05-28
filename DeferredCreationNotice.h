#ifndef _DEFERRED_CREATION_NOTICE_H
#define _DEFERRED_CREATION_NOTICE_H
#include <memory>

class CoreContext;

/// <summary>
/// A utility class which, when destroyed, broadcasts a context creation notice
/// </summary>
/// <remarks>
/// </remarks>
class DeferredCreationNotice
{
public:
  DeferredCreationNotice(const char* name, std::shared_ptr<CoreContext> ctxt);
  ~DeferredCreationNotice(void);

private:
  // The name of the created context, or null if the context was not created
  const char* m_name;

  // The created context
  std::shared_ptr<CoreContext> m_ctxt;

public:
  bool WasCreated(void) const {return m_name == nullptr;}
  const std::shared_ptr<CoreContext>& GetContext(void) const {return m_ctxt;}
};

#endif