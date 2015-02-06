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
    class AutoFilterTests : public ::testing::Test {
     protected:
      // You can remove any or all of the following functions if its body
      // is empty.


      AutoFilterTests() {
        try {


        } catch (int e) {
            std::cout << "Broke " << e << std::endl;
        }

      }

      virtual ~AutoFilterTests() {
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

TEST_F(AutoFilterTests, Dummy) {
      EXPECT_EQ(1, 1.0);
}

TEST_F(AutoFilterTests, AutoFilter_PrimeFilter) {
    class PrimeGenerator : public BasicThread {
        public:
            PrimeGenerator() : BasicThread("Prime"){}
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

    AutoCurrentContext()->Initiate();
    AutoCreateContext ctxt;
    CurrentContextPusher pusher(ctxt);
    AutoRequired<PrimeGenerator> generator;
    AutoRequired<PrimeListener> listener;

    ctxt->Initiate();
    ctxt->Wait(std::chrono::milliseconds(100));
    ctxt->SignalShutdown(true);
}

TEST_F(AutoFilterTests, ImageFilterNet) {
    struct Image{};
    struct RawImage: public Image{};
    struct BlurredImage: public Image{};
    struct CircleList{};
    struct LineList{};
    struct FeatureList{};

    class Camera{
        public:
            AutoRequired<AutoPacketFactory> packetGenerator;
            void produceImage(int id){
                RawImage raw;
                std::cout << "Produced raw image # " << id << std::endl;
                auto packet = packetGenerator->NewPacket();
                packet->Decorate(raw);
            }
    };

    class BlurFilter{
        public:
            void AutoFilter(const RawImage img, BlurredImage &blurred){
                blurred = blur(img);
            }
            BlurredImage blur(const RawImage raw){
                std::cout << "Blurred image." << std::endl;
                return BlurredImage();
            }
    };

    class CircleFinder : public CoreThread {
        public:
            void AutoFilter(const BlurredImage img, CircleList &circles){
                this->WaitFor(std::chrono::milliseconds(500)); //simulate long running process
                circles = findCircles(img);
                //return Deferred(this);
            }
            CircleList findCircles(const BlurredImage blurred){
                std::cout << "Found circles." << std::endl;
                return CircleList();
            }
    };

    class LineFinder {
        public:
            AutoRequired<AutoPacketFactory> packetGenerator;
            void AutoFilter(const BlurredImage img, LineList &lines){
                lines = findLines(img);
            }
            LineList findLines(const BlurredImage blurred){
                std::cout << "Found lines." << std::endl;
                return LineList();
            }
    };

    class GraphOut {
        public:
            void AutoFilter(const CircleList circles, const LineList lines, AutoPacket &packet){
                std::cout << "Received final packet with lines and circles. " << packet.Has<RawImage>() << std::endl;
            }
    };

    AutoCreateContext ctxt;
    ctxt->SetCurrent();

    AutoRequired<Camera> camera;
    AutoRequired<BlurFilter> blurFilter;
    AutoRequired<CircleFinder> circleFilter;
    AutoRequired<LineFinder> lineFilter;
    AutoRequired<GraphOut> graphOutput;

    ctxt->Initiate();

    camera->produceImage(1);
}

TEST_F(AutoFilterTests, AutoFilter_Types) {
    class T{public: int attribute = 0;};
    class W{public: int property = 0;};
    class Q{public: int member = 0;};
    class Input_ByValue{
        // !!!AutoFilter_InType_ByValue
        void AutoFilter(T value){}
        // !!!END
    };
    class Input_ByRef{
        // !!!AutoFilter_InType_ByRef
        void AutoFilter(const T& reference){}
        // !!!END
    };
    class Input_SharedPtr{
        // !!!AutoFilter_InType_shared_ptr
        void AutoFilter(std::shared_ptr<const T> pointer){}
        // !!!END
    };

    class Output_ByRef{
        // !!!AutoFilter_OutType_ByRef
        void AutoFilter(T& reference){}
        // !!!END
    };
    class Output_SharedPtr{
        // !!!AutoFilter_OutType_shared_ptr
        void AutoFilter(std::shared_ptr<T> pointer){}
        // !!!END
    };
    class Output_RValue{
        // !!!AutoFilter_OutType_Rvalue
        void AutoFilter(T&& temp){}
        // !!!END
    };
    class InOutput_Array{
        // !!!AutoFilter_Type_array
        void AutoFilter(T* arg[]){}
        // !!!END
    };
    class InOutput_Packet{
        // !!!AutoFilter_Type_packet
        void AutoFilter(AutoPacket &packet){}
        // !!!END
    };
    class ReturnType_void{
        // !!!AutoFilter_ReturnType_void
        void AutoFilter(T input, W& output){}
        // !!!END
    };
    // !!!AutoFilter_ReturnType_Deferred
    class DeferredFilter : public CoreThread{
        Deferred AutoFilter(T input, W& output){
            return Deferred(this);
        }
    };
    // !!!END
    // !!!AutoFilter_BasicAutofilter
    class BasicFilter {
        void AutoFilter(T inputByValue, const Q& inputByRef, W& output){
            output.property = inputByValue.attribute + inputByRef.member;
        }
    };
    // !!!END

}

} //End of test class
