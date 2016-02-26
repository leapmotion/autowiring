// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include <chrono>
#include <iostream>
#include <iomanip>

struct PrintableDuration {
  PrintableDuration(std::chrono::duration<double> duration) :
    duration(duration)
  {}

  const std::chrono::duration<double> duration;

  std::ostream& Print(std::ostream& os) const {
    if (duration < std::chrono::microseconds(1))
      return os << duration.count() * 1e9 << " ns";

    os << std::fixed << std::setprecision(3);

    // Decide which units to print format, print three decimals of accuracy:
    if (duration < std::chrono::milliseconds(1))
      return os << duration.count() * 1e6 << " us";

    if (duration < std::chrono::seconds(1))
      return os << duration.count() * 1e3 << " ms";

    return os << duration.count() << " s";
  }
};

inline std::ostream& operator<<(std::ostream& os, const PrintableDuration& rhs) {
  return rhs.Print(os);
}
