#!/bin/sh
set -ex
aclocal --install --force -I config
autoreconf --install --force --verbose -I config
