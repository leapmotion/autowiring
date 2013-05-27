#include "stdafx.h"
#include "CreationListenerTest.h"
#include "ContextCreator.h"
#include <string>

extern const char sc_pipelineName[] = "Pipeline";

TEST_F(CreationListenerTest, VerifyMapping) {
  ContextCreator<std::wstring, sc_pipelineName> pipelineContext;
}