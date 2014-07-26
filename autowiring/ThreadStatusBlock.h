// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include <vector>
#include MEMORY_HEADER

class CoreContext;

/// <summary>
/// Represents a status block for the current thread
/// </summary>
class ThreadStatusBlock
{
protected:
  ThreadStatusBlock(void);

public:
  ~ThreadStatusBlock(void);

  /// <returns>
  /// The status block for the calling thread
  /// </returns>
  static ThreadStatusBlock* Get(void);

  struct CurrentSpace {
    void* pSpace;
    size_t nBytes;

    bool Contains(const void* ptr) const {
      return pSpace <= ptr && ptr < ((char*) pSpace + nBytes);
    }
  };

  // Constructive object stack.  The top of the stack denotes the object that is
  // currently being constructed.  This is useful to obtain information about the
  // slots that a ContextMember contains, and whether an Autowired field is a
  // candidate for deferred satisfaction.
  std::vector<CurrentSpace> m_spaces;

  // The currently active context for this thread:
  std::shared_ptr<CoreContext> m_context;
};
