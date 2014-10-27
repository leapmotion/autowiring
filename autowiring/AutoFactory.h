#pragma once
#include "ContextMember.h"

/// <summary>
/// A proxy context member that knows how to create a factory for a particular type
/// </summary>
/// <remarks>
/// This is an internal routine!  Don't try to use it yourself!  If you would like to
/// register yourself as a factory producing a certain type, use the static new method
/// which has one of the signatures defined <see ref="factorytype">factorytype</see>
/// </remarks>
template<class Factory, class Type>
class AutoFactory final:
  public ContextMember
{
public:
  void Inject(CoreContext& ctxt) {

  }
};