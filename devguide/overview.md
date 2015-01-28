Autowiring IoC Framework    {#mainpage}
=========================

Autowiring is an [inversion-of-control](http://en.wikipedia.org/wiki/Inversion_of_control) framework for C++11. It provides a declarative way to manage resources through [dependency injection](http://en.wikipedia.org/wiki/Dependency_injection). Instead of explicitly instantiating dependencies, simply declare what you need and Autowiring will manage object creation and wiring.

Autowiring provides the following services:

* Injects dependencies
* Manages object lifespan
* Manages thread lifespan
* Manages event broadcast and reception
* Manages [filter graph](http://en.wikipedia.org/wiki/Filter_graph) packet creation and delivery

# Organization

A Context is a basic unit of organization in an autowired application. A Context defines the scope in which autowiring resolves dependencies, broadcasts events and filter graph packets, and maintains object and thread lifespans. Contexts form a tree structure. The root of the tree is the global context, which is created automatically. Contexts can "contain" other contexts as child nodes in the context tree. Dependencies within a child context can be satisfied by an autowired member of a parent context (if it can't be satisfied within the child itself). 

\htmlonly
<iframe src="ContextTree.svg" frameBorder="0"></iframe>
\endhtmlonly

Most applications will only have a few contexts. Child contexts are most useful when managing temporary resources. For example, if you have multiple views to the same data, the data model could be a type within a parent context while each view exists within a child context. These child contexts could be created and destroyed on demand.

# Autowiring

Inside a context, objects can be autowired directly or as members of another autowired object. Only one instance of an autowired type can exist in a context. Once a autowired type is added to a context, it exists until the context is destroyed.

\include snippets/Context_Overview.txt

This example creates a context and autowires two members into it. Type Foo contains its own AutoRequired member, so an instance of Bar is also created. When a member of type Bar is added to the context directly, it receives a shared pointer to the existing instance of Bar. Thus bar and foo.bar reference the same instance.

# Filter graph networks

Autofilters support the creation of filter graph networks within a context. Autowired objects within a context can implement the Autofilter function, which will be automatically invoked when a suitable packet is dispatched by the AutoPacketFactory or another filter.

\htmlonly
<iframe src="FilterGraph.svg" frameBorder="0"></iframe>
\endhtmlonly

 The publishing object does not need any knowledge of the receiving objects. Similarly, the AutoFired event system allows an object to define a callback function that gets called whenever an event is triggered.

 


