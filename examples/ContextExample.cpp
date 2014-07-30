// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include <autowiring/Autowired.h>
#include <iostream>
#include <cassert>

int main() {
  // The CoreContext is the basic unit of organization. They are organized
  // in a tree structure, the root of which is the GlobalCoreContext. Each
  // thread keeps track of its current context, which defaults to
  // GlobalCoreContext

  // This creates a shared_ptr to the global context
  AutoGlobalContext global;

  // This is the a shared_ptr to the current context
  AutoCurrentContext ctxt;

  assert(global == ctxt)

}