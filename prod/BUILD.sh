#!/usr/bin/env bash

git pull

if [ -d "build" ]; then
  rm -rf build
else
  echo "Directory build doesn't exists."
fi

cmake --preset debug
cmake --build --preset debug 