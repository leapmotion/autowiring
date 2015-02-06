#include <fstream>
#include "gtest/gtest.h"
#include <autowiring/Autowired.h>
#include <autowiring/ContextCreator.h>
#include <autowiring/AutoNetServer.h>

namespace {
    class AutoFiredTests : public ::testing::Test {
     protected:
      // You can remove any or all of the following functions if its body
      // is empty.


      AutoFiredTests() {
        try {


        } catch (int e) {
            std::cout << "Broke " << e << std::endl;
        }

      }

      virtual ~AutoFiredTests() {
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

TEST_F(AutoFiredTests, Dummy) {
      EXPECT_EQ(1, 1.0);
}



TEST_F(AutoFiredTests, AutoFired_AutoFire) {
    AutoCreateContext child;
    CurrentContextPusher pusher(child);
    // !!!AutoFired_AutoFire
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

TEST_F(AutoFiredTests, AutoFired_Class) {
    AutoCreateContext child;
    CurrentContextPusher pusher(child);
    // !!!AutoFired_Event
    class Event {
        public:
            virtual void eventFunction(int) = 0;
    };
    // !!!END
    {
        // !!!AutoFired_Declare
        AutoFired<Event> eventDispatcher;
        // !!!END
        // !!!AutoFired_Dispatch
        eventDispatcher(&Event::eventFunction)(7);
        // !!!END
    }
    // !!!AutoFired_Receiver
    class Foo : Event {
        public:
            void eventFunction(int data) override {
                std::cout << "Event code " << data << " handled by Foo." << std::endl;
            }
    };
    // !!!END
    // !!!AutoFired_Example
    AutoCurrentContext context;
    AutoFired<Event> eventDispatcher;
    AutoRequired<Foo> eventHandler;
    context->Initiate();
    eventDispatcher(&Event::eventFunction)(12);
    // !!!END
    /* !!!AutoFired_Result
       Event code 12 handled by Foo.
    !!!END */
}

TEST_F(AutoFiredTests, AutoFired_AutoFire_Inheritance) {
    AutoCreateContext child;
    CurrentContextPusher pusher(child);
    // !!!AutoFired_AutoFire_Inheritance
    class Event{
        public:
            virtual void fireEvent() = 0;
    };

    class GeneralEvent : public Event{
        public:
            void fireEvent() override {
                std::cout << "--General Event" << std::endl;
            }
    };

    class FooEvent : public GeneralEvent {
        public:
            void fireEvent() override {
                std::cout << "--Foo Event." << std::endl;
            }
    };

    class BarEvent : public GeneralEvent  {
        public:
            void fireEvent() override {
                std::cout << "--Bar Event." << std::endl;
            }
    };

    class EventListener {
        public:
            AutoRequired<GeneralEvent> generalListener;
            AutoRequired<FooEvent> fooListener;
            AutoRequired<BarEvent> barListener;
    };

    class EventDispatcher {
        public:
            AutoFired<GeneralEvent> generalDispatcher;
            AutoFired<FooEvent> fooDispatcher;
            AutoFired<BarEvent> barDispatcher;
    };
    AutoCurrentContext ctxt;
    AutoRequired<EventListener> listener;
    AutoRequired<EventDispatcher> dispatcher;
    ctxt->Initiate(); //required for events to dispatch

    std::cout << "Fire General Event:" << std::endl;
    dispatcher->generalDispatcher(&GeneralEvent::fireEvent)();
    std::cout << "Fire Foo Event:" << std::endl;
    dispatcher->fooDispatcher(&FooEvent::fireEvent)();
    std::cout << "Fire Bar Event:" << std::endl;
    dispatcher->barDispatcher(&BarEvent::fireEvent)();

    // Results:
    // Fire General Event:
    // --General Event
    // --Bar Event.
    // --Foo Event.
    // Fire Foo Event:
    // --Foo Event.
    // Fire Bar Event:
    // --Bar Event.
    // !!!END
}


} //End of test class
