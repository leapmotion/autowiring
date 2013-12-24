#pragma once
#include "AutowirableSlot.h"
#include "BoltBase.h"

/// <summary>
/// </summary>
/// <param name="Sigil">The sigil of the context whose creation we want to listen for</param>
/// <remarks>
/// A bolt is a way to augment the creation of a new context in a declarative, by-name manner.
/// By inheriting from Bolt, the object will recieve ContextCreated() calls.
/// To create a class that will have a new instance inserted into each instance of a context
/// with a given sigil, use Boltable.
/// </remarks>
template<class Sigil>
class Bolt:
  public BoltBase
{
public:
  const std::type_info& GetContextSigil(void) override {
    return typeid(Sigil);
  }
  static Sigil MySigilType;
};