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
  std::shared_ptr<CoreContext> next;
  if(
    // Try to traverse into the first child
    !(next = m_cur->FirstChild()) &&

    // If that doesn't work, then try the next sibling
    !(next = m_cur->NextSibling())
  ) {
    // No children, no siblings.  Ascend until we find an ancestor with a sibling.
    while(
      // Continue as long as we don't hit the root
      m_cur != m_root &&

      // And we will also continue as long as our current sibling is null
      !(next = m_cur->NextSibling())
    ) {
      // If we can't stop, then ascend
      m_cur = m_cur->GetParentContext();
      assert(m_cur);
    }
  }

  // Update, return
  m_cur = next;
  return *this;
}

CurrentContextEnumerator::CurrentContextEnumerator(void) :
  ContextEnumerator(CoreContext::CurrentContext())
{
}