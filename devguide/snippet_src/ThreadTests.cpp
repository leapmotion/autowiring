#include <fstream>
#include "gtest/gtest.h"
#include <autowiring/Autowired.h>
#include <autowiring/ContextCreator.h>
#include <autowiring/AutoNetServer.h>
#include <autowiring/BasicThread.h>

#include <memory>
#include <thread>
#include <chrono>

namespace {
    class ThreadTests : public ::testing::Test {
     protected:
      // You can remove any or all of the following functions if its body
      // is empty.


      ThreadTests() {
        try {


        } catch (int e) {
            std::cout << "Broke " << e << std::endl;
        }

      }

      virtual ~ThreadTests() {
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



TEST_F(ThreadTests, BasicThread_Extend) {
    class FactorialCalculator : public BasicThread {
        public:
            unsigned long int factorial(int in){
                m_in = in;
                this->Run();
                return m_out;
            }
            virtual void Run() override {
                m_out = 1;
                for(int i = 1; i <= m_in; i++){
                    m_out *= i;
                }
            }
        private:
            int m_in;
            unsigned long int m_out;
    };

    AutoCreateContext ctxt;
    CurrentContextPusher pusher(ctxt);
    AutoRequired<FactorialCalculator> calculator;
    ctxt->Initiate();

    std::cout << "Factorial(7) = " << calculator->factorial(7) << std::endl;
    std::cout << "Factorial(49) = " << calculator->factorial(49) << std::endl;
    std::cout << "Factorial(65) = " << calculator->factorial(65) << std::endl;
    ctxt->SignalShutdown(true);
}

TEST_F(ThreadTests, BasicThread_Prime_Printer) {
    // !!!BasicThread_Class
    class PrimeGenerator : public BasicThread {
        public:
            void Run() override {
                unsigned long int test = 2;
                while (!ShouldStop())
                {
                    for (unsigned long int i = test/2; i > 0 ; --i) {
                        if (i == 1) {
                            std::cout << "Prime: " << test << std::endl;
                        }
                        if (test % i == 0) {break;}
                    }
                    test++;
                }
            }
    };

    AutoCreateContext ctxt;
    CurrentContextPusher pusher(ctxt);
    AutoRequired<PrimeGenerator> generator;

    ctxt->Initiate();
    ctxt->Wait(std::chrono::milliseconds(100));
    ctxt->SignalShutdown(true);
    // !!!END
}

TEST_F(ThreadTests, BasicThread_Prime) {
    // !!!BasicThread_Events
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

    class PrimeListener : public PrimeEvent{
        public:
            void onPrime(unsigned long int prime) override {
                std::cout << "Prime: " << prime << std::endl;
            }
    };

    AutoCreateContext ctxt;
    CurrentContextPusher pusher(ctxt);
    AutoRequired<PrimeGenerator> generator;
    AutoRequired<PrimeListener> listener;

    ctxt->Initiate();
    ctxt->Wait(std::chrono::milliseconds(100));
    ctxt->SignalShutdown(true);
    // !!!END
}

TEST_F(ThreadTests, BasicThread_PrimeFilter) {
    // !!!BasicThread_Filters
    class PrimeGenerator : public BasicThread {
        public:
            Autowired<AutoPacketFactory> packetFactory;
            void Run() override {
                unsigned long int test = 2;
                while (!ShouldStop())
                {
                    for (unsigned long int i = test/2; i > 0 ; --i) {
                        if (i == 1) {
                            auto packet = packetFactory->NewPacket();
                            packet->Decorate(test);
                        }
                        if (test % i == 0) {break;}
                    }
                    test++;
                }
            }
    };

    class PrimeListener{
        public:
            void AutoFilter(unsigned long int prime) {
                std::cout << "Prime: " << prime << std::endl;
            }
    };

    AutoCreateContext ctxt;
    CurrentContextPusher pusher(ctxt);
    AutoRequired<PrimeGenerator> generator;
    AutoRequired<PrimeListener> listener;

    ctxt->Initiate();
    ctxt->Wait(std::chrono::milliseconds(100));
    ctxt->SignalShutdown(true);
    // !!!END
}

TEST_F(ThreadTests, CoreThread_Use) {
    // !!!CoreThread_Class
    AutoCreateContext ctxt;
    CurrentContextPusher pusher(ctxt);

    AutoRequired<CoreThread> runner;
    ctxt->Initiate();

    *runner += []{
        std::cout << "Job ran." << std::endl;
    };

    int x = 0;
    auto func = [&x]{
        std::cout << "Value of x: " << x << std::endl;
    };
    x = 4;
    *runner += func;
    ctxt->SignalShutdown(true);
    // !!!END
}

TEST_F(ThreadTests, CoreThread_GracefulEnd) {
    AutoCreateContext ctxt;
    CurrentContextPusher pusher(ctxt);

    AutoRequired<CoreThread> runner;

    *runner += []{
        std::cout << "Jobs started." << std::endl;
    };

    int x = 0;
    auto func = [&x]{
        std::cout << "Value of x: " << x++ << std::endl;
    };

    for (int i = 0; i < 50; i++) {
        *runner += func;
    }
    ctxt->Initiate();
    ctxt->SignalShutdown(true, ShutdownMode::Graceful);
}

TEST_F(ThreadTests, CoreThread_ImmediateEnd) {
    AutoCreateContext ctxt;
    CurrentContextPusher pusher(ctxt);

    AutoRequired<CoreThread> runner;

    auto func = []{
        //while(false){
            std::cout << "Still running "  << std::endl;
        //}
    };
    *runner += func;

    ctxt->Initiate();
    ctxt->SignalShutdown(true, ShutdownMode::Graceful);
}

TEST_F(ThreadTests, BasicThread_Constructor) {
    // !!!BasicThread_Constructor
    AutoCreateContext ctxt;
    CurrentContextPusher pusher(ctxt);

    class FooThread : public BasicThread{
        public:
            FooThread(const char* pName = nullptr)
                :BasicThread(pName){}
            void Run() override {
                std::cout << "Thread starting" << std::endl;
                this->WaitFor(std::chrono::seconds(1));
                std::cout << "Thread done" << std::endl;

            }
    };
    ctxt->Initiate();
    AutoRequired<FooThread>(AutoCurrentContext(), "thread_foo");
    ctxt->SignalShutdown(true, ShutdownMode::Graceful);
    // !!!END
}

TEST_F(ThreadTests, BasicThread_Shutdown) {
    // !!!BasicThread_OnStop
    AutoCreateContext ctxt;
    CurrentContextPusher pusher(ctxt);

    class FooThread : public BasicThread{
        public:
            void OnStop(bool graceful) override{
                std::cout << "OnStop" << std::endl;
            }
            void Run() override {
                unsigned long int test = 2;
                while (!ShouldStop())
                {
                    for (unsigned long int i = test/2; i > 0 ; --i) {
                        if (i == 1) {
                            std::cout << "Prime: " << test << std::endl;
                        }
                        if (test % i == 0) {break;}
                    }
                    test++;
                }
                std::cout << "Should stop " << std::endl;
            }
    };
    ctxt->Initiate();
    AutoRequired<FooThread> foo;
    foo->Stop(true);
    // !!!END
}
TEST_F(ThreadTests, BasicThread_GetRunningTime) {
    // !!!BasicThread_GetRunningTime
    class PrimeGenerator : public BasicThread{
        public:
            void Run() override {
                unsigned long int test = 2;
                while (!ShouldStop())
                {
                    for (unsigned long int i = test/2; i > 0 ; --i) {
                        if (i == 1) {
                            std::chrono::milliseconds kernelTime, userTime;
                            this->GetThreadTimes(kernelTime, userTime);
                            std::cout << "Calculated primes up to: " << test
                                      << " in " << (&kernelTime)->count() << " kernel ms "
                                      << "and " << (&userTime)->count() << " user ms."
                                      << std::endl;
                        }
                        if (test % i == 0) {break;}
                    }
                    test++;
                }
            }
    };

    AutoCreateContext ctxt;
    CurrentContextPusher pusher(ctxt);

    AutoRequired<PrimeGenerator> timedPrimer;
    ctxt->Initiate();
    ctxt->Wait(std::chrono::milliseconds(100));
    ctxt->SignalShutdown(true);
    // !!!END
}

/* Not implemented on Mac or Linux
TEST_F(ThreadTests, BasicThread_ElevatePriority) {
    AutoCreateContext ctxt;
    CurrentContextPusher pusher(ctxt);

    class FooThread : public BasicThread{
        public:
            FooThread(const char* pName = nullptr)
                :BasicThread(pName){}
            void Run() override {
                std::cout << "Thread starting" << std::endl;
                FooThread::ElevatePriority priority(*this, ThreadPriority::Normal);

                std::cout << "Raised piority to normal" << std::endl;
                this->WaitFor(std::chrono::seconds(3));
                std::cout << "Thread done" << std::endl;
                this->GetContext()->SignalShutdown(false);
            }

    };
    ctxt->Initiate();
    AutoRequired<FooThread>(AutoCurrentContext(), "foo");
    AutoRequired<FooThread> foo;

    ctxt->Wait();

}*/

} //End of test class
