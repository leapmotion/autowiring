#include "stdafx.h"
#include "SimpleOwnershipValidator.h"
#include <assert.h>
#include <iostream>

using namespace std;

SimpleOwnershipValidator::~SimpleOwnershipValidator()
{
  // Find violating entries, gather them, and destroy our array:
  vector<const type_info*> violating;
  for(size_t i = 0; i < m_entries.size(); i++) {
    auto cur = m_entries[i];
    if(!cur->Validate())
      violating.push_back(&cur->GetType());
    delete cur;
  }

  if(!IS_INTERNAL_BUILD || violating.empty())
    return;

  // If no listeners, we can only assert:
  assert(!listeners.empty());

  // And now notify all listeners:
  for(size_t i = 0; i < listeners.size(); i++)
    listeners[i](violating);
}

void SimpleOwnershipValidator::PrintToStdErr(const std::vector<const type_info*>& violating) {
  cerr
    << "A context is being destroyed, but some of its members are not being destroyed at the." << endl
    << "same time.  The user has indicated that this context was supposed to have simple" << endl
    << "ownership over its members.  The following types are participants in what may be a" << endl
    << "cycle:" << endl;

  for(size_t i = 0; i < violating.size(); i++)
    cerr << violating[i]->name() << endl;
}

void SimpleOwnershipValidator::AddValidationListener(const std::function < void(const std::vector<const type_info*>&)>& fn)
{
  listeners.push_back(fn);
}