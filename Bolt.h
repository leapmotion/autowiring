#pragma once
#include "BoltBase.h"

/// <summary>
/// </summary>
/// <param name="Sigil">The sigil of the context whose creation we want to listen for</param>
/// <remarks>
/// A bolt is a way to augment the creation of a new context in a declarative, by-name manner.
/// </remarks>
template<class Sigil>
class Bolt:
  public BoltBase
{
public:
  const std::type_info& GetContextSigil(void) override {
    return typeid(Sigil);
  }
};
