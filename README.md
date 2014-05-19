# Purpose
The Autowire library provides a declarative way to manage dependencies through [dependency injection](http://en.wikipedia.org/wiki/Dependency_injection). Instead of explicitly instantiating dependencies, simply declare what you need and Autowire will manage the creation and wiring. This used primarily to used to manage resources and functionality throughout the platform. However, only works with polymorphic types.

# Basic Types
### CoreContext
The basic organizational unit of Autowiring. CoreContexts are aranged in a tree structures, the root of which is a specialized CoreContext of type ```GlobalCoreContext```.

### ContextMember
Every context member must inherit from ```ContextMembers```. Context members are stored in a context, and can be accessed with Autowired<T> and AutoRequired<T>. Only one member of each type can added to a context 

### EventReceiver
Context members that inherit from ```EventReceiver``` can receive events broadcast within a context. Events progate to child contexts, so an EventReceiver in a child context can receive events broadcast from the parent context. Events call a member function on each EventReceiver that implements the specified function signature.

### CoreThread
A specialization of ```ContextMember``` that encapulates a thead. ```Run()``` is run in it's own thead when the context is initiated. You can override this member function to customize what's run in the thread.

# Basic Components
### ```Autowired<T>```
This returns a ```std::shared_ptr``` to singleton of type T if it has already been created. Otherwise, this returns and and empty pointer. This can be used to check if the singleton has already been created. The autowired class offers a quick way to import or create an instance of a specific class in the local context. This class may be safely used even when the member in question is an abstract type. Autowired types must inherit from ```ContextMember```.

```cpp
// MyInstance is empty pointer if and instance of "MyType" doesn't exist in the context.
Autowired<MyType> MyInstance;
```

### ```AutoRequired<T>```
Creates a new instance if this instance isn't autowired. If type T has a static member function called New, the helper's Create routine will attempt call this function instead of the default constructor, even if the default constructor has been supplied, and even if the arity of the New routine is not zero. To prevent this behavior, use a name other than New

```cpp
// MyInstance will always be a std::shared_ptr to and instance of "MyType"
AutoRequired<MyType> MyInstance;
```

### ```AutoCurrentContext```
Provides a simple way to obtain a reference to the current context. Users of this class are encouraged not to hold references for longer than needed.  Failing to release a context pointer could prevent resources from being correctly released.
```cpp
// "ctxt" is a shared_ptr to the current context.
AutoCurrentContext ctxt;
```

### ```AutoCreateContext```
Provides a simple way to create a dependent context pointer. The newly created context will be created using ```CoreContext::CurrentContext()->Create()```.
```cpp
AutoCreateContext childCtxt; // "ctxt" is an anonymous child context of the current context.
AutoCreateContextT<Foo> FooCtxt; // New child context of sigil type "Foo"

//This behaves the same as above
AutoCurrentContext ctxt;
std::shared_ptr<CoreContext> childCtxt = ctxt->Create(); // new child context
std::shared_ptr<CoreContext> fooCtxt = ctxt->Create<Foo>(); //new chld context with sigil "Foo"
```

### ```CurrentContextPusher```
Saves the current context and makes the passed one current, and then restores the current context when destroyed.
```cpp
AutoCurrentContext outerCtxt; // Current context
{
  AutoCreateContext innerCtxt; // New child context
  CurrentContextPusher pshr(innerCtxt); // "innerCtxt" is now the current context
}
// "outerCtxt" is now the current context
```

### ```AutoFired<T>```
Instantiates an event firer that can trigger events of sigil type ```T```. ```T``` must inherit from EventReceiver.
```cpp
AutoFired<UpBroadcastListener> firer;
firer(&UpBroadcastListener::SimpleCall)(); // "SimpleCall()" is called on any context members that inherit from "UpBroadcastListener"

// This behaves the same as the previous line
AutoCurrentContext()->Invoke(&UpBroadcastListener::SimpleCall)();
```

### ```AutoEnable<T>```
A bolt is a way to augment the creation of a new context in a declarative, by-name manner. By inheriting from Bolt, the object will recieve ```ContextCreated()``` calls. To create a class that will have a new instance inserted into each instance of a context with a given sigil, use Boltable.
```cpp
class MyClass:
  public Bolt<MySigil>
{
public:
  void ContextCreated(void) override {
    std::cout << "Bolted!" << std::endl;
  }
};
 
AutoEnable<MyClass>();
 
AutoCurrentContext ctxt;
ctxt->Create<MySigil>(); // "Bolted!" is printed
```

### ```CreatePeer()```
A peer context allows clients to create autowiring contexts which are in the same event domain with respect to each other, but are not in the same autowiring domain.  This can be useful where multiple instances of a particular object are desired, but inserting such objects into a simple child context is cumbersome because the objects at parent scope are listening to events originating from objects at child scope.

# AutoFilter
AutoFilter provides a declarative way to to create a filter graph network within a context. Packets on this network have the same domain as events (snooping works with packets as well). An AutoFilter is made by creating a ContextMember that implements the ```AutoFilter(...)``` member function.

