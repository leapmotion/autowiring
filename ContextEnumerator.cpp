#include "stdafx.h"
#include "ContextEnumerator.h"

ContextEnumerator::ContextEnumerator(void):
  m_root(CoreContext::CurrentContext())
{

}

ContextEnumerator::ContextEnumerator(const std::shared_ptr<CoreContext>& root):
  m_root(root)
{
}

ContextEnumerator::~ContextEnumerator(void)
{
}

const ContextEnumerator::iterator& ContextEnumerator::iterator::operator++(void) {
  std::shared_ptr<CoreContext> next;
  if(
    // Try to traverse into the first child
    !(next = m_cur->GetFirstChild()) &&

    // If that doesn't work, then try the next sibling
    !(next = m_cur->GetNextSibling())
    )
    // No children, no siblings.  Ascend until we find an ancestor with a sibling.
    do {
      m_cur = m_cur->GetParentContext();
    } while(m_cur != m_root && !(next = m_cur->GetNextSibling()));

  // Update, return
  m_cur = next;
  return *this;
}