#ifndef _DEPENDENT_CONTEXT_H
#define _DEPENDENT_CONTEXT_H

template<class T>
struct DependentContext:
  public T
{
  Autowired<CoreContext> m_context;
};

#endif