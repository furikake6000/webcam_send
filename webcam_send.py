import sys
ros_cv_path = '/opt/ros/kinetic/lib/python2.7/dist-packages'
if(ros_cv_path in sys.path):
    sys.path.remove(ros_cv_path)
import numpy as np
import cv2
import threading

from socket import socket, AF_INET, SOCK_STREAM

WIDTH = 320
HEIGHT = 240

ADDR = 'localhost'
if(len(sys.argv) > 1):
    ADDR = sys.argv[1]

def capture_camera(sock, mirror=True):
    """Capture video from camera"""
    # カメラをキャプチャする
    cap = cv2.VideoCapture(0) # 0はカメラのデバイス番号

    try:
        while True:
            _, frame = cap.read()

            # 鏡のように映るか否か
            if mirror is True:
                frame = frame[:,::-1]

            # フレームをリサイズ
            frame = cv2.resize(frame, (WIDTH, HEIGHT))

            data = frame.tobytes()
            while len(data) > 0:
                data_part = data[:2048]
                data = data[2048:]
                sock.send(data_part)
            
            sock.send(b'_frame_')

    except KeyboardInterrupt:
        # キャプチャを解放する
        cap.release()
        cv2.destroyAllWindows()
        sock.close()

if __name__ == '__main__':
    s = socket(AF_INET, SOCK_STREAM)
    #to_addr = (ADDR, 3000)
    # connect to server
    s.connect((ADDR, 3000))

    capture_camera(s)
    
