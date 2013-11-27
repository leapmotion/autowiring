#include "stdafx.h"
#include "SimpleOwnershipValidator.h"
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

  // Time to show the user what went wrong:
  cerr
    << "A context is being destroyed, but some of its members are not being destroyed at the." << endl
    << "same time.  The user has indicated that this context was supposed to have simple" << endl
    << "ownership over its members.  The following types are participants in what may be a" << endl
    << "cycle:" << endl;

  for(size_t i = 0; i < violating.size(); i++)
    cerr << violating[i]->name() << endl;
}
