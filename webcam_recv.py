import sys
ros_cv_path = '/opt/ros/kinetic/lib/python2.7/dist-packages'
if(ros_cv_path in sys.path):
    sys.path.remove(ros_cv_path)
import cv2
import numpy as np
import threading
import struct

from socket import socket, AF_INET, SOCK_STREAM

WIDTH = 320
HEIGHT = 240

if __name__ == '__main__':
    s = socket(AF_INET, SOCK_STREAM)
    # listen to another device
    s.bind(('', 3000))
    s.listen(1)
    conn, addr = s.accept() # Get connection
    print('Connected by', addr)

    lastbufdata = b''
    while True:
        data = lastbufdata
        while True:
            receivedstr=conn.recv(1024*8)
            index = receivedstr.find(b'_frame_')
            if(index != -1):
                data += receivedstr[:index]
                lastbufdata = receivedstr[(index + 7):]
                break
            data += receivedstr
        if not data:
           break

        if len(data) < WIDTH * HEIGHT * 3:
            data += [0] * (WIDTH * HEIGHT * 3 - len(data))
        elif len(data) > WIDTH * HEIGHT * 3:
            data = data[:WIDTH * HEIGHT * 3]

        received_frame = np.frombuffer(data, dtype=np.uint8).reshape((HEIGHT, WIDTH, 3))

        cv2.imshow('camera capture', received_frame)
        k = cv2.waitKey(1) # wait 1msec
        if k == 113: # exit with Q
            break

    s.close()
