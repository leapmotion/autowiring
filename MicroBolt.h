#pragma once
#include "Bolt.h"
#include <vector>

struct MicroBoltUtilities {
  /// <summary>
  /// Instantiates and returns the SAME EXACT Vector per given Sigil Class and Container Type
  /// </summary>
  template<class SigilClass>
  static std::vector<void(*)()>& GetBoltEnumerationList(void) {
    static std::vector<void(*)()> retVal;
    return retVal;
  }

  /// <summary>
  /// Makes sure any given ContainerType elt is inserted only once into given ContainerType vector 
  /// </summary>
  template<class SigilClass, void(*callback)()>
  static bool RegisterCallbackWithSigil(void) {
    static bool initialized = PerformInsert<SigilClass, callback>();
    return initialized;
  }

  /// <summary>
  /// Enumerates all microbolts for given sigilclass and containertype
  /// </summary>
  template<class SigilClass>
  static void AddBoltsToContext(std::shared_ptr<CoreContext> ctxtptr);

private:
  template<class SigilClass, void(*callback)()>
  static bool PerformInsert(void) {
    auto& list = GetBoltEnumerationList<SigilClass>();
    list.push_back(callback);
    return true;
  }
};

// Include must be provided here due to include dependency arrangement
#include "CoreContext.h"

template<class SigilClass>
void MicroBoltUtilities::AddBoltsToContext(std::shared_ptr<CoreContext> ctxtptr) {
  auto& list = GetBoltEnumerationList<SigilClass>();
  for(size_t i = 0; i < list.size(); i++){
    // Call every callable in the vector, passing a ref to me each time
    list[i](ctxtptr);
  }
}

template<class SigilClass, void (*callback)()>
struct MicroBolt {
  MicroBolt(void) {
    MicroBoltUtilities::RegisterCallbackWithSigil<SigilClass, callback>();
	}
};

template<class T>
void InsertNameIntoContext(void) {
  // We will allow this to throw an exception if a contradictory
  // or duplicated bolt registration exists
  CoreContext::CurrentContext()->Add<T>(std::shared_ptr<T>(new T));
}

// Explicit enable routine, needed to ensure that microbolt modules are instantiated
template<class Type>
void ModuleAnchorPoint(void);

#define ANCHOR_POINT(TypeToInsert) \
  template<> void ModuleAnchorPoint<TypeToInsert>(void) {}

// Bolts a particular concrete type into the specified context, identified by its sigil
#define BOLT_TO(SigilClass, TypeToInsert) \
  MicroBolt<SigilClass, &InsertNameIntoContext<TypeToInsert>> s_##SigilClass##TypeToInsert;

#define BOLT_TO_AND_ANCHOR_POINT(SigilClass, TypeToInsert) \
  ANCHOR_POINT(TypeToInsert) \
  BOLT_TO(SigilClass, TypeToInsert)

// Causes the specified function to be invoked when a context is created whose sigil matches
// the specified sigil
#define BOLT_TO_FUNCTION(SigilClass, fptr) namespace { \
    MicroBolt<SigilClass, fptr> s_##SigilClass##Bolted_Function; \
  }

// Enables a particular class for microbolting
#define ANCHOR_MODULE_HERE(TypeToInsert) \
  static void(*const s_unreferenced##TypeToInsert)() = &ModuleAnchorPoint<TypeToInsert>;