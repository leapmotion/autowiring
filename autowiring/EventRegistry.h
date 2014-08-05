// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include <typeinfo>
#include TYPE_TRAITS_HEADER
#include STL_TUPLE_HEADER
#include MEMORY_HEADER

template<class T>
class JunctionBox;

class JunctionBoxBase;
class Object;

// Checks if an Object* listens to a event T;
struct EventIdentifierBase {
  virtual bool IsSameAs(const Object* obj) = 0;
  virtual const std::type_info& Type() = 0;
};

template<typename T>
struct EventIdentifier:
  public EventIdentifierBase
{
  // true if "obj" is an event receiver for T
  bool IsSameAs(const Object* obj){
    return !!dynamic_cast<const T*>(obj);
  }
  
  const std::type_info& Type(){
    return typeid(T);
  }
};

struct EventRegistryEntry {
  EventRegistryEntry(const std::type_info& ti);
  
  // Next entry in the list:
  const EventRegistryEntry* const pFlink;
  
  // Type of this entry:
  const std::type_info& ti;
  
  /// <summary>
  /// The runtime type information corresponding to this entry
  /// </summary>
  virtual const std::type_info& GetTypeInfo(void) const = 0;
  
  /// <summary>
  /// Constructor method, used to generate a new junction box
  /// </summary>
  virtual std::shared_ptr<JunctionBoxBase> NewJunctionBox(void) const = 0;
  
  /// <summary>
  /// Used to create a type identifier value, for use with AutoNet
  /// </summary>
  virtual std::shared_ptr<EventIdentifierBase> NewEventIdentifier(void) const = 0;
};

template<class T>
struct EventRegistryEntryT:
  public EventRegistryEntry
{
  EventRegistryEntryT(void):
    EventRegistryEntry(typeid(T))
  {}
  
  virtual const std::type_info& GetTypeInfo(void) const override { return typeid(T); }


  virtual std::shared_ptr<JunctionBoxBase> NewJunctionBox(void) const override {
    return std::static_pointer_cast<JunctionBoxBase>(
      std::make_shared<JunctionBox<T>>()
    );
  }

  std::shared_ptr<EventIdentifierBase> NewEventIdentifier(void) const override {
    return std::static_pointer_cast<EventIdentifierBase>(
      std::make_shared<EventIdentifier<T>>()
    );
  }
};

extern const EventRegistryEntry* g_pFirstEventEntry;
extern size_t g_eventEntryCount;

/// <summary>
/// Adds the specified type to the universal event registry
/// </summary>
/// <remarks>
/// Any instance of this event registry parameterized on type T will be added to the
/// global static event registry, and this registry is computed at link time.
/// </remarks>
template<class T>
class RegEvent
{
public:
  static const EventRegistryEntryT<T> r;
};

template<class T>
const EventRegistryEntryT<T> RegEvent<T>::r;
