#include "stdafx.h"
#include "UnutteredSelectingFixture.h"
#include "MicroBolt.h"

struct SelfSelect {};

BOLT_TO(SelfSelect, UnutteredSelectingFixture);

template<class S, class I, S from, I input, S to>
struct Transition {};

enum PlaybackStates {
  Stopped,
  Paused,
  Running
};

enum Commands {
  Play,
  Stop,
  Pause
};

template<>
struct Transition<PlaybackStates, Commands, Stopped, Play, Running> {};

class ComdatClass {
public:
  ComdatClass(void) {};
};

UnutteredSelectingFixture::UnutteredSelectingFixture()
{
}

UnutteredSelectingFixture::~UnutteredSelectingFixture()
{
}
