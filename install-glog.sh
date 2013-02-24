#!/bin/bash
set -ex

VERSION="0.3.2"

pushd /tmp
rm -rf glog-$VERSION glog-$VERSION.tar.gz
wget http://google-glog.googlecode.com/files/glog-$VERSION.tar.gz || exit $?
tar -xzf glog-$VERSION.tar.gz || exit $?
pushd glog-$VERSION
./configure || exit $?
make -j4 || exit $?
sudo make install || exit $?
popd
rm -rf glog-$VERSION glog-$VERSION.tar.gz
popd

echo -e "\e[1;32msuccess\e[0m"
