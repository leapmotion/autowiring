#pragma once

#include "AutowirableSlot.h"
#include "BoltBase.h"
#include "C++11/cpp11.h"
#include TYPE_INDEX_HEADER

/// <summary>
/// </summary>
/// <param name="Sigil">The sigil of the context whose creation we want to listen for</param>
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
  Bolt() {
    bool dummy[] = {
      false,
      (m_BoltedTypes.push_back(typeid(Sigil)), false)...
    };
    (void)dummy;
  }
  
  const t_TypeInfoVector& GetContextSigils(void){
    return m_BoltedTypes;
  }
private:
  t_TypeInfoVector m_BoltedTypes;
};
