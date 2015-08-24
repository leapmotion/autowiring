// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AnySharedPointer.h"
#include "auto_signal.h"
#include "AutoFilterDescriptor.h"
#include "AutowirableSlot.h"
#include "AutowiringEvents.h"
#include "autowiring_error.h"
#include "Bolt.h"
#include "CoreRunnable.h"
#include "ContextMember.h"
#include "CallExtractor.h"
#include "CreationRules.h"
#include "CurrentContextPusher.h"
#include "ExceptionFilter.h"
#include "fast_pointer_cast.h"
#include "has_autoinit.h"
#include "InvokeRelay.h"
#include "JunctionBoxManager.h"
#include "member_new_type.h"
#include "MemoEntry.h"
#include "CoreObjectDescriptor.h"
#include "result_or_default.h"
#include "ThreadPool.h"
#include "TypeRegistry.h"
#include "TypeUnifier.h"

#include <list>
#include MEMORY_HEADER
#include TYPE_INDEX_HEADER
#include STL_UNORDERED_MAP

struct CoreContextStateBlock;
class AutoInjectable;
class BasicThread;
class BoltBase;
class CoreContext;
class DeferrableAutowiring;
class GlobalCoreContext;
class JunctionBoxBase;
class OutstandingCountTracker;

template<class T, class Fn>
class AutowirableSlotFn;

template<typename T>
class Autowired;

template<typename... Sigils>
struct Boltable;

template<class T>
class CoreContextT;

template<typename T>
class JunctionBox;

namespace autowiring {
  template<typename... Args>
  struct signal_relay_t;

  class ThreadPool;
}

/// \file
/// CoreContext definitions.

/// <summary>
/// The modes in which a context can shut down.
/// </summary>
enum class ShutdownMode {
  Graceful,   ///< Shut down gracefully by allowing threads to run down dispatch queues.
  Immediate   ///< Shut down immediately, do not attempt to run down thread dispatch queues.
};

class AutoSearchLambda {
public:
  /// <summary>
  /// Invoked at the conclusion of a FindByTypeRecursive search operation
  /// </summary>
  /// <remarks>
  /// No information is passed about the success or failure of the search operation.  Implementors of this interface are
  /// required to store enough context about the AnySharedPointer passed as the first argument to FindByTypeRecursive to
  /// determine the status of this operation, if that status is interesting.
  /// </remarks>
  virtual void operator()() const = 0;
};

class AutoSearchLambdaDefault:
  public AutoSearchLambda
{
public:
  // AutoSearchLambda overrides:
  void operator()() const override {}
};

