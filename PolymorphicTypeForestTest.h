#pragma once
#include "EnclosedContextTestBase.h"
#include "PolymorphicTypeForest.h"
#include "Object.h"

class PolymorphicTypeForestTest:
  public EnclosedContextTestBase
{
public:
  PolymorphicTypeForest<Object> forest;
};

