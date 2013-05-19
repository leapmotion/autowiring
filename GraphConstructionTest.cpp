#include "stdafx.h"
#include "GraphConstructionTest.h"
#include "Autowired.h"

class NodeTwo;
class NodeThree;

class NodeOne
{
  Autowired<NodeTwo> n2;
};

class NodeTwo
{
  Autowired<NodeThree> n3;
};

class NodeThree
{
  AutoRequired<NodeOne> n1;
  AutoRequired<NodeTwo> n2;
};

TEST_F(GraphConstructionTest, VerifySimpleGraph)
{
  // Make our items:
  AutoRequired<NodeOne> n1;
  AutoRequired<NodeTwo> n2;
  AutoRequired<NodeThree> n3;

  // Dump a graph from the context object:
  //m_create->DumpGraph();
}
