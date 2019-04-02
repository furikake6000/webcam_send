import sys
ros_cv_path = '/opt/ros/kinetic/lib/python2.7/dist-packages'
if(ros_cv_path in sys.path):
    sys.path.remove(ros_cv_path)
import cv2
import numpy as np
import threading
import struct

from socket import socket, AF_INET, SOCK_DGRAM

WIDTH = 320
HEIGHT = 240

if __name__ == '__main__':
    s = socket(AF_INET, SOCK_DGRAM)
    # listen to another device
    s.bind(('', 3000))
    #s.listen(1)
    #conn, addr = s.accept() # Get connection
    #print('Connected by', addr)

    while True:
        data = b''
        remained_data = WIDTH * HEIGHT * 3
        while remained_data > 0:
            receivedstr, _=s.recvfrom(min(1024*8, remained_data))
            index = receivedstr.find(b'_frame_')
            if(index != -1):
                receivedstr = receivedstr[:index]
                data = b''
                remained_data = WIDTH * HEIGHT * 3
            remained_data -= len(receivedstr)
            data += receivedstr
        if not data:
           break

        received_frame = np.frombuffer(data, dtype=np.uint8).reshape((HEIGHT, WIDTH, 3))

        cv2.imshow('camera capture', received_frame)
        k = cv2.waitKey(1) # wait 1msec
        if k == 113: # exit with Q
            break

    s.close()
