#include "stdafx.h"
#include "HeteroBlock.h"
#include <functional>

using namespace autowiring;

HeteroBlock::HeteroBlock(std::initializer_list<HeteroBlockEntry> entries)
{
  // Compute the total amount of space that will be needed
  size_t nTotal = 0;
  for (const auto& entry : entries)
    nTotal += entry.ncb;

  // Allocate just this much space
  block.reset(new uint8_t[nTotal]);
}

HeteroBlock::~HeteroBlock(void)
{
}
