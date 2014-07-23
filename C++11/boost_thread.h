#pragma once

#include <boost/thread/thread.hpp>

namespace std {
  using boost::thread;
  namespace this_thread = boost::this_thread;
}
