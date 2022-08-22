#!/bin/sh
#
# make rtkrcv cui applications by gcc
#

echo; echo % fusion/gcc
cd gcc
make $1
cd ../..

