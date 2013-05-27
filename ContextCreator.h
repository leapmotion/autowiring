#ifndef _CONTEXT_CREATOR_H
#define _CONTEXT_CREATOR_H

/// <summary>
/// Implements a foundation class that allows named context registration and augmentation
/// </summary>
/// <param name="contextName">A unique name for this context type</param>
/// <param name="Key">A key type used to identify this context</param>
/// <remarks>
/// A context creator is a type of foundation
/// </remarks>
template<class Key, const char* contextName>
class ContextCreator
{
public:
  ContextCreator(void);
  ~ContextCreator(void);
};

#endif