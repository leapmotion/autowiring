#ifndef _DEPENDENT_CONTEXT_H
#define _DEPENDENT_CONTEXT_H

class CoreContext;

template<class T>
class DependentContext:
  public T
{
public:
  DependentContext(const cpp11::shared_ptr<CoreContext>& context):
    m_context(context)
  {}

  cpp11::shared_ptr<CoreContext> m_context;
};

#endif