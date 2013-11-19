#pragma once
#include "ContextMember.h"

extern const char s_expectedFixtureName[];
extern const char s_pipelinename[];

class SelfSelectingFixture:
  public ContextMember
{
public:
  SelfSelectingFixture(void);

private:
  int magic;

public:
  /// <returns>
  /// True if the internally-held magic value is initialized to the correct value
  /// </returns>
  bool IsMagicCorrect(void) const;
};