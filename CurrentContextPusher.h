#ifndef _CURRENT_CONTEXT_PUSHER_H
#define _CURRENT_CONTEXT_PUSHER_H

class CoreContext;

/// <summary>
/// Saves the current context and makes the passed one current, and then
/// restores the current context when destroyed
/// </summary>
class CurrentContextPusher
{
public:
  CurrentContextPusher(std::shared_ptr<CoreContext> pContext);
  CurrentContextPusher(CoreContext* pContext);
  ~CurrentContextPusher(void);

private:
  std::shared_ptr<CoreContext> m_prior;
};

#endif
