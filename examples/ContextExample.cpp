// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include <autowiring/Autowired.h>
#include <iostream>
#include <cassert>
#include <memory>

// pretty print boolean with desciption
#define check(desc,val) std::cout << desc << (val ? ": True" : ": False") << std::endl

int main() {
  ///////////////////////////
  //// GlobalCoreContext ////
  ///////////////////////////
  { 
    // The CoreContext is the basic unit of organization. They are organized
    // in a tree structure, the root of which is the GlobalCoreContext. Each
    // thread keeps track of its current context, which defaults to
    // GlobalCoreContext
  
    // This creates a shared_ptr to the global context
    AutoGlobalContext global;
  
    // This is the a shared_ptr to the current context
    AutoCurrentContext ctxt;
  
    // Since we default to the current context, they should be the same
    check("Current context is global", global==ctxt);
  }

  /////////////////////////////////////////////////
  //// Context Creation and Switching Contexts ////
  /////////////////////////////////////////////////
  {
    // New contexts can be created using the 'Create' factory method on a context.
    // The factory will create a child context to 'this' contxt
    
    AutoGlobalContext global; //same as AutoCurrentContext

    // Create's a chile of the current context
    AutoCreateContext childCtxt; // Same as childCtxt = global->Create<void>();
    
    // Even though we've created a child context, we're still in the global context
    check("Are we in the GlobalCoreContext", AutoCurrentContext()->IsGlobalContext());
    
    // CurrentContextPusher can be used to switch contexts using RAII patterns
    {
      CurrentContextPusher pshr(childCtxt);
      check("Now in child context", AutoCurrentContext() == childCtxt);
    }
    // Back in global context
  }
  
  

}