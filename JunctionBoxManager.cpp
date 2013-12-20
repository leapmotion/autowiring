// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "JunctionBoxManager.h"

JunctionBoxManager::JunctionBoxManager(void) {
}

JunctionBoxManager::~JunctionBoxManager(void) {
}

std::shared_ptr<JunctionBoxBase> JunctionBoxManager::operator[](std::type_index pTypeIndex) {
  return m_junctionBoxes[pTypeIndex];
}