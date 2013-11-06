#pragma once

/// <summary>
/// A simple "decoration" class which will be added to a variety of sample packets
/// </summary>
template<int N>
class Decoration {
public:
  Decoration(void) :
    i(N)
  {}

  int i;
};
