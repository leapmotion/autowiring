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
}

cpp11::shared_ptr<ContextMember> Autowirer::Add(ContextMember* pContextMember)
{
  cpp11::shared_ptr<ContextMember> interior = AddInternal<ContextMember>(pContextMember);
  boost::lock_guard<boost::mutex> lk(m_lock);

  // Insert context members by name.  If there is no name, just return the base pointer.
  if(!pContextMember->GetName())
    return interior;
  
  string name = pContextMember->GetName();
  t_mpName::iterator q = m_byName.upper_bound(name);
  if(q != m_byName.end() && q->first == name)
    throw std::runtime_error("Two values have been mapped to the same key in the same context");

  class Lambda {
  public:
    Lambda(Autowirer* pThis, cpp11::shared_ptr<ContextMember> interior, t_mpName::iterator q):
      pThis(pThis),
      interior(interior),
      q(q)
    {}

    Autowirer* pThis;
    cpp11::shared_ptr<ContextMember> interior;
    t_mpName::iterator q;

    void operator()(ContextMember*) {
      // Remove the elment we inserted earlier
      boost::lock_guard<boost::mutex> lk(pThis->m_lock);
      pThis->m_byName.erase(q);
    }
  };

  // Insert and retain the iterator for later removal
  q = m_byName.insert(q, t_mpName::value_type(name, cpp11::shared_ptr<ContextMember>()));
  q->second = cpp11::shared_ptr<ContextMember>(
    pContextMember,
    Lambda(this, interior, q)
  );

  // Wrap and hand up:
  return q->second;
}

cpp11::shared_ptr<CoreContext> CreateContextThunk(void) {
  return CoreContext::NewContext();
}
