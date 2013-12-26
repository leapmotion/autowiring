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

class NoSigil;

template<class Sigil1, class Sigil2 = NoSigil, class Sigil3 = NoSigil>
class Bolt:
  public BoltBase
{
  const t_TypeInfoVector& GetContextSigils(void){
    static t_TypeInfoVector v;
    if( v.empty() ) {
      v.push_back(typeid(Sigil1));
      v.push_back(typeid(Sigil2));
      v.push_back(typeid(Sigil3));
      v.shrink_to_fit();
    }
    return {typeid(Sigil1),typeid(Sigil2),typeid(Sigil3),0};
  }
  static Sigil1 MySigilType1;
  static Sigil2 MySigilType2;
  static Sigil1 MySigilType3;
};

template<class Sigil1>
class Bolt<Sigil1,NoSigil,NoSigil>:
  public BoltBase
{
public:
  const t_TypeInfoVector& GetContextSigils(void){
    static t_TypeInfoVector v;
    if( v.empty() ) {
      v.push_back(typeid(Sigil1));
      v.shrink_to_fit();
    }
    return v;
  }
  static Sigil1 MySigilType1;
};

template<class Sigil1, class Sigil2>
class Bolt<Sigil1,Sigil2,NoSigil>:
  public BoltBase
{
  const t_TypeInfoVector& GetContextSigils(void){
    static t_TypeInfoVector v;
    if( v.empty() ) {
      v.push_back(typeid(Sigil1));
      v.push_back(typeid(Sigil2));
      v.shrink_to_fit();
    }
    return v;
  }
  static Sigil1 MySigilType1;
  static Sigil2 MySigilType2;
};
