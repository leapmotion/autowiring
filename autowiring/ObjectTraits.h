// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include <typeinfo>
#include MEMORY_HEADER

#include "fast_pointer_cast.h"
#include "AnySharedPointer.h"
#include "AutoFilterDescriptor.h"

#include "Object.h"
#include "ContextMember.h"
#include "CoreRunnable.h"
#include "BasicThread.h"
#include "ExceptionFilter.h"
#include "BoltBase.h"

#include "EventRegistry.h"

/// <summary>
/// Mapping and extraction structure used to provide a runtime version of an Object-implementing shared pointer
/// </summary>
struct ObjectTraits {
  template<class T>
  ObjectTraits(const std::shared_ptr<typename SelectTypeUnifier<T>::type>& value, T*) :
    type(typeid(T)),
    value(value),
    subscriber(MakeAutoFilterDescriptor(value)),
    pObject(autowiring::fast_pointer_cast<Object>(value)),
    pContextMember(autowiring::fast_pointer_cast<ContextMember>(value)),
    pCoreRunnable(autowiring::fast_pointer_cast<CoreRunnable>(value)),
    pBasicThread(autowiring::fast_pointer_cast<BasicThread>(value)),
    pFilter(autowiring::fast_pointer_cast<ExceptionFilter>(value)),
    pBoltBase(autowiring::fast_pointer_cast<BoltBase>(value)),
    receivesEvents(
      [this]{
        for (auto evt = g_pFirstEventEntry; evt; evt = evt->pFlink) {
          auto identifier = evt->NewTypeIdentifier();
          if (identifier->IsSameAs(pObject.get()))
            return true;
        }
        return false;
      }()
    )
  {
    if(!pObject)
      throw autowiring_error("Cannot add a type which does not implement Object");
  }

  // The declared original type:
  const std::type_info& type;

  // A holder to store the original shared pointer, to ensure that type information propagates
  // correctly on the right-hand side of our map
  const AnySharedPointer value;

  // The packet subscriber introduction method, if appropriate:
  const AutoFilterDescriptor subscriber;

  // There are a lot of interfaces we support, here they all are:
  const std::shared_ptr<Object> pObject;
  const std::shared_ptr<ContextMember> pContextMember;
  const std::shared_ptr<CoreRunnable> pCoreRunnable;
  const std::shared_ptr<BasicThread> pBasicThread;
  const std::shared_ptr<ExceptionFilter> pFilter;
  const std::shared_ptr<BoltBase> pBoltBase;

  // Does this type receive events?
  const bool receivesEvents;
};