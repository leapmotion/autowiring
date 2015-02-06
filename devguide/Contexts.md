Contexts
=========================

Contexts are the base container and scope mechanism of autowiring. The context determines how autowired dependencies are resolved, how events are broadcast, the scope of filter graph networks, and the lifetime of threads.

Every application has a global context, which is created automatically. You can also create additional contexts as needed. Contexts are created in a tree structure with the global context at its root. 

* CoreContext
* GlobalCoreContext
* AutoGlobalContext
* ContextCreator
* CurrentContextEnumerator
* CurrentContextPusher

Within a context:

* Autowired
* AutoConstruct
* AutoRequired

Injecting code into another context:

* BoltBase
* Bolt
* AutoEnable

# Getting a Context

If you do nothing else, every autowiring operation takes place within the global context. There is always a single *current* context. When you start the application, the global context is current. You can change the current context as needed.

To get a reference to the global context, use:

    AutoGlobalContext global;
    
Avoid holding this reference longer than necessary.

Get a reference to the current context with:

    AutoCurrentContext current;

You can use the ContextEnumerator and ContextEnumeratorT classes to enumerate existing contexts.

# Creating Contexts

The simplest way to create a new context is with:

    AutoCreateContext newContext;
    
This creates a new context as a child of the current context. You can also create a context as a child of another context with the CoreContext::Create() and CoreContext::CreatePeer() methods.

## Sigils

You can categorize your contexts by assigning a sigil class at creation. A sigil class serves as a marker, which you can use to identify contexts by type. The sigil class can be any type, even an empty class declaration. Use the AutoCreateContextT template to create a class with a sigil:

    class Foo{};
    AutoCreateContextT<Foo> contextWithSigil;
    
You can check the sigil of a context with CoreContext::GetSigilType(). (Use the autowiring::demangle() function to get a human-readable string from the std::type_info object.) You can also enumerate contexts that share a given sigil using ContextEnumeratorT. The sigil class used for the global context is GlobalCoreContext.

# Changing Contexts

All basic autowiring operations take place in the current context. You can change contexts in a number of ways:

* CoreContext::SetCurrent() makes a given context the current context:

      AutoCreateContext foo;
      foo->SetCurrent();
    
* The CurrentContextPusher class provides stack-like behavior that stores a reference to the current context when changing contexts and then restores it when the CurrentContextPusher object goes out of scope. 

      { //limits scope
          CurrentContextPusher push(ctxt); //Saves the current context before swithing to ctxt
          // ... perform some actions
      } // push goes out of scope, restoring previous context
    
* CoreContext::EvictCurrent(), a static function, effectively makes the global context current.


# Enumerating Contexts

The easiest way to enumerate contexts is with the ContextEnumerator classes. ContextEnumerator enumerates all children of a specified context. CurrentContextEnumerator is a special case that enumerates the current context. When enumerating, the parent context is included. ContextEnumeratorT enumerates only those children created with a specific sigil class. The parent context is included in the enumeration is it has the matching sigil.

To enumerate all contexts:

\include snippets/CoreContext_EnumeratePusher.txt

If you use the CurrentContextPusher class to change to the enumerated context, the current context is restored automatically when the for-loop terminates.

Similarly, to enumerate all contexts with the sigil class, Foo:

\include snippets/CoreContext_EnumerateTPusher.txt

Alternately, you can enumerate contexts manually with the CoreContext functions like CoreContext::firstChild() and CoreContext::nextSibling();

# Autowiring Members to a Context

The Autowiring library provides a few classes for adding objects to a context and connecting resources to the requested resources.

When you autowire a type, the autowiring system looks for an existing instance of that type and assigns a shared pointer to the existing instance, if one exists. Dependencies wired to a child context can be satisfied by resources in parent (or other direct ancestor) context, but not by one in a peer or sub-child context.

The primary ways to autowire a dependency to a context include:

* Autowire -- creates a "slot" for a type in a context, but does not create an instance of that type. If the resource already exists, the slot is assigned a shared pointer to the existing instance. Otherwise, the slot is filled when a suitable instance is created in the context or a parent context. You can use Autowire::NotifyWhenAutowired() to detect when the dependency is resolved.

* AutoRequired -- Works like Autowired, but if no suitable instance of the specified type exists, AutoRequired attempts to create one by calling the default, zero-argument constructor. 

* AutoConstruct -- Works like AutoRequired, but forwards constructor arguments to the type constructor.

* AutowiredFast -- Works like Autowired (only faster) if the resource already exists. However, if no suitable resource is found the dependency is not satisfied later. Use where performance optimaization is warrented and you know that the dependencies already exist.

Only one instance of a type can be autowired to a context. All slots created for the same type will be filled with a shared pointer to the same instance. 
 
The type specified in an autowiring declaration does not need to be a concrete type. If you autowire (but nor auto-require) a pure abstract type, for example, the dependency can be resolved by any concrete implementation of that type. However, if more than one concrete type already exists in a context, autowiring a common super type leads to an exception. Attempting to use AutoRequired for a type that cannot be constructed using the default constructor also leads to an exception.


