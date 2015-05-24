#!/bin/sh

ROOT=${PWD}
XCODEBUILD=/Applications/Xcode.app/Contents/Developer/usr/bin/xcodebuild
if [ ! -f ${XCODEBUILD} ]; then
  XCODEBUILD=/Developer/usr/bin/xcodebuild
fi

DEVICE=iphoneos
SIMULATOR=iphonesimulator
ACTION="build"
BUILD_CEFIX=${1}


build_project () {
  TARGET=${1}
  ${XCODEBUILD} -configuration Debug -target "${TARGET}" -sdk ${DEVICE} ${ACTION} RUN_CLANG_STATIC_ANALYZER=NO
  if [ $? -eq 1 ] ; then
    echo "compile went wrong"
    exit 1
  fi
  ${XCODEBUILD} -configuration Debug -target "${TARGET}" -sdk ${SIMULATOR} ${ACTION} RUN_CLANG_STATIC_ANALYZER=NO
  if [ $? -eq 1 ] ; then
    echo "compile went wrong"
    exit 1
  fi
  ${XCODEBUILD} -configuration Release -target "${TARGET}" -sdk ${DEVICE} ${ACTION} RUN_CLANG_STATIC_ANALYZER=NO
  if [ $? -eq 1 ] ; then
    echo "compile went wrong"
    exit 1
  fi
  ${XCODEBUILD} -configuration Release -target "${TARGET}" -sdk ${SIMULATOR} ${ACTION} RUN_CLANG_STATIC_ANALYZER=NO
  if [ $? -eq 1 ] ; then
    echo "compile went wrong"
    exit 1
  fi
}


build_project cefix-for-iphone

