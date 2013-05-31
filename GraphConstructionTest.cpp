// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "GraphConstructionTest.h"
#include "Autowired.h"
#include "AutoNetworkMonitor.h"

class NodeTwo;
class NodeThree;

class NodeOne:
  public ContextMember
{
public:
  NodeOne(void) {
  }
  Autowired<NodeTwo> n2;
};

class NodeTwo:
  public ContextMember
{
  Autowired<NodeThree> n3;
};

class NodeThree:
  public ContextMember
{
  AutoRequired<NodeOne> n1;
  AutoRequired<NodeTwo> n2;
};

TEST_F(GraphConstructionTest, VerifySimpleGraph)
{
  // Set up the network monitor first:
  AutoRequired<AutoNetworkMonitor> networkMon;

  // Make our items:
  AutoRequired<NodeOne> n1;
  AutoRequired<NodeTwo> n2;
  AutoRequired<NodeThree> n3;
}
