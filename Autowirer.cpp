#include "stdafx.h"
#include "Autowirer.h"
#include "ContextMember.h"
#include "CoreContext.h"
#include "AutowirableSlot.h"
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
  for(t_deferred::iterator q = m_deferred.begin(); q != m_deferred.end(); q++)
    delete q->second;
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

void Autowirer::NotifyWhenAutowired(const AutowirableSlot& slot, const cpp11::function<void()>& listener) {
  boost::lock_guard<boost::mutex> lk(m_deferredLock);

  // If the slot is already autowired then we can invoke the listener here and return early
  if(slot.IsAutowired())
    return listener();

  t_deferred::iterator q = m_deferred.find(&slot);
  if(q == m_deferred.end())
    if(m_pParent)
      // Try the parent context first, it could be present there
      return m_pParent->NotifyWhenAutowired(slot, listener);
    else
      throw std::domain_error("An attempt was made to observe a principal not in this context");

  q->second->AddPostBindingListener(listener);
}

cpp11::shared_ptr<CoreContext> CreateContextThunk(void) {
  return CoreContext::CurrentContext()->Create();
}