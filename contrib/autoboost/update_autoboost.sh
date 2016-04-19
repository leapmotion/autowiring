#!/bin/bash
# Copies relevant files from a plain boost install

if [[ -z $1 ]]; then
  echo "Must specify a path to a raw boost install"
  exit
fi

if [[ ! -d $1 ]]; then
  echo "Must specify a valid path"
  exit
fi

boostroot=$(realpath $1)

rm -rf ./autoboost
rm -rf ./libs

while read line; do
  cpfile=${line##*/}
  directory=${line%/*}

  if [[ -n ${cpfile// /} ]]; then
    cpfile="*"
  fi

  mkdir -p ${directory}
  pushd ${directory} >> /dev/null
  cp ${boostroot}/${directory}/${cpfile} . 2>&1 | grep -v 'omitting directory'
  popd >> /dev/null
done <AUTOBOOST_MANIFEST

mv ./boost ./autoboost

filelist=`find . -iname "*.h" -o -iname "*.cpp" -o -iname "*.hpp" -o -iname "*.ipp"`

echo "changing boost to autoboost"
sed -ri 's/(^|[^\.o])boost/\1autoboost/g' ${filelist}
echo "changing BOOST to AUTOBOOST"
sed -ri 's/(^|[^\.O])BOOST/\1AUTOBOOST/g' ${filelist}
echo "removing trailing whitespace"
sed -ri 's/[[:space:]]*$//g' ${filelist}
