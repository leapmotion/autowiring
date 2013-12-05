// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _DEFERRED_CREATION_NOTICE_H
#define _DEFERRED_CREATION_NOTICE_H
#include SHARED_PTR_HEADER

class CoreContext;

/// <summary>
/// A utility class which, when destroyed, broadcasts a context creation notice
/// </summary>
/// <remarks>
/// </remarks>
class DeferredCreationNotice
{
public:
  DeferredCreationNotice(const std::type_info* pSigil, std::shared_ptr<CoreContext> ctxt);
  ~DeferredCreationNotice(void);

private:
  // The sigil of the created context, or null if the context was not created
  const std::type_info* m_pSigil;

  // The created context
  std::shared_ptr<CoreContext> m_ctxt;

public:
  bool WasCreated(void) const { return m_pSigil != nullptr; }
  const std::shared_ptr<CoreContext>& GetContext(void) const {return m_ctxt;}

  operator const std::shared_ptr<CoreContext>&(void) const {return GetContext();}
};

#endif
