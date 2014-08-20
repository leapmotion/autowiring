#!/usr/bin/env bash

#
# Enforce spaces instead of tabs
#

ENFORCED_FILES="autowiring examples src CMakeLists.txt"

# Go to root directory

if grep -r $'\t' $ENFORCED_FILES
then 
  #echo "The Autowiring project prohibits tabs for spacing."
  #echo "Tabs found in the following files:"
  #grep -rn "\t" $ENFORCED_FILES
  exit 1
fi
