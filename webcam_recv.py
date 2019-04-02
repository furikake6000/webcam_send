import numpy as np
import cv2
import threading
import struct

from socket import socket, AF_INET, SOCK_STREAM

WIDTH = 640
HEIGHT = 480

if __name__ == '__main__':
    s = socket(AF_INET, SOCK_STREAM)
    # listen to another device
    s.bind(('', 3000))
    s.listen(1)
    conn, addr = s.accept() # Get connection
    print('Connected by', addr)

    while True:
        data = b''
        remained_data = WIDTH * HEIGHT * 3
        while remained_data > 0:
            receivedstr=conn.recv(min(1024*8, remained_data))
            remained_data -= len(receivedstr)
            data += receivedstr
        if not data:
           break

        received_frame = np.frombuffer(data, dtype=np.uint8).reshape((HEIGHT, WIDTH, 3))

        cv2.imshow('camera capture', received_frame)
        k = cv2.waitKey(1) # 1msec待つ
        if k == 113: # Qキーで終了
            break

    s.close()
