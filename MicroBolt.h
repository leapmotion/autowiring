#pragma once
#include "Bolt.h"
#include "CoreContext.h"
#include <vector>

typedef void(*ctxtfnptr)(std::shared_ptr<CoreContext>);

struct MicroBoltUtilities {
  /// <summary>
  /// Instantiates and returns the SAME EXACT Vector per given Sigil Class and Container Type
  /// </summary>
  template<class SigilClass>
  static std::vector<ctxtfnptr>& GetBoltEnumerationList(void) {
    static std::vector<ctxtfnptr> retVal; //runs once at init
    return retVal;
  }

  /// <summary>
  /// Makes sure any given ContainerType elt is inserted only once into given ContainerType vector 
  /// </summary>
  template<class SigilClass, ctxtfnptr callback>
  static void RegisterCallbackWithSigil(void) {
    static bool initialized = PerformInsert<SigilClass, callback>();
  }

  /// <summary>
  /// Enumerates all microbolts for given sigilclass and containertype
  /// </summary>
  template<class SigilClass>
  static void AddBoltsToContext(std::shared_ptr<CoreContext> ctxtptr) {
    auto& list = GetBoltEnumerationList<SigilClass>();
    for(size_t i = 0; i < list.size(); i++){
      // Call every callable in the vector, passing a ref to me each time
      list[i](ctxtptr);
    }
  }

private:
  template<class SigilClass, ctxtfnptr callback>
  static bool PerformInsert(void) {
    auto& list = GetBoltEnumerationList<SigilClass>();
    list.push_back(callback);
    return true;
  }
};

template<class SigilClass, ctxtfnptr callback>
struct MicroBolt {
  MicroBolt(void) {
    MicroBoltUtilities::RegisterCallbackWithSigil<SigilClass, callback>();
	}
};

template<class T>
void InsertNameIntoContext(std::shared_ptr<CoreContext> cptr) {
  // We will allow this to throw an exception if a contradictory
  // or duplicated bolt registration exists
  cptr->Add<T>(std::shared_ptr<T>(new T));
}

// Bolts a particular concrete type into the specified context, identified by its sigil
#define BOLT_TO(SigilClass, TypeToInsert) namespace { \
    MicroBolt<SigilClass, &InsertNameIntoContext<TypeToInsert>> s_##SigilClass; \
  }

// Causes the specified function to be invoked when a context is created whose sigil matches
// the specified sigil
#define BOLT_TO_FUNCTION(SigilClass, fptr) namespace { \
    MicroBolt<SigilClass, fptr> s_##SigilClass##Bolted_Function; \
  }



