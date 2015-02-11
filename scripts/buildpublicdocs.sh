#!/usr/bin/env bash

#
# Run doxygen on the source and copy graphics
#

cd ..
scripts/processcodeexamples.sh
doxygen publicDoxyFile.conf
echo "Copy svg files"
cp -v devguide/diagrams/*.svg docs/html
cd scripts
