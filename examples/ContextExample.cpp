// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#include <autowiring/autowiring.h>
#include <iostream>
#include <memory>

// pretty print boolean with desciption
#define check(desc,val) std::cout << desc << (val ? ": True" : ": False") << std::endl

// dummy classes
class Foo {
public:
  int x;
};

class Bar{
public:
  Autowired<Foo> foo;
};

int main() {
  ///////////////////////////
  //                       //
  //   GlobalCoreContext   //
  //                       //
  ///////////////////////////
  { 
    // The CoreContext is the basic unit of organization in Autowriring. They are organized
    // in a tree structure, the root of which is the GlobalCoreContext. Each thread
    // keeps track of its current context, which defaults to GlobalCoreContext
  
    // This creates a shared_ptr to the global context
    AutoGlobalContext global;
  
    // This is the a shared_ptr to the current context
    AutoCurrentContext ctxt;
  
    // Since we default to the current context, they should be the same
    check("Current context is global", global==ctxt);
  }

  /////////////////////////////////////////////////
  //                                             //
  //   Context Creation and Switching Contexts   //
  //                                             //
  /////////////////////////////////////////////////
  {
    // New contexts can be created using the 'Create' factory method on a context.
    // The factory will create a child context to 'this' context.
    // A helper type 'AutoCreateContext' will call Create on the current context
    // automatically
    
    AutoGlobalContext global; //same as AutoCurrentContext here

    // Create's a chile of the current context
    AutoCreateContext childCtxt; // Same as childCtxt = AutoCurrentContext()->Create<void>();
    
    // Even though we've created a child context, we're still in the global context
    check("Are we in the GlobalCoreContext", AutoCurrentContext()->IsGlobalContext());
    
    // CurrentContextPusher can be used to switch contexts using RAII patterns
    {
      CurrentContextPusher pshr(childCtxt);
      check("Now in child context", AutoCurrentContext() == childCtxt);
    }
    // Back in global context
  }
  
  ///////////////////////////////////////////////////////
  //                                                   //
  //   Adding members to a context with AutoRequired   //
  //                                                   //
  ///////////////////////////////////////////////////////
  {
    // Switch to a child context
    AutoCreateContext ctxt;
    CurrentContextPusher pshr(ctxt);
    
    // A single instance of any type can be injected into a context
    // This is done using AutoRequied
    AutoRequired<Foo> foo; // We now have a std::shared_ptr<Foo>
    
    // AutoRequired will return an instance of the requested type from
    // the current context. If that type doesn't exist in the current
    // context, it will create an instace
    foo->x = 5;
    
    // If we try to AutoRequire a second Foo, we'll get a pointer to
    // the same instance
    AutoRequired<Foo> foo2;
    check("foo2 is the same instance as foo", foo==foo2);
    std::cout << "foo2 value of 'x': " << foo2->x << std::endl;
    
  } // 'ctxt' and all members a destroyed when the context goes out of scope
  
  //////////////////////
  //                  //
  //     Autowired    //
  //                  //
  //////////////////////
  {
    {
      AutoCreateContext ctxt;
      CurrentContextPusher pshr(ctxt);
    
      // Autowired is similar to AutoRequied, except it doesn't create the
      // object if it doesn't already exist. It also searches up the tree
      // if the object isn't found in the current context
      Autowired<Foo> foo;
      check("foo is an empty pointer", !foo);
    
      // If we inject to type Foo later, foo will automatically point to that instance
      AutoRequired<Foo>();
      check("foo now contains an instance of Foo", foo);
    }
    
    // This delayed satisfaction also works for class members.
    {
      AutoCreateContext ctxt;
      CurrentContextPusher pshr(ctxt);
      
      // Bar has an Autowrired<Foo> classmember
      AutoRequired<Bar> bar;
      
      check("Foo member of bar is satisfied before AutoRequired", bar->foo);
      
      // If we inject Foo into the context, the Autowired<Foo> member of Bar will be satisfied
      AutoRequired<Foo>();
      check("Foo member of bar is satisfied after AutoRequired", bar->foo);
    }
  }
}
