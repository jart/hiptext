#!/bin/bash
set -ex

VERSION="2.0"

pushd /tmp
rm -rf gflags-$VERSION gflags-$VERSION.tar.gz
wget http://gflags.googlecode.com/files/gflags-$VERSION.tar.gz || exit $?
tar -xzf gflags-$VERSION.tar.gz || exit $?
pushd gflags-$VERSION
./configure || exit $?
make -j4 || exit $?
sudo make install || exit $?
popd
rm -rf gflags-$VERSION gflags-$VERSION.tar.gz
popd

echo -e "\e[1;32msuccess\e[0m"
