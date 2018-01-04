// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "HeteroBlock.h"
#include "CreationRules.h"
#include <functional>
#include <new>
#include <numeric>

using namespace autowiring;

HeteroBlock::HeteroBlock(const HeteroBlockEntry* pFirst, const HeteroBlockEntry* pLast)
{
  entries.reserve(pLast - pFirst);

  // All entries we will stamp on the block space once everything is figured out
  std::vector<BlockHeader> headers;
  headers.resize(pLast - pFirst);

  // Minimum alignment requirement for the entire space
  size_t maxAlign = sizeof(void*);

  // Need to set up the first and last entries
  headers.front().pfnDtor = pFirst->pfnDtor;

  // Allocate enough space for everything:
  size_t ncb = 0;

  size_t temp;
  size_t* sizePrior = &temp;

  for (size_t i = 0; i < headers.size(); i++) {
    // Minimum alignment is the word size for this architecture.  This prevents bytes
    // from getting sliced up and shared among multiple processors, which can create
    // performance problems.  This also ensures that the data members of the block
    // header are correctly aligned.
    size_t align = pFirst[i].id.block->align;
    if (align <= sizeof(void*))
      align = sizeof(void*);
    else if (maxAlign < align)
      maxAlign = align;

    // Shift amount is the amount by which ncb changes in the next block
    *sizePrior = ncb;

    // Shift over the amount needed due to misalignment.  This works because we always
    // assume that the space at offset [0] is perfectly aligned for all data.
    // This crazy arithmetic is a simplification of the following expression:
    //
    //  size_t slop = ncb % align;
    //  size_t slack = (align - slop) % align;
    //  ncb += slack;
    //
    // Written in one line, this is just:
    //
    //  ncb += (align - (ncb % align)) % align;
    //
    // However, `modulo x` is an idempotent operation under addition and subtraction,
    // which means that the above expression simplifies down to just this:
    ncb += (align - ncb) % align;

    // Update the offset, advance by the size of the block header
    entries[pFirst[i]].offset = ncb;
    ncb += pFirst[i].id.block->ncb;

    // Store the block entry
    *sizePrior = ncb - *sizePrior;
    sizePrior = &headers[i].nextSize;
    headers[i].pfnDtor = pFirst[i].pfnDtor;

    // Increment past the header:
    ncb += sizeof(BlockHeader);
  }

  size_t alignedRequest = ncb + sizeof(BlockHeader);
  alignedRequest += (maxAlign - alignedRequest) % maxAlign;
  uint8_t* pBlock = (uint8_t*)autowiring::aligned_malloc(alignedRequest, maxAlign);
  if (!pBlock)
    throw std::bad_alloc();

  // Initialize the deleter sentinel with null:
  *(BlockHeader*)(pBlock + ncb) = {};

  size_t firstSize = pFirst->id.block->ncb;
  block.reset(
    pBlock,
    [firstSize] (void* ptr) {
      size_t curSize = firstSize;
      for (
        BlockHeader* pCur = (BlockHeader*)((uint8_t*)ptr + firstSize);
        pCur->pfnDtor;
        curSize = pCur->nextSize,
        pCur = (BlockHeader*)((uint8_t*)pCur + pCur->nextSize) + 1
      )
        // Run the deleter here
        pCur->pfnDtor((uint8_t*)pCur - curSize);

      // Now we can free the whole space
      autowiring::aligned_free(ptr);
    }
  );

  // Placement construct all objects and accumulate the offset:
  size_t curSize = firstSize;
  BlockHeader* pCurHeader = (BlockHeader*)&pBlock[firstSize];
  for (size_t i = 0; i < headers.size(); i++) {
    try {
      // Allocate this space right away:
      pFirst[i].pfnCtor((uint8_t*)pCurHeader - curSize);
    }
    catch (...) {
      // Zeroize the deleter, something went wrong, and forward the exception
      pCurHeader->pfnDtor = nullptr;
      throw;
    }

    // Initialize, track, and advance
    *pCurHeader = headers[i];
    curSize = pCurHeader->nextSize;
    pCurHeader = (BlockHeader*)((uint8_t*)pCurHeader + pCurHeader->nextSize) + 1;
  }
}

HeteroBlock::~HeteroBlock(void) {}
