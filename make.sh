#!/bin/bash

docker run -it --rm \
    -v ${PWD}:/root/workdir \
    -w "/root/workdir" \
    imai-laboratory/webcam_recv:latest \
    bash -c "rm build/* -rf \
    && mkdir -p build \
    && cd build \
    && cmake ../ \
    && make"