/// <summary>
/// A top-level container class representing an autowiring domain, a minimum
/// broadcast domain, and a thread execution domain.
/// </summary>
/// <remarks>
/// A context is the basic unit of organization within an autowired application. The scope of a context
/// determines:
/// * How Autowired dependencies are resolved
/// * Who receives AutoFired events
/// * Thread ownership (BasicThread, CoreThread)
/// * AutoPacket filter graph scope
///
/// Dependencies can be injected into a context using Autowired or AutoRequired.
/// The system looks in the current context for an existing object of the required
/// type to satisfy the dependency. If one does not exist, it looks in parent contexts.
/// When using AutoRequired, a new instance of the required type is created if no
/// existing object is found. Otherwise, the dependoncy is satisfied when another
/// object of the same type (or subtype) is added to the context (or one of its
/// parents). This resolution system carries the restriction that only one instance
/// of an Autowired type can exist in the same branch of the context tree.
///
/// In addition, an Autowired member of a context exists for as long as that context.
/// exists.
///
/// Autowired dependencies do not need to meet any special requirements such as
/// inheriting from a particular Autowiring type or implementing a particular interface.
/// However, if a type contains Autowired members, instances of that type must be
/// Autowired to a context in order for the dependencies of their members to be satisfied.
///
/// Contexts can be created with ::AutoCreateContext and AutoCreateContextT.
/// The global context is created automatically; get a reference to it using AutoGlobalContext.
///
/// \include snippets/Context_Class_Create.txt
///
/// Contexts can be enumerated using the ContextEnumerator class. You can restrict enumeration
/// to those contexts marked with a given sigil with ContextEnumeratorT<Sigil>. You can also
/// enumerate the current context with CurrentContextEnumerator.
///
/// \include snippets/Context_Class_Enumerate.txt
///
/// Autowired members of a context can pass information using AutoFired events, as well as with a filter graph.
/// You can also pass data from one context to another in the same way. A context can also snoop on another context's
/// events and filter graph packets to receive data it wouldn't otherwise have access to. Use Bolt objects to receive
/// notification when a context of a particular sigil type is created. This allows you to set up snooping
/// whenever a particular type of context is created.
///
/// Events, threads, and filter graphs require that the context's Initiate() function is called.
/// </remarks>
class CoreContext:
  public std::enable_shared_from_this<CoreContext>
{
protected:
  typedef std::list<std::weak_ptr<CoreContext>> t_childList;
  CoreContext(const std::shared_ptr<CoreContext>& pParent, t_childList::iterator backReference, const std::type_info& sigilType);

public:
  // Asserted when the context is initiated
  autowiring::signal<void()> onInitiated;

  // Asserted when the context is actually running
  autowiring::signal<void()> onRunning;

  // Asserted when the context is being shut down
  autowiring::signal<void()> onShutdown;

  // Asserted when the context is tearing down but before members objects are destroyed or
  // any contained AutoWired fields are unlinked
  autowiring::signal<void(const CoreContext&)> onTeardown;

  virtual ~CoreContext(void);

  /// <summary>
  /// Returns a shared reference to the global context.
  /// </summary>
  /// \sa AutoGlobalContext, GlobalCoreContext
  static std::shared_ptr<CoreContext> GetGlobal(void);

protected:
  // A pointer to the parent context
  const std::shared_ptr<CoreContext> m_pParent;

  // Back-referencing iterator which refers to ourselves in our parent's child list:
  const t_childList::iterator m_backReference;

  // Sigil type, used during bolting
  const std::type_info& m_sigilType;

  // State block for this context:
  std::shared_ptr<CoreContextStateBlock> m_stateBlock;

  enum class State {
    // Not yet started
    NotStarted,

    // Parent context has been started, and this context can proceed directly to the Running state
    CanRun,

    // User has called Initiated, but the parent context cannot yet be run
    Initiated,

    // Parent context is in the Running state and this context was in the Initiated state.
    // Transition to the running state is automatic once the parent context transitions.
    Running,

    // Context is terminated, cleanup operations are underway.
    Shutdown,
    
    // Context was terminated without having been initiated
    Abandoned
  };

  State m_state = State::NotStarted;
  
  // Set if a thread is added and needs to be run
  bool m_beforeRunning = false;

  // Child contexts:
  t_childList m_children;

  // Lists of event receivers, by name.  The type index of "void" is reserved for
  // bolts for all context types.
  typedef std::unordered_map<std::type_index, std::list<BoltBase*>> t_contextNameListeners;
  t_contextNameListeners m_nameListeners;

  /// \internal
  /// <summary>
  /// A proxy context member that knows how to create a factory for a particular type
  /// </summary>
  /// <remarks>
  /// This is an internal routine!  Don't try to use it yourself!  If you would like to
  /// register yourself as a factory producing a certain type, use the static new method
  /// which has one of the signatures defined <see ref="factorytype">factorytype</see>
  /// </remarks>
  template<class T>
  class AutoFactory;

  /// \internal
  template<class T, class Fn>
  class AutoFactoryFn;

  // Simple list of concrete types
  std::list<CoreObjectDescriptor> m_concreteTypes;

  // This is a memoization map used to memoize any already-detected interfaces.
  mutable std::unordered_map<auto_id, MemoEntry> m_typeMemos;

  // All known context members, exception filters:
  std::vector<ContextMember*> m_contextMembers;
  std::vector<ExceptionFilter*> m_filters;

  // All known event receivers and receiver proxies originating from this context:
  typedef std::set<JunctionBoxEntry<CoreObject>> t_rcvrSet;
  t_rcvrSet m_eventReceivers;

  // List of eventReceivers to be added when this context in initiated
  t_rcvrSet m_delayedEventReceivers;
  
  // Context members from other contexts that have snooped this context
  std::set<AnySharedPointer> m_snoopers;

  // Manages events for this context
  const std::unique_ptr<JunctionBoxManager> m_junctionBoxManager;

  // Actual core threads:
  std::list<CoreRunnable*> m_threads;

  // The thread pool used by this context.  By default, a context inherits the thread pool of
  // its parent, and the global context gets the system thread pool.
  std::shared_ptr<autowiring::ThreadPool> m_threadPool;

  // The start token for the thread pool, if one exists
  std::shared_ptr<void> m_startToken;

  // Unlink flag
  bool m_unlinkOnTeardown = false;

  // Creation rules are allowed to refer to private methods in this type
  template<autowiring::construction_strategy, class T, class... Args>
  friend struct autowiring::crh;

protected:
  // Delayed creation routine
  typedef std::shared_ptr<CoreContext> (*t_pfnCreate)(
    std::shared_ptr<CoreContext> pParent,
    t_childList::iterator backReference
    );

  /// \internal
  /// <summary>
  /// Overload which does not perform injection
  /// </summary>
  std::shared_ptr<CoreContext> CreateInternal(t_pfnCreate pfnCreate);

  /// \internal
  /// <summary>
  /// Register new context with parent and notify others of its creation.
  /// </summary>
  /// <param name="pfnCreate">A creation routine which can create the desired context</param>
  /// <param name="inj">An injectable to be inserted into the context before bolts are fired</param>
  std::shared_ptr<CoreContext> CreateInternal(t_pfnCreate pfnCreate, AutoInjectable&& pInj);

  // Adds a bolt proper to this context
  template<typename T, typename... Sigils>
  void EnableInternal(T*, Bolt<Sigils...>*) {
    Inject<T>();
  }

  // Enables a boltable class
  template<typename T, typename... Sigils>
  void EnableInternal(T*, Boltable<Sigils...>*) {
    autowiring::noop((AutoRequireMicroBolt<T, Sigils>(),false)...);
  }

  void EnableInternal(...) {}

  template<typename T, typename... Sigil>
  void AutoRequireMicroBolt(void);

  /// \internal
  /// <summary>
  /// Unregisters all event receivers in this context
  /// </summary>
  void UnregisterEventReceiversUnsafe(void);

  /// \internal
  /// <summary>
  /// Broadcasts a notice to any listener in the current context regarding a creation event on a particular context name
  /// </summary>
  /// <remarks>
  /// The broadcast is made without altering the current context.  Recipients expect that the current context will be the
  /// one about which they are being informed.
  /// </remarks>
  void BroadcastContextCreationNotice(const std::type_info& sigil) const;

  /// \internal
  /// <summary>
  /// Satisfies all slots associated with the passed memo entry and causes finalizers to be invoked
  /// </summary>
  /// <remarks>
  /// The passed lock will be unlocked when this function returns
  /// </remarks>
  void SatisfyAutowiring(std::unique_lock<std::mutex>& lk, MemoEntry& entry);

  /// \internal
  /// <summary>
  /// Updates slots related to a single autowired field
  /// </summary>
  void UpdateDeferredElement(std::unique_lock<std::mutex>&& lk, MemoEntry& entry);

  /// \internal
  /// <summary>
  /// Updates all deferred autowiring fields, generally called after a new member has been added
  /// </summary>
  void UpdateDeferredElements(std::unique_lock<std::mutex>&& lk, const CoreObjectDescriptor& entry, bool local);

  /// \internal
  /// <summary>
  /// Adds the named event receiver to the collection of known receivers
  /// </summary>
  /// <param name="pRecvr">The junction box entry corresponding to the receiver type</param>
  void AddEventReceiver(const JunctionBoxEntry<CoreObject>& pRecvr);

  /// \internal
  /// <summary>
  /// Add delayed event receivers
  /// </summary>
  void AddEventReceiversUnsafe(t_rcvrSet&& receivers);

  /// \internal
  /// <summary>
  /// Removes all recognized event receivers in the indicated range
  /// </summary>
  void RemoveEventReceivers(const t_rcvrSet& receivers);

  /// \internal
  /// <summary>
  /// Adds an object of any kind to the IOC container
  /// </summary>
  /// <param name="pContextMember">The member which was added</param>
  /// <param name="notReady">Allows the insertion of a thread, even if that thread isn't ready yet</param>
  /// <return>The shared pointer which contains the context member.</return>
  /// <remarks>
  /// It's safe to allow the returned shared_ptr to go out of scope; the core context
  /// will continue to hold a reference to it until Remove is invoked.
  /// </remarks>
  void AddCoreRunnable(const std::shared_ptr<CoreRunnable>& pCoreRunnable);

  /// \internal
  /// <summary>
  /// Adds the specified context creation listener to receive creation events broadcast from this context
  /// </summary>
  /// <param name="pBase">The instance being added</param>
  void AddBolt(const std::shared_ptr<BoltBase>& pBase);

  /// \internal
  /// <summary>
  /// Overload of Add based on ContextMember
  /// </summary>
  void AddContextMember(const std::shared_ptr<ContextMember>& ptr);

  /// \internal
  /// <summary>
  /// Forwarding routine, recursively adds a packet subscriber to the internal packet factory
  /// </summary>
  void AddPacketSubscriber(const AutoFilterDescriptor& rhs);

  /// \internal
  /// <summary>
  /// Internal type introduction routine
  /// </summary>
  void AddInternal(const CoreObjectDescriptor& traits);

  /// \internal
  /// <summary>
  /// Internal specific interface introduction routine
  /// </summary>
  void AddInternal(const AnySharedPointer& ptr);

  /// \internal
  /// <summary>
  /// Scans the memo collection for the specified entry, or adds a deferred resolution marker if resolution was not possible
  /// </summary>
  /// <returns>
  /// The memo entry where this type was found
  /// </returns>
  /// <param name="reference">An initialized shared pointer slot which may be used in type detection</param>
  void FindByType(AnySharedPointer& reference, bool localOnly = false) const;

  /// \internal
  /// <summary>
  /// Unsynchronized version of FindByType
  /// </summary>
  MemoEntry& FindByTypeUnsafe(AnySharedPointer& reference, bool localOnly = false) const;

  /// \internal
  /// <summary>
  /// Recursive locking for Autowire satisfaction search
  /// </summary>
  void FindByTypeRecursive(AnySharedPointer& reference, const AutoSearchLambda& searchFn) const;

  /// \internal
  /// <summary>
  /// Adds the specified deferrable autowiring to be satisfied at a later date when its matched type is inserted
  /// </summary>
  void AddDeferredUnsafe(DeferrableAutowiring* deferrable);

  /// \internal
  /// <summary>
  /// Adds a snooper to the snoopers set
  /// </summary>
  void InsertSnooper(const AnySharedPointer& snooper);

  /// \internal
  /// <summary>
  /// Removes a snooper to the snoopers set
  /// </summary>
  void RemoveSnooper(const AnySharedPointer& snooper);
  
  /// \internal
  /// <summary>
  /// Recursively removes the specified snooper
  /// </summary>
  /// <remarks>
  /// This method has no effect if the passed value is presently a snooper in this context; the
  /// snooper collection must therefore be updated prior to the call to this method.
  /// </remarks>
  void UnsnoopEvents(const AnySharedPointer& snooper, const JunctionBoxEntry<CoreObject>& traits);
  
  /// \internal
  /// <summary>
  /// Forwarding routine, only removes from this context
  /// </summary>
  void UnsnoopAutoPacket(const CoreObjectDescriptor& traits);

  /// \internal
  /// <summary>
  /// Invoked by a parent context when the parent has transitioned to the Running state
  /// </summary>
  void TryTransitionChildrenState(void);

  /// <summary>
  /// Registers a factory _function_, a lambda which is capable of constructing decltype(fn())
  /// </summary>
  template<class Fn>
  void RegisterFactoryFn(Fn&& fn) {
    Inject<AutoFactoryFn<typename std::remove_pointer<decltype(fn())>::type, Fn>>(std::forward<Fn>(fn));
  }

  /// \internal
  /// <summary>
  /// Registers a new foreign factory type without explicitly specifying the returned value type
  /// </summary>
  /// <param name="Factory">The factory type to be added</param>
  /// <param name="obj">A reference to the factory proper</param>
  template<class Factory>
  void RegisterFactory(Factory& obj, autowiring::member_new_type<Factory, autowiring::factorytype::ret_val>) {
    RegisterFactoryFn([&obj] { return obj.New(); });
  }

  /// \internal
  template<class Factory>
  void RegisterFactory(const Factory&, autowiring::member_new_type<Factory, autowiring::factorytype::none>) {}

  // Internal resolvers, used to determine which teardown style the user would like to use
  /// \internal
  template<class Fx>
  void AddTeardownListener2(Fx&& fx, void (Fx::*)(void)) {
    onTeardown += [fx](const CoreContext&) { fx(); };
  }

  /// \internal
  template<class Fx>
  void AddTeardownListener2(Fx&& fx, void (Fx::*)(const CoreContext&)) {
    onTeardown += std::move(fx);
  }

  /// \internal
  template<class Fx>
  void AddTeardownListener2(Fx&& fx, void (Fx::*)(void) const) {
    onTeardown += [fx](const CoreContext&) { fx(); };
  }

  /// \internal
  template<class Fx>
  void AddTeardownListener2(Fx&& fx, void (Fx::*)(const CoreContext&) const) {
    onTeardown += std::move(fx);
  }

public:
  // Accessor methods:
  /// True if and only if this is the global context.
  bool IsGlobalContext(void) const { return !m_pParent; }
  /// The number of Autowired members in this context.
  size_t GetMemberCount(void) const { return m_concreteTypes.size(); }
  /// The number of child contexts of this context.
  size_t GetChildCount(void) const;
  /// The type used as a sigil when creating this class, if any.
  const std::type_info& GetSigilType(void) const { return m_sigilType; }
  /// The Context iterator for the parent context's children, pointing to this context.
  t_childList::iterator GetBackReference(void) const { return m_backReference; }
  /// A shared reference to the parent context of this context.
  const std::shared_ptr<CoreContext>& GetParentContext(void) const { return m_pParent; }

  /// <summary>
  /// Returns a copy of the list of runnables
  /// </summary>
  /// <remarks>
  /// This list is intended primarily for diagnostic purposes.  The pointers are dumb pointers, and may be
  /// invalidated if the caller is not careful to hold a shared pointer to the context.
  /// </remarks>
  std::vector<CoreRunnable*> GetRunnables(void) const;

  /// True if the sigil type of this CoreContext matches the specified sigil type.
  template<class Sigil>
  bool Is(void) const { return m_sigilType == typeid(Sigil); }

  /// <summary>
  /// The first child in the set of this context's children.
  /// </summary>
  std::shared_ptr<CoreContext> FirstChild(void) const;

  /// <summary>
  /// The next context sharing the same parent, or null if this is the last entry in the list
  /// </summary>
  std::shared_ptr<CoreContext> NextSibling(void) const;

  /// <summary>
  /// Gets the type information for the instance referenced by the specified shared pointer.
  /// </summary>
  /// <remarks>
  /// The returned type structure will be the actual type of the specified object as defined at the time of
  /// injection.  In the case of a static factory new or AutoFactory new, this type will be the type of the
  /// interface.  All other members are the concrete type actually injected, as opposed to the type unifier
  /// for that type.
  ///
  /// This method will throw an exception if the passed shared pointer is not strictly a member of this context.
  /// </remarks>
  /// <returns>
  /// The type identifier of the referenced instance.
  /// </returns>
  auto_id GetAutoTypeId(const AnySharedPointer& ptr) const;

  /// \internal
  /// <summary>
  /// Creation helper routine
  /// </summary>
  template<class T>
  static std::shared_ptr<CoreContextT<T>> Create(
    std::shared_ptr<CoreContext> pParent,
    t_childList::iterator backReference
  ) {
    return std::make_shared<CoreContextT<T>>(pParent, backReference);
  }

private:
  /// \internal
  /// <summary>
  /// Creation helper routine
  /// </summary>
  template<class T>
  static std::shared_ptr<CoreContext> CreateUntyped(
    std::shared_ptr<CoreContext> pParent,
    t_childList::iterator backReference
  ) {
    return std::static_pointer_cast<CoreContext>(std::make_shared<CoreContextT<T>>(pParent, backReference));
  }

public:
  /// \internal
  /// <summary>
  /// Factory to create a new context
  /// </summary>
  /// <param name="inj">An injectable type.</param>
  template<class T>
  std::shared_ptr<CoreContextT<T>> Create(AutoInjectable&& inj) {
    return std::static_pointer_cast<CoreContextT<T>>(CreateInternal(&CoreContext::CreateUntyped<T>, std::move(inj)));
  }

  /// <summary>
  /// Creates a child context of this context.
  /// </summary>
  /// <remarks>
  /// Using using ::AutoCreateContext or AutoCreateContextT is the prefered way to create
  /// children of the current context.
  ///
  /// \code
  /// AutoCreateContext myContext;
  /// AutoCreateContextT<MySigil> myMarkedContext;
  /// \endcode
  /// </remarks>
  template<class T>
  std::shared_ptr<CoreContextT<T>> Create(void) {
    return std::static_pointer_cast<CoreContextT<T>>(CreateInternal(&CoreContext::CreateUntyped<T>));
  }

  /// <summary>
  /// Allows a specifically named class to be "bolted" to this context or one of its subcontexts.
  /// </summary>
  /// <remarks>
  /// Call Enable<T> before calling BoltTo<T>. If the specified type does not inherit from Bolt,
  /// this method has no effect.
  /// </remarks>
  template<class T>
  void Enable(void) {
    static_assert(!std::is_abstract<T>::value, "Cannot enable an abstract class for bolting");
    EnableInternal((T*)nullptr, (T*)nullptr);
  }

  /// <summary>
  /// Injects the specified type T into all subcontexts created with one of the matching sigil types.
  /// </summary>
  template<class T, class... Sigils>
  void BoltTo(void) {
    EnableInternal((T*)nullptr, (Boltable<Sigils...>*)nullptr);
  }

  /// <summary>
  /// Introduces the specified pointer to this context explicitly
  /// </summary>
  /// <param name="ptr">The interface to make available in this context</param>
  /// <remarks>
  /// Add is similar in behavior to Inject, except that the passed pointer is not treated as a concrete
  /// type.  This means that other interfaces implemented by ptr will not be available for autowiring
  /// unless explicitly made discoverable by another call to Add.
  ///
  /// It is an error to add a type which is already autowirable in a context.
  /// </remarks>
  template<typename T>
  void Add(const std::shared_ptr<T>& ptr) {
    AddInternal(AnySharedPointer(ptr));
  }

  /// <summary>
  /// Sets the context's teardown unlink behavior
  /// </summary>
  /// <remarks>
  /// If this feature is turned on, then during context destruction and after teardown listeners have
  /// been run, all context members will be scanned for uses of Autowired.  If a context member has
  /// such a field, and that field points to another member of the current context, then the field
  /// will be unlinked.
  ///
  /// If a field is AutoRequired, it is skipped.  If the autowiring was satisfied outside of the
  /// context (for instance, at global scope), it's also skipped.  AutowiredFast uses aren't registered
  /// anywhere, so they are also skipped.
  ///
  /// This method may be called at any time where a valid CoreContext reference exists.  CoreContext
  /// does not track additional state if this flag is set.
  /// </reamrks>
  void SetUnlinkOnTeardown(bool unlinkOnTeardown) {
    m_unlinkOnTeardown = unlinkOnTeardown;
  }

  /// <summary>
  /// Injects the specified types into this context.
  /// </summary>
  /// <remarks>
  /// Arguments will be passed to the T constructor, if provided.
  /// </remarks>
  template<typename T, typename... Args>
  std::shared_ptr<T> Inject(Args&&... args) {
    // Creator proxy, knows how to create the type we intend to inject
    typedef autowiring::CreationRules<T, Args...> CreationRules;

    // Add this type to the TypeRegistry, also ensure that we initialize support for blind
    // fast pointer cast to CoreObject.
    (void) RegType<T>::r;
    (void) autowiring::fast_pointer_cast_initializer<CoreObject, T>::sc_init;

    // First see if the base object type has already been injected.  This is also necessary to
    // ensure that a memo slot is created for the type by itself, in cases where the injected
    // member does not inherit CoreObject and this member is eventually satisfied by one that does.
    {
      std::shared_ptr<T> pure;
      FindByType(pure, true); //skip non-local slots.
      if (pure)
        return pure;
    }

    // We must make ourselves current for the remainder of this call:
    CurrentContextPusher pshr(shared_from_this());
    std::shared_ptr<typename CreationRules::TActual> retVal(
      CreationRules::New(*this, std::forward<Args>(args)...)
    );

    // AutoInit if sensible to do so:
    CallAutoInit(*retVal, has_autoinit<T>());

    try {
      // Pass control to the insertion routine, which will handle injection from this point:
      AddInternal(CoreObjectDescriptor(retVal, (T*)nullptr));
    }
    catch(autowiring_error&) {
      // We know why this exception occurred.  It's because, while we were constructing our
      // type, someone else was constructing the same type at the same time.  As a consequence,
      // we will simply eat this exception, and handle it silently by returning the type that
      // someone else has already attempted to construct, as per the documented behavior of
      // Construct.
      FindByType(retVal);
    }

    // Factory registration if sensible to do so, but only after the underlying type has been added
    // This ensures that any creation operations that happen as a consequence of factory registration
    // can correctly back-reference the factory proper via autowiring
    RegisterFactory(*retVal, autowiring::member_new_type<typename CreationRules::TActual>());
    return std::static_pointer_cast<T>(retVal);
  }

  /// <summary>
  /// Injects a type into the current context.
  /// </summary>
  template<typename T>
  static void InjectCurrent(void) {
    CurrentContext()->Inject<T>();
  }

  /// <summary>
  /// True, if an instance of the specified type exists and dependencies of that type can be autowired in this context.
  /// </summary>
  template<class T>
  bool Has(void) const {
    std::shared_ptr<T> ptr;
    FindByType(ptr);
    return ptr != nullptr;
  }

  /// <summary>
  /// Runtime variant of All
  /// </summary>
  /// <remarks>
  /// This instance does not cause a correct instantiation of the underlying junction box.  Users are cautioned against
  /// using this method directly unless they are able to ensure a proper entry is made into the type registry.
  ///
  /// It is an error to call this method on an unregistered type.
  /// </remarks>
  JunctionBoxBase& All(const std::type_info& ti) const;

  /// <summary>
  /// Returns all members of and snoopers on this context which implement the specified interface
  /// </summary>
  /// <remarks>
  /// This method makes use of the JunctionBox subsystem, and thus is extremely efficient.  The underlying system is
  /// memoized, and new entries automatically update any existing memos, which gives this routine O(n) efficiency, where
  /// n is the number of types in this context that implement the specified interface.
  ///
  /// Note that the junction box will also contain members of child contexts that implement the specified interface, and
  /// instances that are snooping this context.
  ///
  /// This method's result will be an empty iterable if the context is not currently initiated.
  /// </remarks>
  template<class T>
  JunctionBox<T>& All(void) {
    // Type registration, needed to ensure our junction box actually exists
    (void) RegEvent<T>::r;

    // Simple coercive transfer:
    return static_cast<JunctionBox<T>&>(All(typeid(T)));
  }

  /// \internal
  /// <summary>
  /// Sends AutowiringEvents to build current state.
  /// </summary>
  void BuildCurrentState(void);

  /// <summary>
  /// A copy of the current list of child CoreRunnables of this context.
  /// </summary>
  /// <remarks>
  /// No guarantee is made about how long the returned collection will be consistent within this
  /// context.  A thread may potentially be added to the context after the method returns.
  /// </remarks>
  std::vector<std::shared_ptr<BasicThread>> CopyBasicThreadList(void) const;

  /// <summary>
  /// True, if the context has been initiated.
  /// </summary>
  /// <remarks>
  /// This method will return true even if the context is currently shut down.  Once the context is
  /// shut down, the return value of this method is guaranteed to not change.
  /// </remarks>
  bool IsInitiated(void) const {
    switch (m_state) {
    case State::Initiated:
    case State::Running:
    case State::Shutdown:
      return true;
    default:
      return false;
    }
  }

  /// <summary>
  /// True, if the context is presently running.
  /// </summary>
  /// <remarks>
  /// This method may return false even if IsInitiated returns true if the parent context has not
  /// yet been initiated.
  /// </remarks>
  bool IsRunning(void) const { return m_state == State::Running; }

  /// <returns>
  /// True if CoreRunnable instances in this context should begin teardown operations
  /// </returns>
  bool IsShutdown(void) const {
    switch (m_state) {
    case State::Shutdown:
    case State::Abandoned:
      return true;
    default:
      return false;
    }
  }

  /// <summary>
  /// Alias of the IsInitiated method
  /// </summary>
  bool WasStarted(void) const { return IsInitiated(); }

  /// <summary>
  /// Reports whether the specified context is an ancestor of this context.
  /// </summary>
  /// <remarks>
  /// This method will also return true if this == child.
  /// </remarks>
  /// <returns>
  /// True if this context is an ancestor of the specified context.
  /// </returns>
  bool IsAncestorOf(const CoreContext* child) const {
    for(auto cur = child; cur; cur = cur->GetParentContext().get())
      if(cur == this)
        return true;
    return false;
  }

  /// \internal
  /// <summary>
  /// Provides a shared pointer to the event sender in this context that matches the specified type.
  /// </summary>
  template<class T>
  std::shared_ptr<JunctionBox<T>> GetJunctionBox(void) {
    // Add this type to the event registry. All events call this function
    (void)RegEvent<T>::r;
    
    return std::static_pointer_cast<JunctionBox<T>, JunctionBoxBase>(
      m_junctionBoxManager->Get(typeid(T))
    );
  }

  /// <summary>
  /// Fires an event in another context.
  /// </summary>
  /// <remarks>
  /// Invoke allows you to fire an event in another context without first making that
  /// context current.
  ///
  /// The following statment:
  ///
  ///    \code
  ///    ctxt->Invoke(&MyEventType::MyEvent)();
  ///    \endcode
  ///
  /// is equivalent to:
  ///
  ///    \code
  ///    CurrentContextPusher(ctxt);
  ///    (AutoFired<MyEventType>())(&MyEventType::MyEvent)();
  ///    ctxt->Pop();
  ///    \endcode
  /// </remarks>
  template<typename MemFn>
  InvokeRelay<MemFn> Invoke(MemFn memFn){
    typedef typename Decompose<MemFn>::type EventType;
    return MakeInvokeRelay(GetJunctionBox<EventType>(), memFn);
  }

  /// <summary>
  /// Starts all registered threads and enables events and the flow of filter graph packets.
  /// </summary>
  void Initiate(void);

  /// <summary>
  /// Begins shutdown of this context, optionally waiting for child contexts and threads to also shut
  /// down before returning.
  /// </summary>
  /// <param name="wait">Specifies whether the function should wait for all child contexts to exit before returning</param>
  /// <param name="shutdownMode">Specifies whether CoreThread objects should run all pending jobs or should shutdown immediately.</param>
  /// <remarks>
  /// After this method is invoked, no new events can be dispatched within this context or any descendant
  /// context, whether those events are fired in this context or one above, and regardless of whether these events
  /// are fired or deferred.  Event receivers in this context will also not receive any messages.
  ///
  /// Filter graph packets will not be sent to receivers.
  ///
  /// The OnStop() method of all threads in the context is invoked. The specified shutdown mode
  /// determines whether the graceful parameter of OnStop() is set to true or false.
  /// </remarks>
  void SignalShutdown(bool wait = false, ShutdownMode shutdownMode = ShutdownMode::Graceful);

  /// <summary>
  /// Shuts down the context with the Immediate shutdown mode.
  /// </summary>
  /// <remarks>
  /// The same as calling SignalShutdown(true, ShutdownMode::Immediate).
  /// </remarks>
  void SignalTerminate(bool wait = true) { SignalShutdown(wait, ShutdownMode::Immediate); }

  /// <summary>
  /// Identical to Wait, except blocks only until the threads in this and all descendant contexts have stopped
  /// </summary>
  /// <remarks>
  /// This method intrinsically may imply a race condition unless the caller is careful to ensure it retains
  /// total control over injection and subcontext creation events.  In a single-threaded system, the currrent
  /// context and all child contexts are guaranteed to have no CoreRunnable instances in a running state.
  /// </remarks>
  void Quiescent(void) const;

  /// <summary>
  /// Timed version of Quiescent
  /// </summary>
  bool Quiescent(std::chrono::nanoseconds duration) const;

  /// <summary>
  /// Retrieves the system's current quiescence status
  /// </summary>
  bool IsQuiescent(void) const;

  /// <summary>
  /// Waits until the context begins shutting down (IsShutdown is true)
  /// and all threads and child threads have terminated.
  /// </summary>
  /// <remarks>
  /// You can use this function to keep a context in scope while it processes events, etc.
  /// For example, to prevent exiting from the program's main() function:
  ///
  /// \code
  /// int main(){
  ///     AutoCurrentContext ctxt;
  ///     // set up and initiate context...
  ///     ctxt->Wait();
  /// }
  /// \endcode
  /// </remarks>
  void Wait(void);

  /// <summary>
  /// Waits for the specified number of nanoseconds.
  /// </summary>
  bool Wait(const std::chrono::nanoseconds duration);

  /// <summary>
  /// Waits until the context is initiated or, if never initiated, until it starts shutting down.
  /// </summary>
  /// <returns>True if initiated, false if shutting down.</returns>
  bool DelayUntilInitiated(void);

  /// <summary>
  /// Static version of SetCurrent, may be invoked with nullptr
  /// </summary>
  static std::shared_ptr<CoreContext> SetCurrent(const std::shared_ptr<CoreContext>& ctxt);

  /// <summary>
  /// Makes this context the current context.
  /// </summary>
  /// <returns>The previously current context, or else nullptr if no context was current.</returns>
  std::shared_ptr<CoreContext> SetCurrent(void);

  /// <summary>
  /// Makes no context current.
  /// </summary>
  /// <remarks>
  /// Generally speaking, if you just want to release a reference to the current context, simply
  /// make the global context current instead.
  ///
  /// This method is identical to CoreContext::SetCurrent(nullptr)
  /// </remarks>
  static void EvictCurrent(void);

  /// <summary>
  /// The shared pointer to the current context.
  /// </summary>
  /// <returns>
  /// A shared pointer to the current CoreContext instance of the current thread, or else nullptr,
  /// if no context is current.
  /// </returns>
  /// <remarks>
  /// This works by using thread-local store, and so is safe in multithreaded systems.  The current
  /// context is assigned before invoking a CoreRunnable instance's Run method, and it's also assigned
  /// when a context is first constructed by a thread.
  /// </remarks>
  static const std::shared_ptr<CoreContext>& CurrentContextOrNull(void);

  /// <summary>
  /// Identical to CurrentContextNoCheck, except returns the global context instead of a null pointer
  /// </summary>
  static std::shared_ptr<CoreContext> CurrentContext(void);

  /// <summary>
  /// Filters std::current_exception using any registered exception filters, or rethrows.
  /// </summary>
  /// <remarks>
  /// The passed exception is assumed to be a generic exception whose default behavior
  /// shall be to tear down the context.  It will be the caller's responsibility to ensure
  /// that this behavior is observed.
  ///
  /// If the exception is successfully handled by a filter, this method returns cleanly.
  /// Otherwise, this method is equivalent to std::rethrow_exception.
  /// </remarks>
  void FilterException(void);

  /// <summary>
  /// Filters a std::current_exception thrown by an EventSenderBase during a Fire
  /// </summary>
  /// <param name="pProxy">The sender of the event</param>
  /// <param name="pRecipient">The recipient of the event</param>
  void FilterFiringException(const JunctionBoxBase* pProxy, CoreObject* pRecipient);

  /// <summary>
  /// Runtime version of AddSnooper
  /// </summary>
  void AddSnooper(const CoreObjectDescriptor& traits);

  /// <summary>
  /// Registers the specified event receiver to receive messages from this context
  /// </summary>
  /// <remarks>
  /// This enables the passed event receiver to snoop events that are broadcast from a
  /// parent context.  The passed event receiver MUST exist in a parent context, or the
  /// behavior of this method may be undefined during teardown.
  ///
  /// The snooper will not receive any events broadcast from parent contexts.  ONLY events
  /// broadcast in THIS context will be forwarded to the snooper.
  /// </remarks>
  template<class T>
  void AddSnooper(const std::shared_ptr<T>& pSnooper) {
    AddSnooper(CoreObjectDescriptor(pSnooper, (T*)nullptr));
  }

  /// <summary>
  /// Resolution overload
  /// </summary>
  template<class T>
  void AddSnooper(const Autowired<T>& snooper) {
    AddSnooper(
      CoreObjectDescriptor(
        static_cast<const std::shared_ptr<T>&>(snooper),
        (T*)nullptr
      )
    );
  }

  /// <summary>
  /// Runtime version of RemoveSnooper
  /// </summary>
  void RemoveSnooper(const CoreObjectDescriptor& traits);

  /// <summary>
  /// Unregisters a snooper previously registered to receive snooped events
  /// </summary>
  /// <remarks>
  /// It is an error to call this method without a prior call to AddSnooper
  /// </remarks>
  template<class T>
  void RemoveSnooper(const std::shared_ptr<T>& pSnooper) {
    RemoveSnooper(CoreObjectDescriptor(pSnooper, (T*)nullptr));
  }
  
  /// <summary>
  /// Resolution overload of RemoveSnooper
  /// </summary>
  template<class T>
  void RemoveSnooper(const Autowired<T>& snooper) {
    RemoveSnooper(
      CoreObjectDescriptor(
        static_cast<const std::shared_ptr<T>&>(snooper),
        (T*)nullptr
      )
    );
  }

  /// <summary>
  /// Remove EventReceiver from parents unless its a member of the parent
  /// </summary>
  /// <summary>
  /// Locates an available context member in this context
  /// </summary>
  template<class T>
  void FindByType(std::shared_ptr<T>& slot, bool localOnly = false) const {
    AnySharedPointerT<T> reference;
    FindByType(reference, localOnly);
    slot = reference.template as<T>();
  }

  /// <summary>
  /// Identical to Autowire, but will not register the passed slot for deferred resolution
  /// </summary>
  template<class T>
  void FindByTypeRecursive(std::shared_ptr<T>& ptr) const {
    AnySharedPointerT<T> slot;
    FindByTypeRecursive(slot, AutoSearchLambdaDefault());
    ptr = slot.get();
  }

  /// <summary>
  /// Identical to Autowire, but will not register the passed slot for deferred resolution
  /// </summary>
  template<class T>
  void FindByTypeRecursive(AnySharedPointerT<T>& slot) const {
    FindByTypeRecursive(slot, AutoSearchLambdaDefault());
  }

  template<class T>
  class AutoSearchLambdaAutowire:
    public AutoSearchLambda
  {
  public:
    AutoSearchLambdaAutowire(CoreContext& ctxt, AnySharedPointerT<T>& ref, DeferrableAutowiring& defer) :
      ctxt(ctxt),
      ref(ref),
      defer(defer)
    {}

    CoreContext& ctxt;
    AnySharedPointerT<T>& ref;
    DeferrableAutowiring& defer;

    void operator()() const override {
      // If the slot wasn't autowired, complete the satisfaction here
      if(!ref)
        ctxt.AddDeferredUnsafe(&defer);
    }
  };

  /// <summary>
  /// Registers a slot to be autowired
  /// </summary>
  /// <param name="ref">The space where the resolved shared pointer will be stored</param>
  /// <param name="defer">
  /// In the event that resolution was not successful, the deferrable that will be used to perform delayed satisfaction
  /// </param>
  template<class T>
  void Autowire(AnySharedPointerT<T>& ref, DeferrableAutowiring& defer) {
    FindByTypeRecursive(
      ref,
      AutoSearchLambdaAutowire<T>(*this, ref, defer)
    );
  }

  template<class T, class Fn>
  class AutoSearchLambdaNotifyWhenAutowired:
    public AutoSearchLambda,
    public AnySharedPointerT<T>
  {
  public:
    AutoSearchLambdaNotifyWhenAutowired(CoreContext& ctxt, Fn& fn) :
      ctxt(ctxt),
      fn(fn),
      resultSlot(nullptr)
    {}

    CoreContext& ctxt;
    Fn& fn;
    mutable AutowirableSlotFn<T, Fn>* resultSlot;

    void operator()() const override {
      if(*this)
        // Matched successfully, do not attempt to defer
        return;

      resultSlot = new AutowirableSlotFn<T, Fn>(ctxt.shared_from_this(), std::forward<Fn>(fn));
      ctxt.AddDeferredUnsafe(resultSlot);
    }
  };

  /// <summary>
  /// Assigns the thread pool handler for this context
  /// </summary>
  /// <remarks>
  /// If the context is currently running, the thread pool will automatically be started.  The pool's
  /// start token and shared pointer is reset automatically when the context is torn down.  If the
  /// context has already been shut down (IE, IsShutdown returns true), this method has no effect.
  ///
  /// Dispatchers that have been attached to the current thread pool will not be transitioned to the
  /// new pool.  Changing the thread pool may cause the previously assigned thread pool to be stopped.
  /// This will cause it to complete all work assigned to it and release resources associated with
  /// processing.  If there are no other handles to the pool, it may potentially destroy itself.
  ///
  /// It is an error to pass nullptr to this method.
  /// </remarks>
  void SetThreadPool(const std::shared_ptr<autowiring::ThreadPool>& threadPool);

  /// <summary>
  /// Returns the current thread pool
  /// </summary>
  /// <remarks>
  /// If the context has been shut down, (IE, IsShutdown returns true), this method returns nullptr.  Calling
  /// ThreadPool::Start on the returned shared pointer will not cause dispatchers pended to this context to
  /// be executed.  To do this, invoke CoreContext::Initiate
  /// </remarks>
  std::shared_ptr<autowiring::ThreadPool> GetThreadPool(void) const;

  /// <summary>
  /// Submits the specified lambda to this context's ThreadPool for processing
  /// </summary>
  /// <returns>True if the job has been submitted for execution</returns>
  /// <remarks>
  /// The passed thunk will not be executed if the current context has already stopped.
  /// </remarks>
  template<class Fx>
  bool operator+=(Fx&& fx) {
    auto pool = GetThreadPool();
    return
      pool ?
      pool->Submit(std::make_unique<DispatchThunk<Fx>>(std::forward<Fx&&>(fx))) :
      false;
  }

  /// <summary>
  /// Adds a post-attachment listener in this context for a particular autowired member.
  /// There is no guarantee for the context in which the listener will be called.
  /// </summary>
  /// <returns>
  /// A pointer to a deferrable autowiring function which the caller may safely ignore if it's not needed.
  /// Returns nullptr if the call was made immediately.
  /// </returns>
  /// <remarks>
  /// This method will succeed if slot was constructed in this context or any parent context.  If the
  /// passed slot was not created in this context or a parent context, an exception will be thrown.
  ///
  /// It's possible that the passed slot will never be filled, and instead the corresponding instance
  /// destroyed without ever having been initialized.
  ///
  /// If the passed slot is already autowired, then the listener will be invoked immediately from the
  /// body of this method.  Care should be taken to avoid deadlocks in this case--either the caller must
  /// not be holding any locks when this method is invoked, or the caller should design the listener
  /// method such that it may be substitutde in place for the notification routine.
  ///
  /// The returned value may be used later in CancelAutowiringNotification in order to explicitly clean
  /// up memory.
  /// </remarks>
  template<class T, class Fn>
  const AutowirableSlotFn<T, Fn>* NotifyWhenAutowired(Fn&& listener) {
    AutoSearchLambdaNotifyWhenAutowired<T, Fn> searchFn(*this, listener);
    FindByTypeRecursive(searchFn, searchFn);

    if(searchFn)
      // Success!  We can satisfy the listener call right away, and be assured that the listener
      // will be able to find what it's looking for in this context without holding down a lock,
      // because entities can't be removed from a context.
      listener();
    return searchFn.resultSlot;
  }

  /// <summary>
  /// Adds a teardown notifier which receives a pointer to this context on destruction
  /// </summary>
  template<class Fx>
  void DEPRECATED(AddTeardownListener(Fx&& fx), "Superceded by onTeardown");

  /// <summary>
  /// Unregisters a slot as a recipient of potential autowiring
  /// </summary>
  void CancelAutowiringNotification(DeferrableAutowiring* pDeferrable);

  /// <summary>
  /// Utility debug method for writing a snapshot of this context to the specified output stream
  /// </summary>
  void Dump(std::ostream& os) const;
};


