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
  return CoreContext::NewContext();
}