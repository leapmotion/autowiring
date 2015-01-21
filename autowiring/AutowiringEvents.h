// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "EventRegistry.h"
#include TYPE_INDEX_HEADER

struct ObjectTraits;
class CoreContext;

/// <summary>
/// These events are broadcast internally by Autowiring
/// for visualizing internal behavior.
/// </summary
class AutowiringEvents {
public:
  AutowiringEvents(void);
  virtual ~AutowiringEvents(void);
  
  virtual void NewContext(CoreContext&)=0;
  virtual void ExpiredContext(CoreContext&)=0;
  virtual void NewObject(CoreContext&, const ObjectTraits&)=0;
};

// Extern explicit template instantiation declarations added to prevent
// exterior instantation of internally used template instances
extern template class RegEvent<AutowiringEvents>;
extern template class TypeUnifierComplex<AutowiringEvents>;
