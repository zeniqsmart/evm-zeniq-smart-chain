#!/usr/bin/env bash

cd ./evmwrap
make
cd ..

go build ./...

