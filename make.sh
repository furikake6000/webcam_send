#!/bin/bash

docker run -it --rm \
    -v ${PWD}:/root/workdir \
    -w "/root/workdir" \
    imai-laboratory/webcam_recv:latest \
    bash -c "mkdir build \
    && cd build \
    && cmake ../ \
    && make"