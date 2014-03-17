#pragma once

#include "BoltBase.h"
#include "Decompose.h"
#include TYPE_INDEX_HEADER

/// <summary>
/// </summary>
/// <param name="Sigil">The sigils of the contexts whose creation we want to listen for</param>
/// <remarks>
/// A bolt is a way to augment the creation of a new context in a declarative, by-name manner.
/// By inheriting from Bolt, the object will recieve ContextCreated() calls.
/// To create a class that will have a new instance inserted into each instance of a context
/// with a given sigil, use Boltable.
/// </remarks>

template<typename... Sigil>
class Bolt:
  public BoltBase
{
public:
  Bolt(void) {
    bool dummy[] = {
      (m_BoltedTypes.push_back(typeid(Sigil)), false)...
    };
    (void) dummy;
  }

  const t_TypeInfoVector& GetContextSigils(void){
    return m_BoltedTypes;
  }
  
  static_assert(!is_any_same<void, Sigil...>::value, "Can't use 'void' as a sigil type");
private:
  t_TypeInfoVector m_BoltedTypes;
};

template<>
class Bolt<>:
  public BoltBase
{
public:
  const t_TypeInfoVector& GetContextSigils(void){
    return m_BoltedTypes;
  }
private:
  t_TypeInfoVector m_BoltedTypes;
};
