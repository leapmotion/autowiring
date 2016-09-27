// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#pragma once
#include <future>
#include <stdexcept>

namespace autowiring {
  #pragma pack(push, _CRT_PACKING)

  template<typename _Ret, typename... _ArgTypes>
  class _Packaged_state_unwrap:
    public std::_Associated_state<_Ret>
  {
  public:
    std::function<_Ret(_ArgTypes...)> _Fn;
  };

  template<typename _Ret, typename... _ArgTypes>
  class _Packaged_state_unwrap<_Ret&, _ArgTypes...>:
    public std::_Associated_state<_Ret*>
  {
  public:
    std::function<_Ret&(_ArgTypes...)> _Fn;
  };

  template<typename... _ArgTypes>
  class _Packaged_state_unwrap<void, _ArgTypes...>:
    public std::_Associated_state<int>
  {
  public:
    std::function<void(_ArgTypes...)> _Fn;
  };

  class _Task_async_state_unwrap:
    public std::_Packaged_state<void()>
  {
  public:
    ::Concurrency::task<void> _Task;
  };
  #pragma pack(pop)

  /// <summary>
  /// Platform-specific operation appending routine
  /// </summary>
  /// <param name="future">The future to be appended to</param>
  /// <param name="fn">The lambda to be executed after the future is ready</param>
  template<typename T, typename Fn>
  void then(std::future<T>& future, Fn&& fn) {
    // Need a pointer to the underlying state block so we can decide what to do
    auto ptr = future._Ptr();

    if (auto* taskAsync = dynamic_cast<std::_Task_async_state<T, 0>*>(ptr)) {
      auto* unwrap = reinterpret_cast<_Task_async_state_unwrap*>(taskAsync);
      unwrap->_Task.then(fn);
    } else if (auto* deferredAsync = dynamic_cast<std::_Deferred_async_state<T>*>(ptr)) {
      auto* packagedState = static_cast<std::_Packaged_state<T()>*>(deferredAsync);
      auto* unwrap = reinterpret_cast<_Packaged_state_unwrap<T>*>(packagedState);

      static_assert(
        sizeof(*packagedState) == sizeof(*unwrap),
        "Size of unwrapped version differs from underlying version, internal API has changed"
      );

      // New function which consists of a call to the original then a call to the continuation
      unwrap->_Fn = std::bind(
        [] (const std::function<T()>& orig, const Fn& fn) {
          auto rv = orig();
          fn();
          return rv;
        },
        std::move(unwrap->_Fn),
        std::forward<Fn&&>(fn)
      );
    }
    else
      throw std::runtime_error("Unrecognized future type");
  }
}
