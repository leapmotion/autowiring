// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "Autowirer.h"
#include "ContextMember.h"
#include "CoreContext.h"
#include "AutowirableSlot.h"
#include <list>
#include EXCEPTION_PTR_HEADER

using namespace std;

Autowirer::Autowirer(const std::shared_ptr<Autowirer>& pParent):
  m_pParent(pParent)
{
}

Autowirer::~Autowirer(void)
{
  // Notify all ContextMember instances that their parent is going away
  for(auto q = m_contextMembers.begin(); q != m_contextMembers.end(); q++)
    (*q)->ReleaseAll();

  // Release all event sender links:
  for(std::set<EventManagerBase*>::iterator q = m_eventSenders.begin(); q != m_eventSenders.end(); q++)
    (**q).ReleaseRefs();

  // Notify our parent (if we're still connected to the parent) that our event receivers are going away:
  if(m_pParent)
    m_pParent->RemoveEventSenders(m_eventReceivers.begin(), m_eventReceivers.end());

  // Explicit deleters to simplify implementation of SharedPtrWrapBase
  for(t_mpType::iterator q = m_byType.begin(); q != m_byType.end(); ++q)
    delete q->second;

  // Explicit deleters to simplify base deletion
  for(t_deferred::iterator q = m_deferred.begin(); q != m_deferred.end(); ++q)
    delete q->second;
}

void Autowirer::UpdateDeferredElements(void) {
  std::list<DeferredBase*> successful;

  // Notify any autowired field whose autowiring was deferred
  // TODO:  We should also notify any descendant autowiring contexts that a new member is now available.
  {
    boost::lock_guard<boost::mutex> lk(m_deferredLock);
    for(t_deferred::iterator r = m_deferred.begin(); r != m_deferred.end(); ) {
      bool rs = (*r->second)();
      if(rs) {
        successful.push_back(r->second);

        // Temporary required because of the absence of a convenience eraser iterator with stl map on all platforms
        t_deferred::iterator rm = r++;
        m_deferred.erase(rm);
      }
      else
        r++;
    }
  }

  // Now, outside of the context of a lock, we destroy each successfully wired deferred member
  // This causes any listeners to be invoked, conveniently, outside of the context of any lock
  for(std::list<DeferredBase*>::iterator q = successful.begin(); q != successful.end(); ++q)
    delete *q;
}

void Autowirer::AddToEventSenders(EventManagerBase* pSender) {
  m_eventSenders.insert(pSender);

  // Scan the list for compatible receivers:
  for(auto q = m_eventReceivers.begin(); q != m_eventReceivers.end(); q++)
    *pSender += *q;
}

void Autowirer::RemoveEventSenders(t_rcvrSet::iterator first, t_rcvrSet::iterator last) {
  for(auto q = first; q != last; q++) {
    // n^2 sender unlinking
    for(auto r = m_eventSenders.begin(); r != m_eventSenders.end(); r++)
      **r -= *q;

    // Trivial erase:
    m_eventReceivers.erase(*q);
  }

  // Detour to the parent collection (if necessary)
  if(m_pParent)
    m_pParent->RemoveEventSenders(first, last);
}

void Autowirer::Snoop(const std::shared_ptr<EventReceiver>& pSnooper) {
  // If the passed type is a ContextMember, we can query relationship status
  ContextMember* pMember = dynamic_cast<ContextMember*>(pSnooper.get());
  if(pMember) {
    // Ancestry check:
    std::shared_ptr<Autowirer> target = pMember->GetContext();
    std::shared_ptr<Autowirer> cur = GetParentContext();

    while(cur && cur != target)
      cur = cur->GetParentContext();

    if(!cur)
      throw_rethrowable std::runtime_error("A context member attempted to snoop a context which was not a child context");
  } else {
    // Dynamic membership check:
    const type_info& info = typeid(*pSnooper);
    
    std::shared_ptr<Autowirer> cur = GetParentContext();
    for(; cur; cur = cur->GetParentContext()) {
      auto q = cur->m_byType.find(info.name());
      if(q != cur->m_byType.end())
        break;
    }
    if(!cur)
      throw_rethrowable std::runtime_error("A generic type attempted to snoop a context which was not a child context");
  }

  // Pass control to the event adder helper:
  ((AutowirerHelpers::AddPolymorphic<EventReceiver>&)*this).AddEventReceiver(pSnooper);
}

void Autowirer::FilterException(void) {
  auto rethrower = [] () {
    std::rethrow_exception(std::current_exception());
  };

  bool handled = false;
  for(auto q = m_filters.begin(); q != m_filters.end(); q++)
    try {
      (*q)->Filter(rethrower);
      handled = true;
    } catch(...) {
      // Do nothing
    }

  // Rethrow if unhandled:
  if(!handled)
    rethrower();
}

void Autowirer::FilterFiringException(const EventManagerBase* pSender, EventReceiver* pRecipient) {
  auto rethrower = [] () {
    std::rethrow_exception(std::current_exception());
  };

  bool handled = false;
  for(auto q = m_filters.begin(); q != m_filters.end(); q++)
    try {
      (*q)->Filter(rethrower, pSender, pRecipient);
      handled = true;
    } catch(...) {
      // Do nothing, filter didn't want to filter this exception
    }

  // Rethrow if unhandled:
  if(!handled)
    rethrower();
}

void Autowirer::AddContextMember(ContextMember* ptr) {
  boost::lock_guard<boost::mutex> lk(m_lock);

  // Always add to the set of context members
  m_contextMembers.insert(ptr);
}

void Autowirer::NotifyWhenAutowired(const AutowirableSlot& slot, const std::function<void()>& listener) {
  boost::lock_guard<boost::mutex> lk(m_deferredLock);

  // If the slot is already autowired then we can invoke the listener here and return early
  if(slot.IsAutowired())
    return listener();

  t_deferred::iterator q = m_deferred.find(&slot);
  if(q == m_deferred.end()) {
    if(m_pParent)
      // Try the parent context first, it could be present there
      return m_pParent->NotifyWhenAutowired(slot, listener);
    else
      throw_rethrowable std::domain_error("An attempt was made to observe a principal not in this context");
  }

  q->second->AddPostBindingListener(listener);
}

std::shared_ptr<CoreContext> CreateContextThunk(void) {
  return CoreContext::CurrentContext()->Create();
}

void Autowirer::Dump(std::ostream& os) const {
  boost::lock_guard<boost::mutex> lk(m_lock);
  for(auto q = m_byType.begin(); q != m_byType.end(); q++) {
    os << q->second->GetTypeInfo().name();
    std::shared_ptr<Object> pObj = q->second->AsObject();
    if(pObj)
      os << hex << " 0x" << pObj;
    os << endl;
  }
}

std::ostream& operator<<(std::ostream& os, const Autowirer& rhs) {
  rhs.Dump(os);
  return os;
}

void Autowirer::DebugPrintCurrentExceptionInformation() {
  try {
    std::rethrow_exception(std::current_exception());
  } catch(std::exception& ex) {
    std::cerr << ex.what() << std::endl;
  } catch(...) {
    // Nothing can be done, we don't know what exception type this is.
  }
}
