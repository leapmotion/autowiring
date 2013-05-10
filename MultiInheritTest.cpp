#include "stdafx.h"
#include "MultiInheritTest.h"
#include "Autowired.h"
#include "CoreContext.h"
#include "TestFixtures/MultiInherit.h"

TEST_F(MultiInheritTest, VerifyCast) {
  // Create a dummy context and make it current:
  Autowired<CoreContext> ctxt;
  ctxt.Create();
  ctxt->SetCurrent();

  // Make sure that we still have a context:
  ASSERT_TRUE(ctxt.IsAutowired()) << "Create call must create a context";

  // Insert a MultiInherit object:
  std::shared_ptr<MultiInherit> obj(new MultiInherit());
  ctxt->Add(obj);

  // Autowire in the pObj:
  Autowired<MultiInherit> wiredPobj;
  ASSERT_TRUE(wiredPobj.IsAutowired()) << "Autowiring failed for a multi-inheritance object";

  // Verify that we get a pObj back with correct casting:
  EXPECT_EQ(obj, wiredPobj) << "Autowiring failed on a multiple inheritance object";
}
