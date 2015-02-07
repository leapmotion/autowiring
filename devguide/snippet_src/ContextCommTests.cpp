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
    class ContextCommTests : public ::testing::Test {
     protected:
      // You can remove any or all of the following functions if its body
      // is empty.


      ContextCommTests() {
        try {


        } catch (int e) {
            std::cout << "Broke " << e << std::endl;
        }

      }

      virtual ~ContextCommTests() {
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

TEST_F(ContextCommTests, Dummy) {
      EXPECT_EQ(1, 1.0);
}

TEST_F(ContextCommTests, Bolt) {
    class Bar{};
    class Foo: public Bolt<Bar> {
        public:
            void ContextCreated(void) override {
                std::cout << "Bar context created"
                          << autowiring::demangle(AutoCurrentContext()->GetSigilType())
                          << std::endl;
            }
    };

    AutoRequired<Foo> foo;
    AutoCreateContextT<Bar> bar;
}

TEST_F(ContextCommTests, Bolt_Sigils) {
    class Bar{};
    class Muu{};
    class Foo: public Bolt<Bar, Muu> {
        public:
            void ContextCreated(void) override {
                std::cout << "Bolted context created: "
                          << autowiring::demangle(AutoCurrentContext()->GetSigilType())
                          << std::endl;
            }
    };

    AutoRequired<Foo> foo;
    AutoCreateContextT<Bar> bar;
    AutoCreateContextT<Muu> muu;
}

TEST_F(ContextCommTests, Boltable) {
    class Bar{};
    class Muu{};
    class Hex: public Boltable<Muu>{
        public:
            Hex(){
                std::cout << "Created Hex instance in: "
                          << autowiring::demangle(AutoCurrentContext()->GetSigilType())
                          << std::endl;
            }
    };

    AutoEnable<Hex>();
    AutoCreateContextT<Bar> bar;
    AutoCreateContextT<Muu> muu;
}

TEST_F(ContextCommTests, BoltTo) {
    class Bar{};
    class Muu{};
    class Hex{
        public:
            Hex(){
                std::cout << "Created Hex instance in: "
                          << autowiring::demangle(AutoCurrentContext()->GetSigilType())
                          << std::endl;
            }
    };
    class Foo: public Bolt<Bar, Muu> {
        public:
            void ContextCreated(void) override {
                std::cout << "Bolted context created: "
                          << autowiring::demangle(AutoCurrentContext()->GetSigilType())
                          << std::endl;
            }
    };

    AutoCurrentContext()->BoltTo<Hex, Muu, Bar>();
    AutoRequired<Foo> foo;
    AutoCreateContextT<Bar> bar;
    AutoCreateContextT<Muu> muu;
}

TEST_F(ContextCommTests, BasicThread_Prime) {

    class PrimeEvent {
        public:
            virtual void onPrime(unsigned long int prime) = 0;
    };

    class PrimeGenerator : public BasicThread {
        public:
            AutoFired<PrimeEvent> primeDispatcher;
            void Run() override {
                unsigned long int test = 2;
                while (!ShouldStop())
                {
                    for (unsigned long int i = test/2; i > 0 ; --i) {
                        if (i == 1) {
                            primeDispatcher(&PrimeEvent::onPrime)(test);
                        }
                        if (test % i == 0) {break;}
                    }
                    test++;
                }
            }
    };

    class PrimeListener : public PrimeEvent, public Object{
        public:
            void onPrime(unsigned long int prime) override {
                std::cout << "Prime: " << prime << std::endl;
            }
    };

    AutoRequired<PrimeListener> listener;

    AutoCreateContext generating;
    CurrentContextPusher pusher(generating);
    AutoRequired<PrimeGenerator> generator;

    generating->Snoop(listener);
    generating->Initiate();
    generating->Wait(std::chrono::milliseconds(1000));
    generating->SignalShutdown(true);
}

} //End of test class
