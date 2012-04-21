#!/bin/sh

# Build tests
make
if [[ $? != 0 ]] ; then
  exit $rc
fi

# Run all tests
./operators
./apply2
