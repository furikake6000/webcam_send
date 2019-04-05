#!/bin/bash

if [[ "$OSTYPE" == "darwin"* ]]; then
  _DISPLAY="$(ip -f inet -o addr show eno1|cut -d\  -f 7 | cut -d/ -f 1):0"
  socat TCP-LISTEN:6000,reuseaddr,fork UNIX-CLIENT:\"$DISPLAY\" &
else
  _DISPLAY=$DISPLAY
fi

xhost +

nvidia-docker run -it --rm \
    --shm-size=256m \
    -e QT_X11_NO_MITSHM=1 \
    -e DISPLAY=$_DISPLAY \
    -v ${PWD}:/root/workdir \
    -v /tmp/.X11-unix/:/tmp/.X11-unix \
    -w "/root/workdir" \
    -p 3000:3000 \
    imai-laboratory/webcam_recv:latest \
    bash -c "python3 webcam_recv_openpose.py"