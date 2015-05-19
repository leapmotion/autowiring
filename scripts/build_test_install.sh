#!/usr/bin/env bash

make -j 8 || make || exit 1

ctest --output-on-failure && sudo make install || exit 2

sudo cpack || (cat _CPack_Packages/Linux/TGZ/InstallOutput.log; exit 3)

# Build examples from installed Autowiring
cd examples
cmake . -Dautowiring_ARCHITECTURE=x64 && make || exit 4
