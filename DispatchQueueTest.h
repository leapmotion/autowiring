#pragma once
#include "EnclosedContextTestBase.h"

class DispatchQueue;

class DispatchQueueTest:
  public EnclosedContextTestBase,
  public DispatchQueue
{
};
