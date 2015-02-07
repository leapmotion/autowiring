//
//  TestTest.cpp
//  DocTests
//
//  Created by Joe Ward on 9/9/14.
//  Copyright (c) 2014 Leap Motion, Inc. All rights reserved.
//

#include <fstream>
#include "gtest/gtest.h"
#include <autowiring/Autowired.h>
#include <autowiring/ContextCreator.h>
#include <autowiring/AutoNetServer.h>

namespace {
    class AutowireTest : public ::testing::Test {
     protected:
      // You can remove any or all of the following functions if its body
      // is empty.


      AutowireTest() {
        try {


        } catch (int e) {
            std::cout << "Broke " << e << std::endl;
        }

      }

      virtual ~AutowireTest() {
        // You can do clean-up work that doesn't throw exceptions here.
      }

      // If the constructor and destructor are not enough for setting up
      // and cleaning up each test, you can define the following methods:

      virtual void SetUp() {
        // Code here will be called immediately after the constructor (right
        // before each test).
      }

      virtual void TearDown() {
        // Code here will be called immediately after each test (right
        // before the destructor).
      }

      // Objects declared here can be used by all tests in the test case for Foo.


    };

TEST_F(AutowireTest, Dummy) {
      EXPECT_EQ(1, 1.0);
}

TEST_F(AutowireTest, Autowired_Autowired) {
    class Foo{};
    AutoCreateContext ctxt;
    CurrentContextPusher pusher(ctxt);
    // !!!Autowired_Autowired
    Autowired<Foo> slotFoo;
    // !!!END
}

TEST_F(AutowireTest, Autowired_AutoRequired) {
    class Foo{};
    AutoCreateContext ctxt;
    CurrentContextPusher pusher(ctxt);
    // !!!Autowired_AutoRequired
    AutoRequired<Foo> foo;
    // !!!END
}

TEST_F(AutowireTest, Autowired_AutowiredFast) {
    class Foo{};
    AutoCreateContext ctxt;
    CurrentContextPusher pusher(ctxt);
    // !!!Autowired_AutowiredFast
    AutowiredFast<Foo> slotFoo;
    // !!!END
}

TEST_F(AutowireTest, Autowired_Autowired_Notify) {
    class Foo{};
    class Bar{};
    AutoCreateContext ctxt;
    CurrentContextPusher pusher(ctxt);
    // !!!Autowired_Notify
    Autowired<Foo> slotFoo;
    slotFoo.NotifyWhenAutowired([](){std::cout << "slotFoo wired." << std::endl;});
    AutoRequired<Foo> foo; // NotifyWhenAutowired on slotFoo invoked.
    // !!!END
}

TEST_F(AutowireTest, Autowired_AutoDesired) {
    class T{};
    AutoCreateContext ctxt;
    CurrentContextPusher pusher(ctxt);
    class Foo{};
    // !!!Autowired_AutoDesired
    AutoDesired<Foo> foo;
    // !!!END
}

TEST_F(AutowireTest, Autowired_AutoDesired_Alternate) {
    class T{};
    AutoCreateContext ctxt;
    CurrentContextPusher pusher(ctxt);
    // !!!Autowired_AutoDesired_Alternate
    try {
        AutoCurrentContext()->Inject<T>();
    } catch(...) {
        AutoCurrentContext()->FilterException();
    }
    Autowired<T> foo;
    // !!!END
}

} //End of test class
