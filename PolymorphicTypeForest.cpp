#include "stdafx.h"
#include "PolymorphicTypeForest.h"

PolymorphicTypeForest::~PolymorphicTypeForest(void) {
  for(auto q = m_trees.begin(); q != m_trees.end(); q++)
    delete q->second;
  for(auto q = m_memos.begin(); q != m_memos.end(); q++)
    delete q->second;
}

bool PolymorphicTypeForest::Contains(const type_info& ground, const type_info& type) const {
  auto q = m_memos.find(GroundedCoordinate(ground, type));
  return
    q == m_memos.end() ?
    false :
    !!q->second;
}