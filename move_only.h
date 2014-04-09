#pragma once

// Workaround until you can move values into a lambda capture in C++14
template<typename T>
class MoveOnly {
public:
  mutable T value;
  MoveOnly(T&& val):
    value(std::move(val)),
    alreadyMoved(false)
  {}
  MoveOnly(const MoveOnly& moveonly):
    value(std::move(moveonly.value)),
    alreadyMoved(false)
  {
    assert(!moveonly.alreadyMoved); //Can't "move" more than once
    moveonly.alreadyMoved = true;
  }
private:
  mutable bool alreadyMoved;
};
