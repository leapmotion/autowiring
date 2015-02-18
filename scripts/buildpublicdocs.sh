#!/usr/bin/env bash

#
# Run doxygen on the source and copy graphics
#

pushd $(dirname "$0")/..
scripts/processcodeexamples.sh
doxygen publicDoxyFile.conf
echo "Copying svg files"
cp -v devguide/diagrams/*.svg docs/html
popd 
