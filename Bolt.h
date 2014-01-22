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

template<class Sigil1, class Sigil2 = void, class Sigil3 = void>
class Bolt:
  public BoltBase
{
  const t_TypeInfoVector& GetContextSigils(void){
    static t_TypeInfoVector v;
    if( v.empty() ) {
      v.push_back(typeid(Sigil1));
      v.push_back(typeid(Sigil2));
      v.push_back(typeid(Sigil3));
    }
    return v;
  }
  static Sigil1 MySigilType1;
  static Sigil2 MySigilType2;
  static Sigil1 MySigilType3;
};

template<class Sigil1>
class Bolt<Sigil1,void,void>:
  public BoltBase
{
public:
  const t_TypeInfoVector& GetContextSigils(void){
    static t_TypeInfoVector v;
    if( v.empty() ) {
      v.push_back(typeid(Sigil1));
    }
    return v;
  }
  static Sigil1 MySigilType1;
};

template<class Sigil1, class Sigil2>
class Bolt<Sigil1,Sigil2,void>:
  public BoltBase
{
  const t_TypeInfoVector& GetContextSigils(void){
    static t_TypeInfoVector v;
    if( v.empty() ) {
      v.push_back(typeid(Sigil1));
      v.push_back(typeid(Sigil2));
    }
    return v;
  }
  static Sigil1 MySigilType1;
  static Sigil2 MySigilType2;
};
