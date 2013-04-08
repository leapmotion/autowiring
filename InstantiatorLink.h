#ifndef _INSTANTIATOR_LINK_H
#define _INSTANTIATOR_LINK_H

typedef void (*t_instantiator)(void);
struct InstantiatorLink {
  // Next instantiator in the chain
  InstantiatorLink* pFlink;

  // Function pointer at this link
  t_instantiator fn;
};

#endif