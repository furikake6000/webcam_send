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

ADDR = '192.168.1.13'

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

            # フレームを表示する
            # cv2.imshow('camera capture', frame)

            # encode_param = [int(cv2.IMWRITE_JPEG_QUALITY), 50]
            # _, encoded = cv2.imencode('.jpg', frame, encode_param)
            
            # print(encoded.tobytes())

            data = frame.tobytes()
            sock.send(data)
    except KeyboardInterrupt:
        # キャプチャを解放する
        cap.release()
        cv2.destroyAllWindows()
        sock.close()

if __name__ == '__main__':
    s = socket(AF_INET, SOCK_STREAM)
    # connect to server
    s.connect((ADDR, 3000))

    capture_camera(s)
    
