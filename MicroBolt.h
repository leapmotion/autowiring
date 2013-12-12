#pragma once
#include "Bolt.h"
#include "CreationRules.h"
#include "Decompose.h"
#include <vector>

/// <summary>
/// A default bolt type which will insert the specified type into a matching context
/// </summary>
template<class Sigil, class T>
struct MicroBolt:
  public Bolt<Sigil>
{
public:
  void ContextCreated(void) override;
};

template<class T>
struct MicroBolt<void, T> {};

#include "CoreContext.h"

template<class Sigil, class T>
void MicroBolt<Sigil, T>::ContextCreated(void) {
  std::shared_ptr<T> ptr;
  AutoCurrentContext ctxt;
  ctxt->FindByType(ptr);
  if(ptr)
    return;

  ptr.reset(CreationRules::New<T>());
  ctxt->Add(ptr);
}

/// <summary>
/// Causes the class inheriting from this definition to be bolted to the specified contexts
/// </summary>
template<class Sigil1, class Sigil2 = void, class Sigil3 = void>
struct Boltable {};