template<class Fx>
void CoreContext::AddTeardownListener(Fx&& fx) {
  AddTeardownListener2<Fx>(std::forward<Fx&&>(fx), &Fx::operator());
}

namespace autowiring {
  /// <summary>
  /// Forward-declarable version of CoreContext::InjectCurrent
  /// </summary>
  template<typename T>
  void InjectCurrent(void){
    CoreContext::InjectCurrent<T>();
  }
}

/// <summary>
/// A type of CoreContext that has a sigil.
/// </summary>
/// <remarks>
/// 
/// </remarks>
template<class T>
class CoreContextT:
  public CoreContext
{
public:
  CoreContextT(const std::shared_ptr<CoreContext>& pParent, t_childList::iterator backReference) :
    CoreContext(pParent, backReference, typeid(T))
  {}
};

std::ostream& operator<<(std::ostream& os, const CoreContext& context);

#include "MicroBolt.h"

template<typename T, typename... Sigil>
void CoreContext::AutoRequireMicroBolt(void) {
  Inject<MicroBolt<T, Sigil...>>();
}

template<class T>
class CoreContext::AutoFactory
{
public:
  virtual T* operator()(CoreContext& ctxt) const = 0;
};

template<class T, class Fn>
class CoreContext::AutoFactoryFn :
  public CoreObject,
  public CoreContext::AutoFactory<T>
{
public:
  AutoFactoryFn(Fn&& fn) :
    fn(std::move(fn))
  {}

  const Fn fn;

  T* operator()(CoreContext&) const override { return fn(); }
};

template<class... Ts, class Fn>
class CoreContext::AutoFactoryFn<std::tuple<Ts...>, Fn> :
  public CoreObject,
  CoreContext::AutoFactory<Ts>...
{
public:
  AutoFactoryFn(Fn&& fn) :
    fn(std::move(fn))
  {}

  const Fn fn;
};
