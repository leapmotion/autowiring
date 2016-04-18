#!/bin/bash
# Renames Boost to Autoboost to avoid namespace collisions


if [[ -n "$1" ]]; then
  directory=$1
else
  directory="."
fi

filelist=`find ${directory} -iname "*.h" -o -iname "*.cpp" -o -iname "*.hpp" -o -iname "*.ipp"`

sed -ri 's/([^\.o])boost/\1autoboost/g' ${filelist}
sed -ri 's/([^\.O])BOOST/\1AUTOBOOST/g' ${filelist}
sed -ri 's/[[:space:]]*$//g' ${filelist}
