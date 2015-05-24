#!/bin/sh

ROOT=${PWD}



# header

echo "copy headers ..."
if [ ! -d "../../libs/include/cefix" ]; then
  svn mkdir ../../libs/include/cefix
fi
rm ../../libs/include/cefix/*
cp -a ../include/cefix/* ../../libs/include/cefix/
svn status ../../libs/include/cefix | grep '^!' | awk '{print $2}' | xargs svn delete --force
svn add --force ../../libs/include/cefix/*
#libs

echo "copy libs ..."
cp -a build/Release-universal/libcefix.a ../../libs/lib/libcefix.a
cp -a build/Debug-universal/libcefix.a ../../libs/lib/libcefixd.a

svn add ../../libs/lib/libcefix.a
svn add ../../libs/lib/libcefixd.a

echo "committing ..."
svn commit --username=hudson --password=hudson ../../libs/ -m "new cefix ios libs"

