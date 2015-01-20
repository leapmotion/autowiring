Autowiring IoC Framework    {#mainpage}
=========================

Autowiring is an [inversion-of-control](http://en.wikipedia.org/wiki/Inversion_of_control) framework for C++11. It provides a declarative way to manage resources through [dependency injection](http://en.wikipedia.org/wiki/Dependency_injection). Instead of explicitly instantiating dependencies, simply declare what you need and Autowiring will manage object creation and wiring.

Autowiring provides the following services:

* Dependency injection
* Manages object lifespan
* Manages thread lifespan
* Manages event broadcast and reception
* Manages [filter graph](http://en.wikipedia.org/wiki/Filter_graph) packet creation and delivery

# Overview

A Context is a basic unit of organization in an autowired application. A Context defines the scope in which autowiring resolves dependencies, broadcasts events and packets, and maintains object lifespans. Contexts form a tree structure. The root of the tree is the global context, which is created automatically. Contexts can "contain" other contexts as child nodes in the context tree. Dependencies within a child context can be satisfied by an autowired member of a parent context (if it can't be satisfied within the child itself). 

Most applications will only have a few contexts. Child contexts are most useful when managing temporary resources. For example, if you have multiple views to the same data, the data model could be a type in within the global context and each view could exist within a child context. These child contexts could be created and destroyed on demand.

Inside a context, objects can be autowired directly or as members of another autowired object. Only one instance of an autowired type can exist in a context. (If you need more than one instance of a type, you can create a collection type to hold them and autowire that to the context instead.) Once a autowired type is added to a context, it exists until the context is destroyed.


Autofilters support the creation of filter graph networks within a context. Autowired objects within a context can implement the Autofilter function, which will be automatically invoked when a packet of the matching type is dispatched.

 allow an object to dispatch filter graph packets of data to other objects that are interested in them. The publishing object does not need any knowledge of the receiving objects. Similarly, the AutoFired event system allows an object to define a callback function that gets called whenever an event is triggered.

 


