C++11 Shim
==========

This module acts as a c++11 shim when building for platforms without the c++11 STL. These shims work by injecting boost equivalents of c++11 types and functions into the `std` namespace. `cpp11.h` defines macros that can used to include the shim when required.

```cpp
#include THREAD_HEADER //equivalent to <thread> on c++11 platforms
#include MUTEX_HEADER //equivalent to <mutex> on c++11 platforms
```
