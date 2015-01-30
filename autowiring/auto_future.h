#pragma once
#include <functional>

#ifdef _MSC_VER
#include "auto_future_win.h"
#elif defined(__APPLE__)
#include "auto_future_mac.h"
#endif
