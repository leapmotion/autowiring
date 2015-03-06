#!/usr/bin/env bash

#
# Ensure that the version.cmake version number is the same everywhere
#

# Get the version number from version.cmake first
if ! version=$(grep -oE "[0-9]+.[0-9]+.[0-9]+" version.cmake); then
  echo "Version number not found in version.cmake"
  exit 1
fi

# Verify that this identical version number appears in our doxygen files
if ! grep $version Doxyfile; then
  echo "Expected to find version $version in Doxyfile"
  exit 1
fi
if ! grep $version publicDoxyfile.conf; then
  echo "Expected to find version $version in publicDoxyfile.conf"
  exit 1
fi
