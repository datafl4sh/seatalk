#!/bin/sh

if [ ! -d "jiffy" ]; then
    git clone https://github.com/davisp/jiffy.git
fi

cd jiffy
make
cd ../serial
clang -O3 -o serial serial.c
cd ../translator
erlc common.erl
erlc seatalk.erl
erlc seatalkng.erl
cd ..

