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

ContextEnumerator::iterator::iterator(const std::shared_ptr<CoreContext>& root, const std::shared_ptr<CoreContext>& cur) :
  m_root(root ? root->GetParentContext() : nullptr),
  m_cur(cur)
{}

ContextEnumerator::iterator::~iterator(void) {}

void ContextEnumerator::iterator::_next(void) {
  std::shared_ptr<CoreContext> i;
  for(
    // Try to traverse the first child if possible:
    i = m_cur->FirstChild();

    // Continue until we find something and we haven't walked off the end:
    !i && m_cur;

    // m_cur is ascending, we are right-traversing
    i = m_cur->NextSibling(),
    m_cur = m_cur->GetParentContext()
  );

  if(m_cur == m_root)
    // Root hit, done traversing
    m_cur.reset();
  else
    // Otherwise, take on the value enumerated to during the loop
    m_cur = i;
}

const ContextEnumerator::iterator& ContextEnumerator::iterator::operator++(void) {
  _next();
  return *this;
}

CurrentContextEnumerator::CurrentContextEnumerator(void) :
  ContextEnumerator(CoreContext::CurrentContext())
{
}