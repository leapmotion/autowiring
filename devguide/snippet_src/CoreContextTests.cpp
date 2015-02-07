#include <fstream>
#include "gtest/gtest.h"
#include <autowiring/Autowired.h>
#include <autowiring/ContextCreator.h>
#include <autowiring/AutoNetServer.h>

namespace {
    class CoreContextTests : public ::testing::Test {
     protected:
      // You can remove any or all of the following functions if its body
      // is empty.


      CoreContextTests() {
        try {


        } catch (int e) {
            std::cout << "Broke " << e << std::endl;
        }

      }

      virtual ~CoreContextTests() {
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

class Foo{};
class Bar{};
class FooPeer{};
class BarPeer{};

TEST_F(CoreContextTests, Dummy) {
      EXPECT_EQ(1, 1.0);
      AutoGlobalContext global;
      global->Initiate();
      AutoCreateContext ctxt;
      CurrentContextPusher pusher(ctxt);
      //AutoRequired<AutoNetServer> server;

      AutoCreateContext voidCtxt;
      CurrentContextPusher push(voidCtxt);
      //AutoRequired<AutoNetServer> server;
      voidCtxt->Initiate();
      AutoCreateContextT<Foo> foo1Ctxt;

      std::shared_ptr<CoreContext> foo4Ctxt = foo1Ctxt->CreatePeer<FooPeer>();
      std::shared_ptr<CoreContext> bar1Ctxt = foo1Ctxt->CreatePeer<Bar>();
      std::shared_ptr<CoreContext> bar2Ctxt = bar1Ctxt->CreatePeer<BarPeer>();



      global->Dump(std::cout);
      //AutoCurrentContext()->Wait();
}

TEST_F(CoreContextTests, CoreContext_AutoCreateContext) {
    // !!!Context_AutoGlobalContext
    AutoGlobalContext global;
    // !!!END
    // !!!Context_AutoCurrentContext
    AutoCurrentContext current;
    // !!!END
      AutoCreateContext ctxt;
      CurrentContextPusher pusher(ctxt);
    // !!!Context_AutoCreateContext
    AutoCreateContext childContext;
    // !!!END
    // !!!Context_AutoCreateContextT
    class Foo{}; //dummy class
    AutoCreateContextT<Foo> childContextWithSigil;
    // !!!END
}

TEST_F(CoreContextTests, CoreContext_Class_Create) {
    // !!!Context_Class_Create
    AutoGlobalContext global;
    AutoCreateContext childContext;

    class Foo{}; //dummy class
    AutoCreateContextT<Foo> childContextWithSigil;
    // !!!END
}

TEST_F(CoreContextTests, DISABLED_CoreContext_Class_Enumerate) {
    AutoCreateContext ctxt;
    AutoCreateContext childContext;
    CurrentContextPusher pusher(childContext);
    class Foo{}; //dummy class
    AutoCreateContextT<Foo> childContextWithSigil;

    // !!!Context_Class_Enumerate
    AutoGlobalContext global;
    for(auto context : ContextEnumerator(global)){
        std::cout << "Enumerating "
                  << autowiring::demangle(&context->GetSigilType())
                  << " within global context."
                  << std::endl;
    }

    for(auto context : ContextEnumeratorT<Foo>(childContext)){
        std::cout << "Enumerating "
                  << autowiring::demangle(&context->GetSigilType())
                  << " within childContext."
                  << std::endl;
    }

    for(auto context : CurrentContextEnumerator()){
        std::cout << "Enumerating "
                  << autowiring::demangle(&context->GetSigilType())
                  << " in current context."
                  << std::endl;
    }
    // !!!END
    {
        // !!!CoreContext_EnumeratePusher
        AutoGlobalContext global;
        for(auto context : ContextEnumerator(global)){
            CurrentContextPusher push(context);
            // Perform some action
        } //Previous context restored
        // !!!END
    }
    {
        // !!!CoreContext_EnumerateTPusher
        AutoGlobalContext global;
        for(auto context : ContextEnumeratorT<Foo>(global)){
            CurrentContextPusher push(context);
            // Perform some action
        } //Previous context restored
        // !!!END
    }
}

TEST_F(CoreContextTests, CoreContext_Sigil_Subclass) {
    AutoCreateContext ctxt;
    AutoCreateContext childContext;
    CurrentContextPusher pusher(childContext);
    class Foo{
        public:
            class Bar{};
    };
    class Muu : Foo{};
    AutoCreateContextT<Foo> FooContext;
    AutoCreateContextT<Foo::Bar> FooBarContext;
    AutoCreateContextT<Muu> MuuContext;

    for(auto context : ContextEnumerator(childContext)){
        std::cout << "Sigil: " << autowiring::demangle(&context->GetSigilType()) << std::endl;
    }


}

TEST_F(CoreContextTests, CoreContext_Class_AutoFire) {
    AutoCreateContext child;
    CurrentContextPusher pusher(child);
    // !!!Context_Class_AutoFire
    class Event{
        public:
            virtual void intEvent(int data) = 0;
    };

    class EventReceiver : public Event{
        public:
            EventReceiver(){}
            void intEvent(int data) override {
                std::cout << "Receiver Handled event with data: " << data << std::endl;
            }
    };

    AutoCurrentContext ctxt;
    AutoFired<Event> eventDispatcher;
    AutoRequired<EventReceiver> receiver;

    ctxt->Initiate(); //required for events to dispatch

    eventDispatcher(&Event::intEvent)(12);
    // !!!END
}

TEST_F(CoreContextTests, CoreContext_Overview) {
    AutoCreateContext child;
    CurrentContextPusher pusher(child);
    // !!!Context_Overview
    class Bar{};
    class Foo{
        public:
            AutoRequired<Bar> bar;
    };
    AutoCreateContext ctxt;
    CurrentContextPusher push(ctxt);

    AutoRequired<Foo> foo;
    AutoRequired<Bar> bar; //bar and foo.bar point to the same instance
    // !!!END
}

TEST_F(CoreContextTests, CoreContext_Overview_Autowire) {
    AutoCreateContext child;
    CurrentContextPusher pusher(child);
    // !!!Context_Overview_Autowire
    class Bar{};
    class Foo{
        public:
            Autowired<Bar> bar;
    };
    AutoCreateContext ctxt;
    CurrentContextPusher push(ctxt);

    AutoRequired<Foo> foo; //foo.bar still unwired
    AutoRequired<Bar> bar; //foo.bar is wired to bar
    // !!!END
}

TEST_F(CoreContextTests, DISABLED_CoreContext_Switching) {
    class Bar{};
    class Foo {
        public:
            void switchContext(){
                AutoGlobalContext global;
                CurrentContextPusher pusher(global);
                AutoCurrentContext context;
                std::cout << "Current context "
                          << autowiring::demangle(&context->GetSigilType())
                          << std::endl;
            }
    };
    AutoCreateContextT<Foo> child;
    CurrentContextPusher pusher(child);

    AutoRequired<CoreThread> thread;
    AutoRequired<Foo> foo;
    *thread += []{
    }; //end of lambda

    std::cout << "Current context before  "
              << autowiring::demangle(AutoCurrentContext()->GetSigilType())
              << std::endl;
    child->Initiate();
    child->Wait(std::chrono::milliseconds(300));

    {
        AutoCreateContextT<Bar> subchild;
        //CurrentContextPusher push(subchild);
        subchild->SetCurrent();
        std::cout << "Current context during  "
              << autowiring::demangle(AutoCurrentContext()->GetSigilType())
              << std::endl;
        subchild->SignalTerminate();
    }
    AutoCurrentContext()->Wait(std::chrono::seconds(7));

    std::cout << "Current context after  "
              << autowiring::demangle(AutoCurrentContext()->GetSigilType())
              << std::endl;

}

TEST_F(CoreContextTests, CoreContext_WireResolution) {
    class Foo{
        public:
            Foo(){
                std::cout << "Foo created." << std::endl;
            }
    };

    AutoGlobalContext global;
    AutoCreateContext c1;
    AutoCreateContext c2;
    c2->SetCurrent();
    Autowired<Foo> c2Foo;
    c2Foo.NotifyWhenAutowired([]{std::cout << "c2 Foo wired" << std::endl;});
    c1->SetCurrent();
    Autowired<Foo> c1Foo;
    c1Foo.NotifyWhenAutowired([]{std::cout << "c1 Foo wired" << std::endl;});

    AutoCreateContext b1;
    AutoCreateContext b2;
    b2->SetCurrent();
    Autowired<Foo> b2Foo;
    b2Foo.NotifyWhenAutowired([]{std::cout << "b2 Foo wired" << std::endl;});
    b1->SetCurrent();
    Autowired<Foo> b1Foo;
    b1Foo.NotifyWhenAutowired([]{std::cout << "b1 Foo wired" << std::endl;});

    b2->Inject<Foo>();
}

TEST_F(CoreContextTests, DISABLED_CoreContext_Wireexception) {
    class Foo{
        public:
            Foo(){
                throw std::exception();
            }
            Foo(bool woo){};
            void soundOff(){
                std::cout << "Foo invoked" << std::endl;
            }
    };
    class Bar: public Foo
    {
        public:
            Bar()
             :Foo(true)
             {}
    };

    AutoCreateContextT<Foo> child;
    CurrentContextPusher pusher(child);
    Autowired<Foo> foo;
    foo.NotifyWhenAutowired([]{std::cout << "Foo wired" << std::endl;});
    try {
      AutoRequired<Foo> woot;
    } catch (std::exception e) {
        std::cout << "Exception swallowed " << e.what() << std::endl;
    }
    AutoConstruct<Foo> woot(true);

}
TEST_F(CoreContextTests, DISABLED_CoreContext_CommonSuperType) {
    class Foo{
            public:
                std::string idd;
                Foo(){
                    idd = "foo";
                }
                void virtual fling() = 0;
        };
    class BarOne: public Foo{
            public:
                std::string idd;
                BarOne(){
                    idd = "barone";
                }
                void fling() override {};
    };
    class BarTwo: public Foo{
            public:
                std::string idd;
                BarTwo(){
                    idd = "bartwo";
                }
                void fling() override {};
    };

    AutoRequired<BarOne> b1;
    //AutoRequired<BarTwo> b2;
    AutoRequired<Foo> foo;
    std::cout << "Identity of foo " << foo->idd << std::endl;
}

} //End of test class
