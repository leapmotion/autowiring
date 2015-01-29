#pragma once
#include <future>
#include <stdexcept>

namespace autowiring {
  class _Task_async_state_unwrap:
    public std::_Packaged_state<void()>
  {
  public:
    ::Concurrency::task<void> _Task;
  };

  /// <summary>
  /// Platform-specific operation appending routine
  /// </summary>
  /// <param name="future">The future to be appended to</param>
  /// <param name="fn">The lambda to be executed after the future is ready</param>
  template<typename T, typename Fn>
  void then(std::future<T>& future, Fn&& fn) {
    auto ptr = future._Ptr();

    auto* taskAsync = dynamic_cast<std::_Task_async_state<T, 0>*>(ptr);
    if (taskAsync) {
      auto* unwrap = reinterpret_cast<_Task_async_state_unwrap*>(taskAsync);
      unwrap->_Task.then(fn);
      return;
    }

    throw std::runtime_error("Unrecognized future type");
  }
}