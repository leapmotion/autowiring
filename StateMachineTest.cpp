// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "StateMachine.h"
#include "StateMachineTest.h"

using namespace std;

enum PlaybackState {
  Playing,
  Paused,
  Stopped
};

enum DeviceCommand {
  Play,
  Pause,
  Stop
};

class SimpleRecordingDevice:
  public StateMachine<PlaybackState, DeviceCommand>
{
public:
  SimpleRecordingDevice(void):
    StateMachine(
      initializeStateMachine(Stopped) //Initial state of the machine is "Stopped"
      << Stopped << Play << Playing // When on state "Stoped", transition to "Playing" when you receive a "Play" event
      << Stopped << Pause << Stopped
      << Paused << Play << Playing
      << Paused << Stop << Stopped
      << Playing << Stop << Stopped
      << Playing << Pause << Paused
    ),
    m_playingCallCount(0),
    m_playInputCount(0)
  {
    // Set a callback when "Play" event is processed
    *this += Play, &SimpleRecordingDevice::OnPlayInput;
    
    // Set a callback when "Playing" state is entered
    *this += Playing, [this] {
      this->OnStartPlaying();
    };
  }

  using StateMachine<PlaybackState, DeviceCommand>::getState;
  using StateMachine<PlaybackState, DeviceCommand>::transition;

  int m_playInputCount;
  int m_playingCallCount;

  void OnPlayInput(void) {
    m_playInputCount++;
  }

  void OnStartPlaying(void) {
    m_playingCallCount++;
  }
};

TEST_F(StateMachineTest, RecordingDeviceTest) {
  SimpleRecordingDevice device;

  // Verify that the device is in the default state:
  ASSERT_EQ(Stopped, device.getState()) << "State machine's ground state was incorrect";

  // Hit the playback state and then transition back to the stopped state:
  device.transition(Play);
  device.transition(Stop);
  ASSERT_EQ(1, device.m_playingCallCount) << "State machine did not hit the input state the expected number of times";
  ASSERT_EQ(1, device.m_playInputCount) << "";
}


