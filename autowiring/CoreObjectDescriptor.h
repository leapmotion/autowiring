// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AnySharedPointer.h"
#include "AutoFilterDescriptor.h"
#include "BoltBase.h"
#include "ContextMember.h"
#include "CoreRunnable.h"
#include "BasicThread.h"
#include "ExceptionFilter.h"
#include "fast_pointer_cast.h"
#include "CoreObject.h"
#include <typeinfo>
#include MEMORY_HEADER

/// <summary>
/// Mapping and extraction structure used to provide a runtime version of an Object-implementing shared pointer
/// </summary>
/// <remarks>
/// This type is used to describe an object that is a member of a context.  This descriptor structure performs
/// all of the type rule induction work that CoreContext performs, and provides users with a way to provide a
/// runtime description of a type.  This is critical in cases where a type is not available at compile time--
/// for instance, if that type is provided in another module that is dynamically linked.
/// </remarks>
struct CoreObjectDescriptor {
  template<typename TActual, typename T>
  CoreObjectDescriptor(const std::shared_ptr<TActual>& value, T*) :
    type(&typeid(T)),
    actual_type(&typeid(*value)),
    stump(&SlotInformationStump<T>::s_stump),
    value(value),
    subscriber(MakeAutoFilterDescriptor(value)),
    pCoreObject(autowiring::fast_pointer_cast<CoreObject>(value)),
    pContextMember(autowiring::fast_pointer_cast<ContextMember>(value)),
    pCoreRunnable(autowiring::fast_pointer_cast<CoreRunnable>(value)),
    pBasicThread(autowiring::fast_pointer_cast<BasicThread>(value)),
    pFilter(autowiring::fast_pointer_cast<ExceptionFilter>(value)),
    pBoltBase(autowiring::fast_pointer_cast<BoltBase>(value)),
    receivesEvents(
      [this]{
        for (auto evt = g_pFirstEventEntry; evt; evt = evt->pFlink) {
          auto identifier = evt->NewTypeIdentifier();
          if (identifier->IsSameAs(pCoreObject.get()))
            return true;
        }
        // "T" not found in event registry
        return false;
      }()
    )
  {
    // We can instantiate casts to CoreObject here at the point where object traits are being generated
    (void) autowiring::fast_pointer_cast_initializer<CoreObject, TActual>::sc_init;
    (void) autowiring::fast_pointer_cast_initializer<TActual, CoreObject>::sc_init;
    (void) autowiring::fast_pointer_cast_initializer<CoreObject, T>::sc_init;
    (void) autowiring::fast_pointer_cast_initializer<T, CoreObject>::sc_init;
  }

  /// <summary>
  /// Special case where the declared type is also the true type
  /// </summary>
  template<typename T>
  CoreObjectDescriptor(const std::shared_ptr<T>& value) :
    CoreObjectDescriptor(value, static_cast<T*>(nullptr))
  {}

  // The type of the passed pointer
  const std::type_info* type;

  // The "actual type" used by Autowiring.  This type may differ from CoreObjectDescriptor::type in cases
  // where a type unifier is used, or if the concrete type is defined in an external module--for
  // instance, by a class factory.
  const std::type_info* actual_type;

  /// <summary>
  /// Used to obtain a list of slots defined on this type, for reflection purposes
  /// </summary>
  /// <returns>
  /// A pointer to the head of a linked list of slots on this context member
  /// </returns>
  /// <remarks>
  /// A slot is an Autowired field defined within a specific type.  Slots are of particular
  /// interest because they denote a compile-time relationship between two types, and generally
  /// are one way to understand class relationships in a system.  Furthermore, because of their
  /// compile-time nature, they are declarative and therefore denote a relationship between
  /// types, rather than states, which makes it easier to understand how slots are linked.
  ///
  /// The returned value is cached, and should not be freed or modified as it may be in use
  /// in other parts of the program.  The behavior of this method is undefined if it's called
  /// on an object before the object is fully constructed (for instance, if the method is
  /// invoked from a constructor).  This method will return correct results even if the
  /// ContextMember type was not the first inherited type.
  ///
  /// If this method returns a correct result at any point, then all subsequent calls to this
  /// method are guaranteed to return correct results, even in the aforementioned case where
  /// the method is called during construction.  This method is guaranteed to return correct
  /// results after the first instance of a concrete type is constructed.
  ///
  /// This list is guaranteed not to contain any AutowiredFast fields defined in the class.
  ///
  /// The linked list is guaranteed to be in reverse-sorted order
  /// </remarks>
  const SlotInformationStumpBase* stump;

  // A holder to store the original shared pointer, to ensure that type information propagates
  // correctly on the right-hand side of our map
  AnySharedPointer value;

  // The packet subscriber introduction method, if appropriate:
  AutoFilterDescriptor subscriber;

  // There are a lot of interfaces we support, here they all are:
  std::shared_ptr<CoreObject> pCoreObject;
  std::shared_ptr<ContextMember> pContextMember;
  std::shared_ptr<CoreRunnable> pCoreRunnable;
  std::shared_ptr<BasicThread> pBasicThread;
  std::shared_ptr<ExceptionFilter> pFilter;
  std::shared_ptr<BoltBase> pBoltBase;

  // Does this type receive events?
  bool receivesEvents;
};
