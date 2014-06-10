#include "stdafx.h"
#include "ContextEnumerator.h"
#include "CoreContext.h"

ContextEnumerator::ContextEnumerator(const std::shared_ptr<CoreContext>& root):
  m_root(root)
{
}

ContextEnumerator::~ContextEnumerator(void)
{
}

ContextEnumerator::iterator::~iterator(void) {}

const ContextEnumerator::iterator& ContextEnumerator::iterator::operator++(void) {
  // Use next to guarantee validity of peek at tree structure
  std::shared_ptr<CoreContext> next;
  // Try to traverse into the first child
  if ((next = m_cur->FirstChild())) {
    m_cur = next;
    return *this;
  }
  // If there are no children then check to see if we are at the root node
  if (m_cur == m_root) {
    m_cur = nullptr;
    return *this;
  }
  // Try to traverse to the next sibling
  while (!(next = m_cur->NextSibling())) {
    if (m_cur == m_root) {
      // If at the root node, halt traversal
      m_cur = nullptr;
      return *this;
    }
    m_cur = m_cur->GetParentContext();
  }
  m_cur = next;
  return *this;
}

CurrentContextEnumerator::CurrentContextEnumerator(void) :
  ContextEnumerator(CoreContext::CurrentContext())
{
}