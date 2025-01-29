#!/bin/bash

arch=$1
version="0.1"

mkdir -p bin

if [ "$arch" = "linux" ]; then
    CGO_ENABLED=1 CC=x86_64-linux-gnu-gcc GOOS=linux GOARCH=amd64 go build -ldflags "-linkmode external -s -w" -o bin/animation-tester-$version-linux-amd64
elif [ "$arch" = "macos" ]; then
    CGO_ENABLED=1 CC=x86_64-apple-darwin24-clang GOOS=darwin GOARCH=amd64 go build -ldflags "-linkmode external -s -w '-extldflags=-mmacosx-version-min=10.15'" -o bin/animation-tester-$version-macos-amd64
else
    echo "No architecture given. Should be 'linux' or 'macos'";
fi


