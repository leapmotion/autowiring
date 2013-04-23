#include "stdafx.h"
#include "Autowirer.h"
#include "ContextMember.h"
#include "CoreContext.h"
#include <string>

using namespace std;

const char sc_emptyName[] = "";

Autowirer::Autowirer(const cpp11::shared_ptr<Autowirer>& pParent):
  m_pParent(pParent)
{
}

Autowirer::~Autowirer(void)
{
  // Explicit deleters to simplify implementation of SharedPtrWrapBase
  for(t_mpType::iterator q = m_byType.begin(); q != m_byType.end(); q++)
    delete q->second;

  // Explicit deleters to simplify base deletion
  for(t_deferredList::iterator q = m_deferred.begin(); q != m_deferred.end(); q++)
    delete *q;
}

void Autowirer::AddContextMember(ContextMember* ptr)
{
  boost::lock_guard<boost::mutex> lk(m_lock);

  // Insert context members by name.  If there is no name, just return the base pointer.
  if(!ptr->GetName())
    return;
  
  string name = ptr->GetName();
  ContextMember*& location = m_byName[name];
  if(location)
    throw std::runtime_error("Two values have been mapped to the same key in the same context");

  // Trivial insertion and return:
  location = ptr;
}

cpp11::shared_ptr<CoreContext> CreateContextThunk(void) {
  return CoreContext::CurrentContext()->Create();
}