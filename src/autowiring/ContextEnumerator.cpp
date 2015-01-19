// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "ContextEnumerator.h"
#include "CoreContext.h"

ContextEnumerator::ContextEnumerator(const std::shared_ptr<CoreContext>& root):
  m_root(root)
{}

ContextEnumerator::~ContextEnumerator(void) {}

ContextEnumerator::iterator::iterator(const std::shared_ptr<CoreContext>& root, const std::shared_ptr<CoreContext>& cur) :
  m_root(root ? root->GetParentContext() : nullptr),
  m_cur(cur)
{}

ContextEnumerator::iterator::~iterator(void) {}

void ContextEnumerator::iterator::_next(const std::shared_ptr<CoreContext>& start) {
  // First node to search
  std::shared_ptr<CoreContext> i = start;
  
  // Continue until we find something and we haven't walked off the end:
  while(!i && m_cur) {
    // m_cur is ascending, we are right-traversing
    i = m_cur->NextSibling();
    m_cur = m_cur->GetParentContext();
  }

  if(m_cur == m_root)
    // Root hit, done traversing
    m_cur.reset();
  else
    // Otherwise, take on the value enumerated to during the loop
    m_cur = i;
}


const ContextEnumerator::iterator& ContextEnumerator::iterator::NextSibling(void) {
  _next(m_cur->NextSibling());
  return *this;
}

const ContextEnumerator::iterator& ContextEnumerator::iterator::operator++(void) {
  _next(m_cur->FirstChild());
  return *this;
}

CurrentContextEnumerator::CurrentContextEnumerator(void) :
  ContextEnumerator(CoreContext::CurrentContext())
{
}
